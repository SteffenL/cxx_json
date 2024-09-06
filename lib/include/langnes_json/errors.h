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

#include "detail/macros.h"

// NOLINTBEGIN(modernize-use-using)
/// @see error_code
typedef int langnes_json_error_code_t;
// NOLINTEND(modernize-use-using)

#ifdef __cplusplus
extern "C" {
#endif

/// @see error_code::parse_error
LANGNES_JSON_API const langnes_json_error_code_t langnes_json_error_parse_error;

/// @see error_code::out_of_range
LANGNES_JSON_API const langnes_json_error_code_t
    langnes_json_error_out_of_range;

/// @see error_code::bad_access
LANGNES_JSON_API const langnes_json_error_code_t langnes_json_error_bad_access;

/// @see error_code::invalid_state
LANGNES_JSON_API const langnes_json_error_code_t
    langnes_json_error_invalid_state;

/// @see error_code::invalid_argument
LANGNES_JSON_API const langnes_json_error_code_t
    langnes_json_error_invalid_argument;

/// @see error_code::unspecified
LANGNES_JSON_API const langnes_json_error_code_t langnes_json_error_unspecified;

/// @see error_code::ok
LANGNES_JSON_API const langnes_json_error_code_t langnes_json_error_ok;

#ifdef __cplusplus
} // extern "C"
#endif
