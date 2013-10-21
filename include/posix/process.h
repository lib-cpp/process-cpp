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

#include <posix/wait.h>

#include <memory>
#include <system_error>

namespace posix
{
class Environment;
enum class Signal;
class Self;
class WaitFlags;

class Process
{
public:
    static const Self& self();
    static Self& mutable_self();

    virtual ~Process() noexcept;

    virtual pid_t pid() const;

    Process parent() const;

    virtual void send_signal(const Signal& signal);
    virtual void send_signal(const Signal& signal, std::system_error& e) noexcept;

protected:
    explicit Process(pid_t pid);

private:
    struct Private;
    std::shared_ptr<Private> d;
};

class Self : public Process
{
public:
    Self(const Self&) = default;
    Self& operator=(const Self&) = default;

    Environment& mutable_env();
    const Environment& env() const;

    std::istream& cin();
    std::ostream& cout();
    std::ostream& cerr();

protected:
    friend class Process;
    Self();
    ~Self();

private:
    struct Private;
    std::shared_ptr<Private> d;
};
}
#endif // POSIX_PROCESS_H_
