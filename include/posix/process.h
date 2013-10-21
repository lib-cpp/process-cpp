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

#ifndef POSIX_PROCESS_H_
#define POSIX_PROCESS_H_

#include <posix/this_process.h>
#include <posix/wait.h>

#include <memory>
#include <system_error>

namespace posix
{
enum class Signal;
class Self;
class WaitFlags;

/**
 * @brief The Process class models a process and possible operations on it.
 */
class Process
{
public:
    /**
     * @brief Frees resources associated with the process.
     */
    virtual ~Process() noexcept;

    /**
     * @brief Query the pid of the process.
     * @return The pid of the process.
     */
    virtual pid_t pid() const;

    /**
     * @brief Query the parent of the process.
     * @return The parent of the process.
     */
    Process parent() const;

    /**
     * @brief Sends a signal to the process.
     * @throws std::system_error in case of problems.
     * @param [in] signal The signal to be sent to the process.
     */
    virtual void send_signal(const Signal& signal);

    /**
     * @brief Sends a signal to the process.
     * @param [in] signal The signal to be sent to the process.
     * @param [out] e Set to contain an error if an issue arises.
     */
    virtual void send_signal(const Signal& signal, std::system_error& e) noexcept;

protected:
    friend const Process& posix::this_process::instance();
    explicit Process(pid_t pid);

private:
    struct Private;
    std::shared_ptr<Private> d;
};
}
#endif // POSIX_PROCESS_H_
