/*
 * Copyright © 2013 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Thomas Voß <thomas.voss@canonical.com>
 */

#include <core/posix/child_process.h>

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

#include <atomic>
#include <fstream>
#include <mutex>
#include <unordered_map>

#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

#include <sys/eventfd.h>
#include <sys/signalfd.h>

namespace io = boost::iostreams;

namespace
{
struct SignalFdDeathObserver : public core::posix::ChildProcess::DeathObserver
{
    enum class State
    {
        not_running,
        running
    };

    SignalFdDeathObserver()
        : signal_fd(-1),
          wakeup_fd(-1),
          state(State::not_running)
    {
        ::sigemptyset(&mask);
        ::sigaddset(&mask, SIGCHLD);

        if (::sigprocmask(SIG_BLOCK, &mask, &old_process_mask) == -1)
            throw std::system_error(errno, std::system_category());

        static const int empty_flags = 0;
        signal_fd = ::signalfd(signal_fd, &mask, empty_flags);

        if (signal_fd == -1)
            throw std::system_error(errno, std::system_category());

        static const unsigned int initial_value = 0;
        wakeup_fd = ::eventfd(initial_value, empty_flags);

        if (wakeup_fd == -1)
            throw std::system_error(errno, std::system_category());
    }

    ~SignalFdDeathObserver()
    {
        ::sigprocmask(SIG_SETMASK, &old_process_mask, nullptr);
        ::close(signal_fd);
        ::close(wakeup_fd);
    }

    bool add(const core::posix::ChildProcess& process) override
    {
        if (process.pid() == -1)
            return false;

        std::lock_guard<std::mutex> lg(guard);

        bool added = false;
        auto new_process = std::make_pair(process.pid(), process);
        std::tie(std::ignore, added) = children.insert(new_process);

        if (added)
        {
            // The process may have died between it's instantiation and it
            // being added to the children map. Check that it's still alive.
            int status{-1};
            pid_t pid = ::waitpid(process.pid(), &status, WNOHANG);

            if (pid != 0) // child no longer alive
            {
                // we missed the SIGCHLD signal so we must now manually
                // inform our subscribers.
                signals.child_died(new_process.second);
                children.erase(new_process.first);
                return false;
            }
        }

        return added;
    }

    bool has(const core::posix::ChildProcess& process) const override
    {
        std::lock_guard<std::mutex> lg(guard);
        return children.count(process.pid()) > 0;
    }

    const core::Signal<core::posix::ChildProcess>& child_died() const override
    {
        return signals.child_died;
    }

    void run(std::error_code& ec) override
    {
        if (state.load() == State::running)
            throw std::runtime_error("DeathObserver::run can only be run once.");

        state.store(State::running);

        sigset_t old_mask;

        if (::pthread_sigmask(SIG_SETMASK, &mask, &old_mask) == -1)
        {
            ec = std::error_code(errno, std::system_category());
            return;
        }

        signalfd_siginfo signal_info;
        ::memset(&signal_info, 0, sizeof(signal_info));

        static const int signal_fd_idx = 0;
        static const int wakeup_fd_idx = 1;

        pollfd fds[2];
        fds[0] = {signal_fd, POLLIN, 0};
        fds[1] = {wakeup_fd, POLLIN, 0};

        while (true)
        {
            auto rc = ::poll(fds, 2, -1);

            if (rc == -1)
            {
                if (errno == EINTR)
                    continue;

                ec = std::error_code(errno, std::system_category());
                break;
            }

            if (rc == 0)
                continue;

            if (fds[signal_fd_idx].revents == POLLIN)
            {
                auto result = ::read(signal_fd, &signal_info, sizeof(signal_info));
                if (result != sizeof(signal_info))
                {
                    ec = std::error_code(errno, std::system_category());
                }

                switch(signal_info.ssi_signo)
                {
                case SIGCHLD:
                {
                    pid_t pid{-1}; int status{-1};
                    while (true)
                    {
                        pid = ::waitpid(-1, &status, WNOHANG);

                        if (pid <= 0 && errno != EINTR) // No more children
                            break;

                        {
                            std::lock_guard<std::mutex> lg(guard);
                            auto it = children.find(pid);

                            if (it != children.end())
                            {
                                if (WIFSIGNALED(status) || WIFEXITED(status))
                                {
                                    signals.child_died(it->second);
                                    children.erase(it);
                                }
                            }
                        }
                    }
                    break;
                }
                default:
                    break;
                }
            }

            if (fds[wakeup_fd_idx].revents == POLLIN)
            {
                std::int64_t value{1};
                if (::read(wakeup_fd, &value, sizeof(value)) != sizeof(value))
                    ec = std::error_code(errno, std::system_category());
                break;
            }
        }

        if (pthread_sigmask(SIG_SETMASK, &old_mask, nullptr) == -1)
            ec = std::error_code(errno, std::system_category());

        state.store(State::not_running);
    }

    void quit() override
    {
        static const std::int64_t value = {1};
        if (sizeof(value) != ::write(wakeup_fd, &value, sizeof(value)))
            throw std::system_error(errno, std::system_category());
    }

    sigset_t mask;
    sigset_t old_process_mask;
    int signal_fd;
    int wakeup_fd;
    std::atomic<State> state;
    mutable std::mutex guard;
    std::unordered_map<pid_t, core::posix::ChildProcess> children;
    struct
    {
        core::Signal<core::posix::ChildProcess> child_died;
    } signals;
};
}

