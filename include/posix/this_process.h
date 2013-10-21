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

#include <functional>
#include <iosfwd>
#include <string>
#include <system_error>

namespace posix
{
class Process;
namespace this_process
{
namespace env
{

/**
 * @brief for_each invokes a functor for every key-value pair in the environment.
 * @param [in] functor Invoked for every key-value pair.
 */
void for_each(const std::function<void(const std::string&, const std::string&)>& functor) noexcept;

/**
 * @brief get queries the value of an environment variable.
 * @param [in] key Name of the variable to query the value for.
 * @return Contents of the variable.
 */
std::string get(const std::string& key);

/**
 * @brief unset_or_throw removes the variable with name key from the environment.
 * @throw std::system_error in case of errors.
 * @param [in] key Name of the variable to unset.
 */
void unset_or_throw(const std::string& key);

/**
 * @brief unset removes the variable with name key from the environment.
 * @return false in case of errors, true otherwise.
 * @param [in] key Name of the variable to unset.
 * @param [out] se Receives error details if unset returns false.
 */
bool unset(const std::string& key,
           std::system_error& se) noexcept;

/**
 * @brief set_or_throw will adjust the contents of the variable identified by key to the provided value.
 * @throw std::system_error in case of errors.
 * @param [in] key Name of the variable to set the value for.
 * @param [in] value New contents of the variable.
 */
void set_or_throw(const std::string& key,
                  const std::string& value);
/**
 * @brief set will adjust the contents of the variable identified by key to the provided value.
 * @return false in case of errors, true otherwise.
 * @param [in] key Name of the variable to set the value for.
 * @param [in] value New contents of the variable.
 * @param [out] se Receives the details in case of errors.
 */
bool set(const std::string &key,
         const std::string &value,
         std::system_error& se) noexcept;
}

/**
  * @brief Returns a Process instance corresponding to this process.
  */
const Process& instance();

/**
 * @brief Access this process's stdin.
 */
std::istream& cin();

/**
 * @brief Access this process's stdout.
 */
std::ostream& cout();

/**
 * @brief Access this process's stderr.
 */
std::ostream& cerr();
}
}
