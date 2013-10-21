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

#ifndef POSIX_CHILD_PROCESS_H_
#define POSIX_CHILD_PROCESS_H_

#include <posix/process.h>
#include <posix/standard_stream.h>

#include <iosfwd>
#include <functional>

namespace posix
{
class ChildProcess : public Process
{
public:
    ~ChildProcess();

    Wait::Result wait_for(const Wait::Flags& flags);

    std::istream& cerr();
    std::ostream& cin();
    std::istream& cout();

private:
    friend ChildProcess fork(const std::function<int()>&, const StandardStreamFlags&);
    friend ChildProcess vfork(const std::function<int()>&, const StandardStreamFlags&);

    class Pipe
    {
    public:
        static Pipe invalid();

        Pipe();
        Pipe(const Pipe& rhs);
        ~Pipe();

        Pipe& operator=(const Pipe& rhs);

        int read_fd() const;
        void close_read_fd();

        int write_fd() const;
        void close_write_fd();

    private:
        Pipe(int fds[2]);
        int fds[2];
    };

    ChildProcess(pid_t pid,
                 const Pipe& stdin,
                 const Pipe& stdout,
                 const Pipe& stderr);

    struct Private;
    std::shared_ptr<Private> d;
};
}

#endif // POSIX_CHILD_PROCESS_H_
