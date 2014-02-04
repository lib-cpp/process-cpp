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

# Build with system gmock and embedded gtest
set (GMOCK_INCLUDE_DIR "/usr/include/gmock/include" CACHE PATH "gmock source include directory")
set (GMOCK_SOURCE_DIR "/usr/src/gmock" CACHE PATH "gmock source directory")
set (GTEST_INCLUDE_DIR "${GMOCK_SOURCE_DIR}/gtest/include" CACHE PATH "gtest source include directory")
set (GMOCK_BOTH_LIBRARIES gmock gmock_main)
add_subdirectory(${GMOCK_SOURCE_DIR} "${CMAKE_CURRENT_BINARY_DIR}/gmock")

#set (OLD_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
# Don't treat warnings as errors in 3rd_party/{gmock,cucumber-cpp}
#string (REPLACE " -Werror " " " CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
#find_package(Gtest REQUIRED)
include_directories(${GMOCK_INCLUDE_DIR} ${GTEST_INCLUDE_DIR})
#set (CMAKE_CXX_FLAGS ${OLD_CMAKE_CXX_FLAGS})

include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${GTEST_INCLUDE_DIR}
)

add_executable(
  posix_process_test
  posix_process_test.cpp
)

add_executable(
  linux_process_test
  linux_process_test.cpp
)

add_executable(
  fork_and_run_test
  fork_and_run_test.cpp

  # We include an external source file to prevent from leaking
  # symbols to the outside world
  ${CMAKE_SOURCE_DIR}/src/core/posix/backtrace.cpp
)

add_executable(
  cross_process_sync_test
  cross_process_sync_test.cpp
)

target_link_libraries(
  posix_process_test

  process-cpp

  ${CMAKE_THREAD_LIBS_INIT}
  ${GMOCK_BOTH_LIBRARIES}
)

target_link_libraries(
  linux_process_test

  process-cpp

  ${CMAKE_THREAD_LIBS_INIT}
  ${GMOCK_BOTH_LIBRARIES}
)

target_link_libraries(
  fork_and_run_test

  process-cpp

  ${CMAKE_THREAD_LIBS_INIT}
  ${GMOCK_BOTH_LIBRARIES}
)

target_link_libraries(
  cross_process_sync_test

  process-cpp

  ${CMAKE_THREAD_LIBS_INIT}
  ${GMOCK_BOTH_LIBRARIES}
)

add_test(posix_process_test ${CMAKE_CURRENT_BINARY_DIR}/posix_process_test)
add_test(linux_process_test ${CMAKE_CURRENT_BINARY_DIR}/linux_process_test)
add_test(fork_and_run_test ${CMAKE_CURRENT_BINARY_DIR}/fork_and_run_test)
add_test(cross_process_sync_test ${CMAKE_CURRENT_BINARY_DIR}/cross_process_sync_test)
