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

#ifndef POSIX_ENVIRONMENT_H_
#define POSIX_ENVIRONMENT_H_

#include <functional>
#include <string>
#include <system_error>

namespace posix
{
class Self;

class Environment
{
public:

    void for_each(const std::function<void(const std::string&, const std::string&)>& functor) const noexcept;

    std::string value_for_key(const std::string& key) const;
    std::string value_for_key(const std::string& key,
                              std::system_error& se) const noexcept;

    void unset_value_for_key(const std::string& key);
    void unset_value_for_key(const std::string& key,
                             std::system_error& se) noexcept;

    void set_value_for_key(const std::string& key,
                           const std::string& value);
    void set_value_for_key(const std::string &key,
                           const std::string &value,
                           std::system_error& se) noexcept;

private:
    friend class Self;
    Environment();
    ~Environment();
};
}

#endif // POSIX_ENVIRONMENT_H_
