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

#include "macros.hpp"

#include <type_traits>

LANGNES_JSON_CXX_NS_BEGIN
namespace detail {

template<typename T>
struct remove_cvref {
    using type =
        typename std::remove_cv<typename std::remove_reference<T>::type>::type;
};

template<typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

template<bool Condition, typename T = void>
using enable_if_t = typename std::enable_if<Condition, T>::type;

} // namespace detail
LANGNES_JSON_CXX_NS_END
