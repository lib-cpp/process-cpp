# Copyright © 2013 Canonical Ltd.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Authored by: Thomas Voss <thomas.voss@canonical.com>

add_library(
  process-cpp SHARED
  
  core/posix/backtrace.h
  core/posix/backtrace.cpp

  core/posix/child_process.cpp
  core/posix/exec.cpp
  core/posix/fork.cpp
  core/posix/process.cpp
  core/posix/process_group.cpp
  core/posix/signal.cpp
  core/posix/signalable.cpp
  core/posix/standard_stream.cpp
  core/posix/wait.cpp
  core/posix/this_process.cpp

  core/posix/linux/proc/process/oom_adj.cpp
  core/posix/linux/proc/process/oom_score.cpp
  core/posix/linux/proc/process/oom_score_adj.cpp
  core/posix/linux/proc/process/stat.cpp

  core/testing/cross_process_sync.cpp
  core/testing/fork_and_run.cpp
)

target_link_libraries(
  process-cpp

  ${Boost_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  execinfo
)

# We compile with all symbols visible by default. For the shipping library, we strip
# out all symbols that are not in core::*
set(symbol_map "${CMAKE_SOURCE_DIR}/symbols.map")
set_target_properties(
  process-cpp

  PROPERTIES
  LINK_FLAGS "${ldflags} -Wl,--version-script,${symbol_map}"
  LINK_DEPENDS ${symbol_map}
  VERSION ${PROCESS_CPP_VERSION_MAJOR}.${PROCESS_CPP_VERSION_MINOR}.${PROCESS_CPP_VERSION_PATCH}
  SOVERSION ${PROCESS_CPP_VERSION_MAJOR}
)

install(
  TARGETS process-cpp
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
