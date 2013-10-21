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

#ifndef POSIX_LINUX_PROCESS_H_
#define POSIX_LINUX_PROCESS_H_

#include <posix/process.h>

#include <memory>
#include <system_error>

namespace posix
{
namespace linux
{
class Self;

class Process : public posix::Process
{
public:
    static const Self& self();
    static Self& mutable_self();

    virtual ~Process() noexcept;

    Process parent() const;
protected:
    explicit Process(pid_t pid);

private:
    struct Private;
    std::shared_ptr<Private> d;
};

class Self : public posix::Self
{
public:
    Self(const Self&) = default;
    Self& operator=(const Self&) = default;

private:
    friend class posix::linux::Process;
    Self();
    ~Self();

    struct Private;
    std::shared_ptr<Private> d;
};
}
}
#endif // POSIX_LINUX_PROCESS_H_
