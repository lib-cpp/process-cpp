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

#ifndef POSIX_FORK_H_
#define POSIX_FORK_H_

#include <posix/child_process.h>
#include <posix/standard_stream.h>
#include <posix/visibility.h>

#include <functional>

namespace posix
{
/**
 * @brief fork forks a new process and executes the provided main function in the newly forked process.
 * @throws std::system_error in case of errors.
 * @param [in] main The main function of the newly forked process.
 * @param [in] flags Specify which standard streams should be redirected to the parent process.
 * @return An instance of ChildProcess in case of success.
 */
POSIX_DLL_PUBLIC ChildProcess fork(const std::function<int()>& main,
                  const StandardStreamFlags& flags);

/**
 * @brief fork vforks a new process and executes the provided main function in the newly forked process.
 * @throws std::system_error in case of errors.
 * @param [in] main The main function of the newly forked process.
 * @param [in] flags Specify which standard streams should be redirected to the parent process.
 * @return An instance of ChildProcess in case of success.
 */
POSIX_DLL_PUBLIC ChildProcess vfork(const std::function<int()>& main,
                   const StandardStreamFlags& flags);
}

#endif // POSIX_FORK_H_
