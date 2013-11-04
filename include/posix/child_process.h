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

#ifndef POSIX_CHILD_PROCESS_H_
#define POSIX_CHILD_PROCESS_H_

#include <posix/process.h>
#include <posix/standard_stream.h>
#include <posix/visibility.h>

#include <iosfwd>
#include <functional>

namespace posix
{
/**
 * @brief The Process class models a child process of this process.
 *
 * In addition to the functionality offered by the Process class, an instance
 * of ChildProcess offers functionality to wait for status changes of the child
 * process and to access the child process's standard streams if they have been
 * redirected when forking or exec'ing.
 */
class POSIX_DLL_PUBLIC ChildProcess : public Process
{
public:
    /**
     * @brief Creates an invalid ChildProcess.
     * @return An invalid ChildProcess instance.
     */
    static ChildProcess invalid();

    ~ChildProcess();

    /**
     * @brief Wait for the child process to change state.
     * @param [in] flags Alters the behavior of the wait operation.
     * @return Result of the wait operation, as well as information about the
     * reasons for a child process's state change.
     */
    wait::Result wait_for(const wait::Flags& flags);

    /**
     * @brief Access this process's stderr.
     */
    std::istream& cerr();

    /**
     * @brief Access this process's stdin.
     */
    std::ostream& cin();

    /**
     * @brief Access this process's stdout.
     */
    std::istream& cout();

private:
    friend ChildProcess fork(const std::function<int()>&, const StandardStream&);
    friend ChildProcess vfork(const std::function<int()>&, const StandardStream&);

    class POSIX_DLL_LOCAL Pipe
    {
    public:
        static Pipe invalid();

        Pipe();
        Pipe(const Pipe& rhs);
        ~Pipe();

        Pipe& operator=(const Pipe& rhs);

        int read_fd() const;
        void close_read_fd();

        int write_fd() const;
        void close_write_fd();

    private:
        Pipe(int fds[2]);
        int fds[2];
    };

    POSIX_DLL_LOCAL ChildProcess(pid_t pid,
                                 const Pipe& stdin,
                                 const Pipe& stdout,
                                 const Pipe& stderr);

    struct POSIX_DLL_LOCAL Private;
    std::shared_ptr<Private> d;
};
}

#endif // POSIX_CHILD_PROCESS_H_
