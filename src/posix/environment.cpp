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

#include <boost/algorithm/string.hpp>

#include <mutex>
#include <system_error>

#include <cerrno>
#include <cstdlib>

#if defined(_GNU_SOURCE)
#include <unistd.h>
#else
extern char** environ;
#endif

namespace
{
std::mutex& env_guard()
{
    static std::mutex m;
    return m;
}
}

namespace posix
{
Environment::Environment()
{
}

Environment::~Environment()
{
}

void Environment::for_each(const std::function<void(const std::string&, const std::string&)>& functor) const noexcept
{
    std::lock_guard<std::mutex> lg(env_guard());
    auto it = ::environ;
    while (it != nullptr && *it != nullptr)
    {
        std::string line(*it);
        std::vector<std::string> tokens;
        boost::algorithm::split(tokens, line, boost::is_any_of("="));

        functor(tokens.front(), tokens.back());

        ++it;
    }
}

std::string Environment::value_for_key(const std::string& key) const
{
    std::lock_guard<std::mutex> lg(env_guard());

    auto result = ::getenv(key.c_str());
    return std::string{result ? result : ""};
}

void Environment::unset_value_for_key(const std::string& key)
{
    std::lock_guard<std::mutex> lg(env_guard());

    auto rc = ::unsetenv(key.c_str());

    if (rc == -1)
        throw std::system_error(errno, std::system_category());
}

void Environment::unset_value_for_key(const std::string &key,
                                      std::system_error& se) noexcept
{
    try
    {
        unset_value_for_key(key);
    } catch(const std::system_error& e)
    {
        se = e;
    }
}

void Environment::set_value_for_key(const std::string& key,
                                    const std::string& value)
{
    std::lock_guard<std::mutex> lg(env_guard());

    static const int overwrite = 0;
    auto rc = ::setenv(key.c_str(), value.c_str(), overwrite);

    if (rc == -1)
        throw std::system_error(errno, std::system_category());
}

void Environment::set_value_for_key(const std::string &key,
                                    const std::string &value,
                                    std::system_error& se) noexcept
{
    try
    {
        set_value_for_key(key, value);
    } catch(const std::system_error& e)
    {
        se = e;
    }
}
}