namespace core
{
namespace posix
{
ChildProcess::DeathObserver& ChildProcess::DeathObserver::instance()
{
    static SignalFdDeathObserver observer;
    return observer;
}

ChildProcess::Pipe ChildProcess::Pipe::invalid()
{
    static Pipe p;
    static std::once_flag flag;

    std::call_once(flag, [&]() { p.close_read_fd(); p.close_write_fd(); });

    return p;
}

ChildProcess::Pipe::Pipe()
{
    int rc = ::pipe(fds);

    if (rc == -1)
        throw std::system_error(errno, std::system_category());
}

ChildProcess::Pipe::Pipe(const ChildProcess::Pipe& rhs) : fds{-1, -1}
{
    if (rhs.fds[0] != -1)
        fds[0] = ::dup(rhs.fds[0]);

    if (rhs.fds[1] != -1)
        fds[1] = ::dup(rhs.fds[1]);
}

ChildProcess::Pipe::~Pipe()
{
    if (fds[0] != -1)
        ::close(fds[0]);
    if (fds[1] != -1)
        ::close(fds[1]);
}

int ChildProcess::Pipe::read_fd() const
{
    return fds[0];
}

void ChildProcess::Pipe::close_read_fd()
{
    if (fds[0] != -1)
    {
        ::close(fds[0]);
        fds[0] = -1;
    }
}

int ChildProcess::Pipe::write_fd() const
{
    return fds[1];
}

void ChildProcess::Pipe::close_write_fd()
{
    if (fds[1] != -1)
    {
        ::close(fds[1]);
        fds[1] = -1;
    }
}

ChildProcess::Pipe& ChildProcess::Pipe::operator=(const ChildProcess::Pipe& rhs)
{
    if (fds[0] != -1)
        ::close(fds[0]);
    if (fds[1] != -1)
        ::close(fds[1]);

    if (rhs.fds[0] != -1)
        fds[0] = ::dup(rhs.fds[0]);
    else
        fds[0] = -1;
    if (rhs.fds[1] != -1)
        fds[1] = ::dup(rhs.fds[1]);
    else
        fds[1] = -1;

    return *this;
}

struct ChildProcess::Private
{
    // stdin and stdout are always "relative" to the childprocess, i.e., we
    // write to stdin of the child process and read from its stdout.
    Private(pid_t pid,
            const ChildProcess::Pipe& stderr,
            const ChildProcess::Pipe& stdin,
            const ChildProcess::Pipe& stdout)
        : pipes{stderr, stdin, stdout},
          serr(pipes.stderr.read_fd(), io::never_close_handle),
          sin(pipes.stdin.write_fd(), io::never_close_handle),
          sout(pipes.stdout.read_fd(), io::never_close_handle),
          cerr(&serr),
          cin(&sin),
          cout(&sout),
          original_parent_pid(::getpid()),
          original_child_pid(pid)
    {
    }

    ~Private()
    {
        // Check if we are in the original parent process.
        if (original_parent_pid == getpid())
        {
            // If so, check if we are considering a valid pid here.
            // If so, we kill the original child.
            if (original_child_pid != -1)
                ::kill(original_child_pid, SIGKILL);
        }
    }

    struct
    {
        ChildProcess::Pipe stdin;
        ChildProcess::Pipe stdout;
        ChildProcess::Pipe stderr;
    } pipes;
    io::stream_buffer<io::file_descriptor_source> serr;
    io::stream_buffer<io::file_descriptor_sink> sin;
    io::stream_buffer<io::file_descriptor_source> sout;
    std::istream cerr;
    std::ostream cin;
    std::istream cout;

    // We need to store the original parent pid as we might have been forked
    // and with our automatic cleanup in place, it might happen that the d'tor
    // is called from the child process.
    pid_t original_parent_pid;
    pid_t original_child_pid;
};

ChildProcess ChildProcess::invalid()
{
    // We take the init process as child.
    static const pid_t invalid_pid = 1;
    return ChildProcess(invalid_pid, Pipe::invalid(), Pipe::invalid(), Pipe::invalid());
}

ChildProcess::ChildProcess(pid_t pid,
                           const ChildProcess::Pipe& stdin_pipe,
                           const ChildProcess::Pipe& stdout_pipe,
                           const ChildProcess::Pipe& stderr_pipe)
    : Process(pid),
      d(new Private{pid, stdin_pipe, stdout_pipe, stderr_pipe})
{
}

ChildProcess::~ChildProcess()
{
}

wait::Result ChildProcess::wait_for(const wait::Flags& flags)
{
    int status = -1;
    pid_t result_pid = ::waitpid(pid(), std::addressof(status), static_cast<int>(flags));

    if (result_pid == -1)
        throw std::system_error(errno, std::system_category());

    wait::Result result;

    if (result_pid == 0)
    {
        result.status = wait::Result::Status::no_state_change;
        return result;
    }

    if (WIFEXITED(status))
    {
        result.status = wait::Result::Status::exited;
        result.detail.if_exited.status = static_cast<exit::Status>(WEXITSTATUS(status));
    } else if (WIFSIGNALED(status))
    {
        result.status = wait::Result::Status::signaled;
        result.detail.if_signaled.signal = static_cast<Signal>(WTERMSIG(status));
        result.detail.if_signaled.core_dumped = WCOREDUMP(status);
    } else if (WIFSTOPPED(status))
    {
        result.status = wait::Result::Status::stopped;
        result.detail.if_stopped.signal = static_cast<Signal>(WSTOPSIG(status));
    } else if (WIFCONTINUED(status))
    {
        result.status = wait::Result::Status::continued;
    }

    return result;
}

std::istream& ChildProcess::cerr()
{
    return d->cerr;
}

std::ostream& ChildProcess::cin()
{
    return d->cin;
}

std::istream& ChildProcess::cout()
{
    return d->cout;
}
}
}
