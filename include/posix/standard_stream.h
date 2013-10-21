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

#ifndef POSIX_STANDARD_STREAM_H_
#define POSIX_STANDARD_STREAM_H_

#include <bitset>

namespace posix
{
enum StandardStream
{
    stdin = 0,
    stdout = 1,
    stderr = 2
};
typedef std::bitset<3> StandardStreamFlags;
}
#endif // POSIX_STANDARD_STREAM_H_
