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
#ifndef POSIX_LINUX_PROC_PROCESS_STAT_H_
#define POSIX_LINUX_PROC_PROCESS_STAT_H_

#include <posix/linux/proc/process/state.h>

#include <string>

namespace posix
{
class Process;
namespace linux
{
namespace proc
{
namespace process
{
struct Stat
{
    pid_t pid = 1;
    std::string executable;
    State state = State::undefined;
    pid_t parent = -1;
    pid_t process_group = -1;
    int session_id = -1;
    int tty_nr = -1;
    int controlling_process_group = -1;
    unsigned int kernel_flags = 0;
    long unsigned int minor_faults_count = 0;
    long unsigned int minor_faults_count_by_children = 0;
    long unsigned int major_faults_count = 0;
    long unsigned int major_faults_count_by_children = 0;
    struct
    {
        long unsigned int user = 0;
        long unsigned int system = 0;
        long unsigned int user_for_children = 0;
        long unsigned int system_for_children = 0;
    } time;
    long int priority = 0;
    long int nice = 0;
    long int thread_count = 0;
    long int time_before_next_sig_alarm = 0;
    long int start_time = 0;
    struct
    {
        long unsigned int virt = 0;
        long unsigned int resident_set = 0;
        long unsigned int resident_set_limit = 0;
    } size;
    struct
    {
        long unsigned int start_code = 0;
        long unsigned int end_code = 0;
        long unsigned int start_stack = 0;
        long unsigned int stack_pointer = 0;
        long unsigned int instruction_pointer = 0;
    } addresses;
    struct
    {
        long unsigned int pending = 0;
        long unsigned int blocked = 0;
        long unsigned int ignored = 0;
        long unsigned int caught = 0;
    } signals;
    long unsigned int channel = 0;
    long unsigned int swap_count = 0;
    long unsigned int swap_count_children = 0;
    int exit_signal = -1;
    int cpu_count = -1;
    unsigned int realtime_priority = 0;
    unsigned int scheduling_policy = 0;
    long long unsigned int aggregated_block_io_delays = 0;
    long unsigned int guest_time = 0;
    long unsigned int guest_time_children = 0;
};

const posix::Process& operator>>(const posix::Process& process, Stat& stat);
}
}
}
}
#endif // POSIX_LINUX_PROC_PROCESS_STAT_H_
