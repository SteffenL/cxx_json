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

#include <memory>

namespace langnes {
namespace json {
namespace detail {

#ifdef __cpp_lib_make_unique
using std::make_unique;
#else
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>{new T(std::forward<Args>(args)...)};
}
#endif

} // namespace detail
} // namespace json
} // namespace langnes
