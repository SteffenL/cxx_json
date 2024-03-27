/*
 * Copyright 2024 Steffen André Langnes
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <string>
#include <unordered_map>

namespace langnes {
namespace json {
namespace detail {

template<typename Key, typename Value>
class dict {
    using container = std::unordered_map<Key, Value>;
    using const_iterator = typename container::const_iterator;
    using iterator = typename container::iterator;
    using key_type = Key;
    using mapped_type = Value;
    using value_type = std::pair<const Key, Value>;

public:
    const Value& operator[](const std::string& key) const {
        return m_data.at(key);
    }

    Value& operator[](const std::string& key) { return m_data[key]; }

    size_t size() const noexcept { return m_data.size(); }
    iterator begin() noexcept { return m_data.begin(); }
    const_iterator begin() const noexcept { return m_data.begin(); }
    const_iterator cbegin() const noexcept { return m_data.cbegin(); }
    iterator end() noexcept { return m_data.end(); }
    const_iterator end() const noexcept { return m_data.end(); }
    const_iterator cend() const noexcept { return m_data.cend(); }
    bool empty() const noexcept { return m_data.empty(); }
    size_t count(const Key& key) const { return m_data.count(key); }
    void clear() noexcept { m_data.clear(); }

    template<typename T>
    void erase(T&& what) noexcept {
        m_data.erase(std::forward<T>(what));
    }

    template<typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        return m_data.emplace(std::forward<Args>(args)...);
    }

    iterator insert(const value_type& kv) { return m_data.insert(kv); }
    iterator insert(value_type&& kv) { return m_data.insert(std::move(kv)); }

private:
    container m_data;
};

} // namespace detail
} // namespace json
} // namespace langnes
