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

#include <testing/fork_and_run.h>

#include <gtest/gtest.h>

TEST(ForkAndRun, succeeding_client_and_service_result_in_correct_return_value)
{
    auto service = [](){ return posix::exit::Status::success; };
    auto client = [](){ return posix::exit::Status::success; };

    auto result = testing::fork_and_run(service, client);

    EXPECT_EQ(testing::ForkAndRunResult::empty, result & testing::ForkAndRunResult::client_failed);
    EXPECT_EQ(testing::ForkAndRunResult::empty, result & testing::ForkAndRunResult::service_failed);
}

TEST(ForkAndRun, succeeding_client_and_failing_service_result_in_correct_return_value)
{
    auto service = [](){ return posix::exit::Status::failure; };
    auto client = [](){ return posix::exit::Status::success; };

    auto result = testing::fork_and_run(service, client);

    EXPECT_EQ(testing::ForkAndRunResult::empty, result & testing::ForkAndRunResult::client_failed);
    EXPECT_NE(testing::ForkAndRunResult::empty, result & testing::ForkAndRunResult::service_failed);
}

TEST(ForkAndRun, failing_client_and_failing_service_result_in_correct_return_value)
{
    auto service = [](){ return posix::exit::Status::failure; };
    auto client = [](){ return posix::exit::Status::failure; };

    auto result = testing::fork_and_run(service, client);

    EXPECT_NE(testing::ForkAndRunResult::empty, result & testing::ForkAndRunResult::client_failed);
    EXPECT_NE(testing::ForkAndRunResult::empty, result & testing::ForkAndRunResult::service_failed);
}

TEST(ForkAndRun, throwing_client_is_reported_as_failing)
{
    auto service = [](){ return posix::exit::Status::success; };
    auto client = [](){ throw std::runtime_error("failing client"); return posix::exit::Status::success; };

    auto result = testing::fork_and_run(service, client);

    EXPECT_NE(testing::ForkAndRunResult::empty, result & testing::ForkAndRunResult::client_failed);
    EXPECT_EQ(testing::ForkAndRunResult::empty, result & testing::ForkAndRunResult::service_failed);
}

TEST(ForkAndRun, exiting_with_failure_client_is_reported_as_failing)
{
    auto service = [](){ return posix::exit::Status::success; };
    auto client = [](){ exit(EXIT_FAILURE); return posix::exit::Status::success; };

    auto result = testing::fork_and_run(service, client);

    EXPECT_NE(testing::ForkAndRunResult::empty, result & testing::ForkAndRunResult::client_failed);
    EXPECT_EQ(testing::ForkAndRunResult::empty, result & testing::ForkAndRunResult::service_failed);
}

TEST(ForkAndRun, aborting_client_is_reported_as_failing)
{
    auto service = [](){ return posix::exit::Status::success; };
    auto client = [](){ abort(); return posix::exit::Status::success; };

    auto result = testing::fork_and_run(service, client);

    EXPECT_NE(testing::ForkAndRunResult::empty, result & testing::ForkAndRunResult::client_failed);
    EXPECT_EQ(testing::ForkAndRunResult::empty, result & testing::ForkAndRunResult::service_failed);
}

