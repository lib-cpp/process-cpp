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

#include <posix/environment.h>
#include <posix/exec.h>
#include <posix/fork.h>
#include <posix/process.h>
#include <posix/signal.h>

#include <gtest/gtest.h>

#include <map>

TEST(PosixProcess, this_process_instance_reports_correct_pid)
{
    EXPECT_EQ(getpid(), posix::Process::self().pid());
}

TEST(PosixProcess, accessing_streams_of_this_process_works)
{
    posix::Process::mutable_self().cout() << "posix::Process::self().cout()" << std::endl;
    posix::Process::mutable_self().cerr() << "posix::Process::self().cerr()" << std::endl;
}

TEST(Self, non_mutable_access_to_the_environment_returns_correct_results)
{
    static const char* home = "HOME";
    static const char* totally_not_existent = "totally_not_existent_42";
    EXPECT_EQ(getenv("HOME"), posix::Process::self().env().value_for_key(home));
    EXPECT_EQ("", posix::Process::self().env().value_for_key(totally_not_existent));
}

TEST(Self, mutable_access_to_the_environment_alters_the_environment)
{
    static const char* totally_not_existent = "totally_not_existent_42";
    static const char* totally_not_existent_value = "42";

    EXPECT_EQ("", posix::Process::self().env().value_for_key(totally_not_existent));
    EXPECT_NO_THROW(posix::Process::mutable_self().mutable_env().set_value_for_key(
                        totally_not_existent,
                        totally_not_existent_value));
    EXPECT_EQ(totally_not_existent_value,
              posix::Process::self().env().value_for_key(totally_not_existent));

    EXPECT_NO_THROW(
                posix::Process::mutable_self().mutable_env().unset_value_for_key(
                    totally_not_existent));
    EXPECT_EQ("",
              posix::Process::self().env().value_for_key(totally_not_existent));
}

TEST(Self, getting_env_var_for_empty_key_does_not_throw)
{
    EXPECT_NO_THROW(posix::Process::self().env().value_for_key(""));
}

TEST(Self, setting_env_var_for_empty_key_throws)
{
    EXPECT_ANY_THROW(posix::Process::mutable_self().mutable_env().set_value_for_key(
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

    EXPECT_NO_THROW(child.send_signal(posix::Signal::sig_kill));
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

    EXPECT_NO_THROW(child.send_signal(posix::Signal::sig_term));
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

    EXPECT_NO_THROW(child.send_signal(posix::Signal::sig_stop));
    auto result = child.wait_for(posix::wait::Flag::untraced);
    EXPECT_EQ(posix::wait::Result::Status::stopped,
              result.status);
    EXPECT_EQ(posix::Signal::sig_stop,
              result.detail.if_stopped.signal);

    EXPECT_NO_THROW(child.send_signal(posix::Signal::sig_kill));
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
    posix::Process::self().env().for_each([&env](const std::string& key, const std::string& value)
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
    EXPECT_NO_THROW(child.send_signal(posix::Signal::sig_kill));
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
    posix::Process::self().env().for_each([&env](const std::string& key, const std::string& value)
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

    EXPECT_NO_THROW(child.send_signal(posix::Signal::sig_kill));
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

    EXPECT_NO_THROW(child.send_signal(posix::Signal::sig_term));
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
    posix::Process::self().env().for_each([&env](const std::string& key, const std::string& value)
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

    EXPECT_NO_THROW(child.send_signal(posix::Signal::sig_stop));
    auto result = child.wait_for(posix::wait::Flag::untraced);
    EXPECT_EQ(posix::wait::Result::Status::stopped,
              result.status);
    EXPECT_EQ(posix::Signal::sig_stop,
              result.detail.if_signaled.signal);
    EXPECT_NO_THROW(child.send_signal(posix::Signal::sig_kill));
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
    EXPECT_NO_THROW(child.send_signal(posix::Signal::sig_kill));
    child.wait_for(posix::wait::Flag::untraced);
}

TEST(Environment, iterating_the_environment_does_not_throw)
{
    EXPECT_NO_THROW(posix::Process::self().env().for_each(
                        [](const std::string& key, const std::string& value)
                        {
                            std::cout << key << " -> " << value << std::endl;
                        }););
}
