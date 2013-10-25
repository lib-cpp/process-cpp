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

#include <posix/exec.h>
#include <posix/fork.h>
#include <posix/process.h>
#include <posix/signal.h>

#include <gtest/gtest.h>

#include <map>

TEST(PosixProcess, this_process_instance_reports_correct_pid)
{
    EXPECT_EQ(getpid(), posix::this_process::instance().pid());
}

TEST(PosixProcess, this_process_instance_reports_correct_parent)
{
    EXPECT_EQ(getppid(), posix::this_process::parent().pid());
}

TEST(PosixProcess, throwing_access_to_process_group_id_of_this_process_works)
{
    EXPECT_EQ(getpgrp(), posix::this_process::instance().process_group_id_or_throw());
}

TEST(PosixProcess, throwing_access_to_process_group_id_of_a_forked_process_works)
{
    posix::ChildProcess child = posix::fork(
                []() { std::cout << "Child" << std::endl; while(true) {} return EXIT_FAILURE;},
                posix::StandardStreamFlags()
                    .set(posix::StandardStream::stdin)
                    .set(posix::StandardStream::stdout));
    EXPECT_TRUE(child.pid() > 0);

    EXPECT_EQ(getpgrp(), child.process_group_id_or_throw());

    EXPECT_NO_THROW(child.send_signal_or_throw(posix::Signal::sig_kill));
    child.wait_for(posix::wait::Flag::untraced);
}

TEST(PosixProcess, non_throwing_access_to_process_group_id_of_this_process_works)
{
    pid_t pid; bool success; std::system_error se;
    std::tie(pid, success) = posix::this_process::instance().process_group_id(se);
    EXPECT_TRUE(success);
    EXPECT_EQ(getpgrp(), pid);
}

TEST(PosixProcess, trying_to_access_process_group_of_invalid_process_throws)
{
    EXPECT_ANY_THROW(posix::Process::invalid().process_group_id_or_throw());
}

TEST(PosixProcess, trying_to_access_process_group_of_invalid_process_reports_error)
{
    pid_t pid; bool success; std::system_error se;
    std::tie(pid, success) = posix::Process::invalid().process_group_id(se);
    EXPECT_FALSE(success);
    EXPECT_TRUE(static_cast<bool>(se.code()));
}

TEST(PosixProcess, non_throwing_access_to_process_group_id_of_a_forked_process_works)
{
    posix::ChildProcess child = posix::fork(
                []() { std::cout << "Child" << std::endl; while(true) {} return EXIT_FAILURE;},
                posix::StandardStreamFlags()
                    .set(posix::StandardStream::stdin)
                    .set(posix::StandardStream::stdout));
    EXPECT_TRUE(child.pid() > 0);

    pid_t pid; bool success; std::system_error se;
    std::tie(pid, success) = child.process_group_id(se);
    EXPECT_TRUE(success);
    EXPECT_EQ(getpgrp(), pid);

    EXPECT_NO_THROW(child.send_signal_or_throw(posix::Signal::sig_kill));
    child.wait_for(posix::wait::Flag::untraced);
}

TEST(PosixProcess, accessing_streams_of_this_process_works)
{
    {
        std::stringstream ss;
        auto old = posix::this_process::cout().rdbuf(ss.rdbuf());
        posix::this_process::cout() << "posix::this_process::instance().cout()\n";
        EXPECT_EQ(ss.str(), "posix::this_process::instance().cout()\n");
        posix::this_process::cout().rdbuf(old);
    }

    {
        std::stringstream ss;
        auto old = posix::this_process::cerr().rdbuf(ss.rdbuf());
        posix::this_process::cerr() << "posix::this_process::instance().cerr()" << std::endl;
        EXPECT_EQ(ss.str(), "posix::this_process::instance().cerr()\n");
        posix::this_process::cerr().rdbuf(old);
    }
}

TEST(Self, non_mutable_access_to_the_environment_returns_correct_results)
{
    static const char* home = "HOME";
    static const char* totally_not_existent = "totally_not_existent_42";
    EXPECT_EQ(getenv("HOME"), posix::this_process::env::get(home));
    EXPECT_EQ("", posix::this_process::env::get(totally_not_existent));
}

TEST(Self, mutable_access_to_the_environment_alters_the_environment)
{
    static const char* totally_not_existent = "totally_not_existent_42";
    static const char* totally_not_existent_value = "42";

    EXPECT_EQ("", posix::this_process::env::get(totally_not_existent));
    EXPECT_NO_THROW(posix::this_process::env::set_or_throw(
                        totally_not_existent,
                        totally_not_existent_value));
    EXPECT_EQ(totally_not_existent_value,
              posix::this_process::env::get(totally_not_existent));

    EXPECT_NO_THROW(
                posix::this_process::env::unset_or_throw(
                    totally_not_existent));
    EXPECT_EQ("",
              posix::this_process::env::get(totally_not_existent));
}

