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
  
  posix/child_process.cpp
  posix/exec.cpp
  posix/fork.cpp
  posix/process.cpp
  posix/process_group.cpp
  posix/signalable.cpp
  posix/standard_stream.cpp
  posix/wait.cpp
  posix/this_process.cpp

  posix/linux/proc/process/oom_adj.cpp
  posix/linux/proc/process/oom_score.cpp
  posix/linux/proc/process/oom_score_adj.cpp
  posix/linux/proc/process/stat.cpp

  testing/fork_and_run.cpp
)

target_link_libraries(
  process-cpp

  ${Boost_LIBRARIES}
)

set_target_properties(
  process-cpp

  PROPERTIES
  VERSION ${PROCESS_CPP_VERSION_MAJOR}.${PROCESS_CPP_VERSION_MINOR}.${PROCESS_CPP_VERSION_PATCH}
  SOVERSION ${PROCESS_CPP_VERSION_MAJOR}
)

install(
  TARGETS process-cpp
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
