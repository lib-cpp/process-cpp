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

#include <functional>

namespace posix
{
ChildProcess fork(const std::function<int()>& main,
                  const StandardStreamFlags& flags);

ChildProcess vfork(const std::function<int()>& main,
                   const StandardStreamFlags& flags);
}

#endif // POSIX_FORK_H_
