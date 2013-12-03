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

#ifndef CORE_POSIX_SIGNAL_H_
#define CORE_POSIX_SIGNAL_H_

#include <signal.h>

namespace core
{
namespace posix
{
/**
 * @brief The Signal enum collects the most common POSIX signals.
 */
enum class Signal
{
    sig_hup = SIGHUP,
    sig_int = SIGINT,
    sig_quit = SIGQUIT,
    sig_ill = SIGILL,
    sig_abrt = SIGABRT,
    sig_fpe = SIGFPE,
    sig_kill = SIGKILL,
    sig_segv = SIGSEGV,
    sig_pipe = SIGPIPE,
    sig_alrm = SIGALRM,
    sig_term = SIGTERM,
    sig_usr1 = SIGUSR1,
    sig_usr2 = SIGUSR2,
    sig_chld = SIGCHLD,
    sig_cont = SIGCONT,
    sig_stop = SIGSTOP,
    sig_tstp = SIGTSTP,
    sig_ttin = SIGTTIN,
    sig_ttou = SIGTTOU
};
}
}

#endif
