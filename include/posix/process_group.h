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

#ifndef POSIX_PROCESS_GROUP_H_
#define POSIX_PROCESS_GROUP_H_

#include <posix/signalable.h>

#include <memory>

namespace posix
{
class Process;

/**
 * @brief The ProcessGroup class models a signalable group of process.
 *
 * Summary from http://en.wikipedia.org/wiki/Process_group:
 *
 * In POSIX-conformant operating systems, a process group denotes a collection
 * of one or more processes. Process groups are used to control the distribution
 * of signals. A signal directed to a process group is delivered individually to
 * all of the processes that are members of the group.
 */
class ProcessGroup : public Signalable
{
public:
    /**
     * @brief Accesses the id of this process group.
     * @return The id of this process group.
     */
    virtual pid_t id() const;

protected:
    friend class Process;
    ProcessGroup(pid_t id);

private:
    struct Private;
    std::shared_ptr<Private> d;
};
}

#endif // POSIX_PROCESS_GROUP_H_
