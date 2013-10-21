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

#ifndef POSIX_WAIT_H_
#define POSIX_WAIT_H_

#include <posix/exit.h>
#include <posix/signal.h>

#include <bitset>

#include <sys/wait.h>

namespace posix
{
struct Wait
{
    Wait() = delete;
    ~Wait() = delete;

    enum Flag
    {
        continued = WCONTINUED,
        untraced = WUNTRACED,
        no_hang = WNOHANG
    };
    typedef std::uint32_t Flags;

    struct Result
    {
        enum class Status
        {
            undefined,
            no_state_change,
            exited,
            signaled,
            stopped,
            continued
        } status = Status::undefined;

        union
        {
            struct
            {
                Exit::Status status;
            } if_exited;
            struct
            {
                Signal signal;
                bool core_dumped;
            } if_signaled;
            struct
            {
                Signal signal;
            } if_stopped;
        } detail;
    };
};
}

#endif // POSIX_WAIT_H_
