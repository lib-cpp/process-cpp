/*
 * Copyright © 2012-2013 Canonical Ltd.
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

#include <testing/fork_and_run.h>

#include <posix/exit.h>
#include <posix/fork.h>
#include <posix/wait.h>

testing::ForkAndRunResult testing::operator|(
        testing::ForkAndRunResult lhs,
        testing::ForkAndRunResult rhs)
{
    return static_cast<testing::ForkAndRunResult>(
                static_cast<unsigned int> (lhs) | static_cast<unsigned int>(rhs));
}

testing::ForkAndRunResult testing::operator&(
        testing::ForkAndRunResult lhs,
        testing::ForkAndRunResult rhs)
{
    return static_cast<testing::ForkAndRunResult>(
                static_cast<unsigned int> (lhs) & static_cast<unsigned int>(rhs));
}

testing::ForkAndRunResult testing::fork_and_run(
        const std::function<posix::exit::Status()>& service,
        const std::function<posix::exit::Status()>& client)
{
    testing::ForkAndRunResult result = testing::ForkAndRunResult::empty;

    auto service_process = posix::fork(service, posix::StandardStream::empty);
    auto client_process = posix::fork(client, posix::StandardStream::empty);

    auto client_result = client_process.wait_for(posix::wait::Flags::untraced);

    switch (client_result.status)
    {
    case posix::wait::Result::Status::exited:
        if (client_result.detail.if_exited.status == posix::exit::Status::failure)
            result = result | testing::ForkAndRunResult::client_failed;
        break;
    default:
        result = result | testing::ForkAndRunResult::client_failed;
        break;
    }

    service_process.send_signal_or_throw(posix::Signal::sig_kill);
    auto service_result = service_process.wait_for(posix::wait::Flags::untraced);

    switch (service_result.status)
    {
    case posix::wait::Result::Status::exited:
        if (service_result.detail.if_exited.status == posix::exit::Status::failure)
            result = result | testing::ForkAndRunResult::service_failed;
        break;
    default:
        result = result | testing::ForkAndRunResult::service_failed;
        break;
    }

    return result;
}
