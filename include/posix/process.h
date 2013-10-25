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
     * @brief Returns an invalid instance for testing purposes.
     * @return An invalid instance.
     */
    static Process invalid();

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
     * @brief Sends a signal to the process.
     * @throws std::system_error in case of problems.
     * @param [in] signal The signal to be sent to the process.
     */
    virtual void send_signal_or_throw(const Signal& signal);

    /**
     * @brief Sends a signal to the process.
     * @param [in] signal The signal to be sent to the process.
     * @param [out] e Set to contain an error if an issue arises.
     */
    virtual bool send_signal(const Signal& signal, std::system_error& e) noexcept(true);

    /**
     * @brief Queries the id of the process group this process belongs to.
     * @throw std::system_error in case of errors.
     * @return The id of the process group this process belongs to.
     */
    virtual pid_t process_group_id_or_throw() const;

    /**
     * @brief Queries the id of the process group this process belongs to.
     *
     * @return A tuple with the first element being the id of the process group
     * this process belongs to and the second element a boolean flag indicating
     * an error if true.
     */
    virtual std::tuple<pid_t, bool> process_group_id(std::system_error& se) const noexcept(true);

protected:
    friend const Process& posix::this_process::instance();
    friend Process posix::this_process::parent() noexcept(true);
    explicit Process(pid_t pid);

private:
    struct Private;
    std::shared_ptr<Private> d;
};
}
#endif // POSIX_PROCESS_H_
