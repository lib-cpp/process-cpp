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

#include <posix/process.h>

#include <posix/signal.h>

#include <sys/types.h>
#include <unistd.h>

#include <iostream>

namespace posix
{

namespace
{
// We have to cleanup any fork'd children.
struct Cleanup
{
    ~Cleanup()
    {
        static const pid_t any_child = -1;
        auto ignored = ::waitpid(any_child, nullptr, 0);
        (void) ignored;
    }
} cleanup;
}

struct Process::Private
{
    pid_t pid;
};

Process::Process(pid_t pid)
    : d(new Private{pid})
{
}

Process::~Process() noexcept
{
}

pid_t Process::pid() const
{
    return d->pid;
}

void Process::send_signal_or_throw(const Signal& signal)
{
    auto result = ::kill(d->pid, static_cast<int>(signal));

    if (result == -1)
        throw std::system_error(errno, std::system_category());
}

bool Process::send_signal(const Signal& signal, std::system_error& e) noexcept
{
    auto result = ::kill(d->pid, static_cast<int>(signal));

    if (result == -1)
    {
        e = std::system_error(errno, std::system_category());
        return false;
    }

    return true;
}

pid_t Process::process_group_id_or_throw() const
{
    pid_t pgid = ::getpgid(pid());

    if (pgid == -1)
        throw std::system_error(errno, std::system_category());

    return pgid;
}

std::tuple<pid_t, bool> Process::process_group_id(std::system_error& se) const noexcept(true)
{
    pid_t pgid = ::getpgid(pid());

    if (pgid == -1)
    {
        se = std::system_error(errno, std::system_category());
        return std::make_tuple(pgid, false);
    }

    return std::make_tuple(pgid, true);
}
}