TEST(Self, getting_env_var_for_empty_key_does_not_throw)
{
    EXPECT_NO_THROW(posix::this_process::env::get(""));
}

TEST(Self, setting_env_var_for_empty_key_throws)
{
    EXPECT_ANY_THROW(posix::this_process::env::set_or_throw(
                        "",
                        "uninteresting"));
}

TEST(ChildProcess, fork_returns_process_object_with_valid_pid_and_wait_for_returns_correct_result)
{
    posix::ChildProcess child = posix::fork(
                []() { std::cout << "Child" << std::endl; return EXIT_SUCCESS; },
                posix::StandardStreamFlags()
                    .set(posix::StandardStream::stdin)
                    .set(posix::StandardStream::stdout));
    EXPECT_TRUE(child.pid() > 0);

    auto result = child.wait_for(posix::wait::Flag::untraced);
    EXPECT_EQ(posix::wait::Result::Status::exited,
              result.status);
    EXPECT_EQ(posix::exit::Status::success,
              result.detail.if_exited.status);

    child = posix::fork(
                []() { std::cout << "Child" << std::endl; return EXIT_FAILURE; },
                posix::StandardStreamFlags()
                    .set(posix::StandardStream::stdin)
                    .set(posix::StandardStream::stdout));
    EXPECT_TRUE(child.pid() > 0);

    result = child.wait_for(posix::wait::Flag::untraced);
    EXPECT_EQ(posix::wait::Result::Status::exited,
              result.status);
    EXPECT_EQ(posix::exit::Status::failure,
              result.detail.if_exited.status);
}

TEST(ChildProcess, signalling_a_forked_child_makes_wait_for_return_correct_result)
{
    posix::ChildProcess child = posix::fork(
                []() { std::cout << "Child" << std::endl; while(true) {} return EXIT_FAILURE;},
                posix::StandardStreamFlags()
                    .set(posix::StandardStream::stdin)
                    .set(posix::StandardStream::stdout));
    EXPECT_TRUE(child.pid() > 0);

    EXPECT_NO_THROW(child.send_signal_or_throw(posix::Signal::sig_kill));
    auto result = child.wait_for(posix::wait::Flag::untraced);
    EXPECT_EQ(posix::wait::Result::Status::signaled,
              result.status);
    EXPECT_EQ(posix::Signal::sig_kill,
              result.detail.if_signaled.signal);

    child = posix::fork(
                []() { std::cout << "Child" << std::endl; while(true) {} return EXIT_FAILURE;},
                posix::StandardStreamFlags()
                    .set(posix::StandardStream::stdin)
                    .set(posix::StandardStream::stdout));
    EXPECT_TRUE(child.pid() > 0);

    EXPECT_NO_THROW(child.send_signal_or_throw(posix::Signal::sig_term));
    result = child.wait_for(posix::wait::Flag::untraced);
    EXPECT_EQ(posix::wait::Result::Status::signaled,
              result.status);
    EXPECT_EQ(posix::Signal::sig_term,
              result.detail.if_signaled.signal);
}

TEST(ChildProcess, stopping_a_forked_child_makes_wait_for_return_correct_result)
{
    posix::ChildProcess child = posix::fork(
                []()
                {
                    std::string line;
                    while(true)
                    {
                        std::cin >> line;
                        std::cout << line << std::endl;
                    }
                    return EXIT_FAILURE;
                },
                posix::StandardStreamFlags()
                    .set(posix::StandardStream::stdin)
                    .set(posix::StandardStream::stdout));
    EXPECT_TRUE(child.pid() > 0);

    const std::string echo_value{"42"};
    child.cin() << echo_value << std::endl;
    std::string line; child.cout() >> line;

    EXPECT_EQ(echo_value, line);

    EXPECT_NO_THROW(child.send_signal_or_throw(posix::Signal::sig_stop));
    auto result = child.wait_for(posix::wait::Flag::untraced);
    EXPECT_EQ(posix::wait::Result::Status::stopped,
              result.status);
    EXPECT_EQ(posix::Signal::sig_stop,
              result.detail.if_stopped.signal);

    EXPECT_NO_THROW(child.send_signal_or_throw(posix::Signal::sig_kill));
    result = child.wait_for(posix::wait::Flag::untraced);
    EXPECT_EQ(posix::wait::Result::Status::signaled,
              result.status);
    EXPECT_EQ(posix::Signal::sig_kill,
              result.detail.if_signaled.signal);
}

TEST(ChildProcess, exec_returns_process_object_with_valid_pid_and_wait_for_returns_correct_result)
{
    const std::string program{"/usr/bin/sleep"};
    const std::vector<std::string> argv = {"10"};
    std::map<std::string, std::string> env;
    posix::this_process::env::for_each([&env](const std::string& key, const std::string& value)
    {
        env.insert(std::make_pair(key, value));
    });

    posix::ChildProcess child = posix::exec(program,
                                            argv,
                                            env,
                                            posix::StandardStreamFlags()
                                                .set(posix::StandardStream::stdin)
                                                .set(posix::StandardStream::stdout));
    EXPECT_TRUE(child.pid() > 0);
    EXPECT_NO_THROW(child.send_signal_or_throw(posix::Signal::sig_kill));
    auto result = child.wait_for(posix::wait::Flag::untraced);
    EXPECT_EQ(posix::wait::Result::Status::signaled,
              result.status);
    EXPECT_EQ(posix::Signal::sig_kill,
              result.detail.if_signaled.signal);
}

TEST(ChildProcess, signalling_an_execd_child_makes_wait_for_return_correct_result)
{
    const std::string program{"/usr/bin/env"};
    const std::vector<std::string> argv = {};
    std::map<std::string, std::string> env;
    posix::this_process::env::for_each([&env](const std::string& key, const std::string& value)
    {
        env.insert(std::make_pair(key, value));
    });

    posix::ChildProcess child = posix::exec(
                program,
                argv,
                env,
                posix::StandardStreamFlags()
                    .set(posix::StandardStream::stdin)
                    .set(posix::StandardStream::stdout));

    EXPECT_TRUE(child.pid() > 0);

    EXPECT_NO_THROW(child.send_signal_or_throw(posix::Signal::sig_kill));
    auto result = child.wait_for(posix::wait::Flag::untraced);
    EXPECT_EQ(posix::wait::Result::Status::signaled,
              result.status);
    EXPECT_EQ(posix::Signal::sig_kill,
              result.detail.if_signaled.signal);

    child = posix::exec(program,
                        argv,
                        env,
                        posix::StandardStreamFlags()
                            .set(posix::StandardStream::stdin)
                            .set(posix::StandardStream::stdout));
    EXPECT_TRUE(child.pid() > 0);

    EXPECT_NO_THROW(child.send_signal_or_throw(posix::Signal::sig_term));
    result = child.wait_for(posix::wait::Flag::untraced);
    EXPECT_EQ(posix::wait::Result::Status::signaled,
              result.status);
    EXPECT_EQ(posix::Signal::sig_term,
              result.detail.if_signaled.signal);
}

TEST(ChildProcess, stopping_an_execd_child_makes_wait_for_return_correct_result)
{
    const std::string program{"/usr/bin/sleep"};
    const std::vector<std::string> argv = {"10"};
    std::map<std::string, std::string> env;
    posix::this_process::env::for_each([&env](const std::string& key, const std::string& value)
    {
        env.insert(std::make_pair(key, value));
    });

    posix::ChildProcess child = posix::exec(program,
                                            argv,
                                            env,
                                            posix::StandardStreamFlags()
                                                .set(posix::StandardStream::stdin)
                                                .set(posix::StandardStream::stdout));
    EXPECT_TRUE(child.pid() > 0);

    EXPECT_NO_THROW(child.send_signal_or_throw(posix::Signal::sig_stop));
    auto result = child.wait_for(posix::wait::Flag::untraced);
    EXPECT_EQ(posix::wait::Result::Status::stopped,
              result.status);
    EXPECT_EQ(posix::Signal::sig_stop,
              result.detail.if_signaled.signal);
    EXPECT_NO_THROW(child.send_signal_or_throw(posix::Signal::sig_kill));
    result = child.wait_for(posix::wait::Flag::untraced);
    EXPECT_EQ(posix::wait::Result::Status::signaled,
              result.status);
    EXPECT_EQ(posix::Signal::sig_kill,
              result.detail.if_signaled.signal);
}

TEST(StreamRedirect, redirecting_stdin_stdout_stderr_works)
{
    posix::ChildProcess child = posix::fork(
                []()
                {
                    std::string line;
                    while(true)
                    {
                        std::cin >> line;
                        std::cout << line << std::endl;
                        std::cerr << line << std::endl;
                    }
                    return EXIT_FAILURE;
                },
                posix::StandardStreamFlags()
                    .set(posix::StandardStream::stdin)
                    .set(posix::StandardStream::stdout)
                    .set(posix::StandardStream::stderr));

    ASSERT_TRUE(child.pid() > 0);

    const std::string echo_value{"42"};
    child.cin() << echo_value << std::endl;
    std::string line;
    EXPECT_NO_THROW(child.cout() >> line);
    EXPECT_EQ(echo_value, line);
    EXPECT_NO_THROW(child.cerr() >> line);
    EXPECT_EQ(echo_value, line);
    EXPECT_NO_THROW(child.send_signal_or_throw(posix::Signal::sig_kill));
    child.wait_for(posix::wait::Flag::untraced);
}

TEST(Environment, iterating_the_environment_does_not_throw)
{
    EXPECT_NO_THROW(posix::this_process::env::for_each(
                        [](const std::string& key, const std::string& value)
                        {
                            std::cout << key << " -> " << value << std::endl;
                        }););
}
