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

#include "errors.h"
#include "types.h"

#include <stdbool.h>
#include <stddef.h>

#ifndef LANGNES_JSON_API
#if defined(LANGNES_JSON_SHARED) || defined(LANGNES_JSON_BUILD_SHARED)
#if defined(_WIN32) || defined(__CYGWIN__)
#if defined(LANGNES_JSON_BUILD_SHARED)
#define LANGNES_JSON_API __declspec(dllexport)
#else
#define LANGNES_JSON_API __declspec(dllimport)
#endif
#else
#define LANGNES_JSON_API __attribute__((visibility("default")))
#endif
#elif !defined(LANGNES_JSON_STATIC) && defined(__cplusplus)
#define LANGNES_JSON_API inline
#else
#define LANGNES_JSON_API extern
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

LANGNES_JSON_API void langnes_json_check_error(langnes_json_error_code_t ec);
LANGNES_JSON_API bool langnes_json_succeeded(langnes_json_error_code_t ec);
LANGNES_JSON_API bool langnes_json_failed(langnes_json_error_code_t ec);

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_load_from_cstring(const char* data, langnes_json_value_t** result);
LANGNES_JSON_API langnes_json_error_code_t langnes_json_save_to_string(
    langnes_json_value_t* json_value, langnes_json_string_t** result);

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_free(langnes_json_value_t* json_value);
LANGNES_JSON_API langnes_json_error_code_t langnes_json_value_replace(
    langnes_json_value_t* target, langnes_json_value_t* replacement);
LANGNES_JSON_API langnes_json_error_code_t langnes_json_value_get_type(
    langnes_json_value_t* json_value, langnes_json_value_type_t* result);
LANGNES_JSON_API langnes_json_value_type_t
langnes_json_value_get_type_s(langnes_json_value_t* json_value);
LANGNES_JSON_API langnes_json_error_code_t langnes_json_value_clone(
    langnes_json_value_t* json_value, langnes_json_value_t** result);
LANGNES_JSON_API langnes_json_value_t*
langnes_json_value_clone_s(langnes_json_value_t* json_value);

//
// String
//

LANGNES_JSON_API langnes_json_error_code_t langnes_json_string_get_cstring(
    langnes_json_string_t* str, const char** result);
LANGNES_JSON_API const char*
langnes_json_string_get_cstring_s(langnes_json_string_t* str);
LANGNES_JSON_API langnes_json_error_code_t
langnes_json_string_get_length(langnes_json_string_t* str, size_t* result);
LANGNES_JSON_API size_t
langnes_json_string_get_length_s(langnes_json_string_t* str);
LANGNES_JSON_API langnes_json_error_code_t
langnes_json_string_free(langnes_json_string_t* str);

//
// JSON string
//

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_string_new_with_cstring(const char* data,
                                           langnes_json_value_t** result);
LANGNES_JSON_API langnes_json_value_t*
langnes_json_value_string_new_with_cstring_s(const char* data);
LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_is_string(langnes_json_value_t* json_value, bool* result);
LANGNES_JSON_API bool
langnes_json_value_is_string_s(langnes_json_value_t* json_value);
LANGNES_JSON_API langnes_json_error_code_t langnes_json_value_get_string(
    langnes_json_value_t* json_value, langnes_json_string_t** result);
LANGNES_JSON_API langnes_json_string_t*
langnes_json_value_get_string_s(langnes_json_value_t* json_value);
LANGNES_JSON_API langnes_json_error_code_t langnes_json_value_get_cstring(
    langnes_json_value_t* json_value, const char** result);
LANGNES_JSON_API const char*
langnes_json_value_get_cstring_s(langnes_json_value_t* json_value);
LANGNES_JSON_API langnes_json_error_code_t langnes_json_value_set_string(
    langnes_json_value_t* json_value, const char* data);

//
// JSON number
//

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_number_new(double value, langnes_json_value_t** result);
LANGNES_JSON_API langnes_json_value_t*
langnes_json_value_number_new_s(double value);
LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_is_number(langnes_json_value_t* json_value, bool* result);
LANGNES_JSON_API bool
langnes_json_value_is_number_s(langnes_json_value_t* json_value);
LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_get_number(langnes_json_value_t* json_value, double* result);
LANGNES_JSON_API double
langnes_json_value_get_number_s(langnes_json_value_t* json_value);
LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_set_number(langnes_json_value_t* json_value, double value);

//
// JSON boolean
//

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_boolean_new(bool value, langnes_json_value_t** result);
LANGNES_JSON_API langnes_json_value_t*
langnes_json_value_boolean_new_s(bool value);
LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_is_boolean(langnes_json_value_t* json_value, bool* result);
LANGNES_JSON_API bool
langnes_json_value_is_boolean_s(langnes_json_value_t* json_value);
LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_get_boolean(langnes_json_value_t* json_value, bool* result);
LANGNES_JSON_API bool
langnes_json_value_get_boolean_s(langnes_json_value_t* json_value);
LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_set_boolean(langnes_json_value_t* json_value, bool value);
//
// JSON null
//

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_null_new(langnes_json_value_t** result);
LANGNES_JSON_API langnes_json_value_t* langnes_json_value_null_new_s(void);
LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_is_null(langnes_json_value_t* json_value, bool* result);
LANGNES_JSON_API bool
langnes_json_value_is_null_s(langnes_json_value_t* json_value);
LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_set_null(langnes_json_value_t* json_value);

//
// JSON object
//

struct langnes_json_object_member_t {
    const char* name;
    langnes_json_value_t* value;
};

// NOLINTNEXTLINE(modernize-use-using)
typedef struct langnes_json_object_member_t langnes_json_object_member_t;

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_object_new(langnes_json_value_t** result);
LANGNES_JSON_API langnes_json_value_t* langnes_json_value_object_new_s(void);
LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_object_new_with_members(
    langnes_json_object_member_t* members, size_t length,
    langnes_json_value_t** result);
LANGNES_JSON_API langnes_json_value_t*
langnes_json_value_object_new_with_members_s(
    langnes_json_object_member_t* members, size_t length);
LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_is_object(langnes_json_value_t* json_value, bool* result);
LANGNES_JSON_API bool
langnes_json_value_is_object_s(langnes_json_value_t* json_value);
LANGNES_JSON_API langnes_json_error_code_t langnes_json_value_object_get_value(
    langnes_json_value_t* json_object, const char* member_name,
    langnes_json_value_t** result);
LANGNES_JSON_API langnes_json_value_t*
langnes_json_value_object_get_value_s(langnes_json_value_t* json_object,
                                      const char* member_name);
LANGNES_JSON_API langnes_json_error_code_t langnes_json_value_object_set_value(
    langnes_json_value_t* json_object, const char* member_name,
    langnes_json_value_t* member_value);
LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_set_object(langnes_json_value_t* json_object);
LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_set_object_with_members(
    langnes_json_value_t* json_object, langnes_json_object_member_t* members,
    size_t length);
LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_object_get_members_length(langnes_json_value_t* json_object,
                                             size_t* result);
LANGNES_JSON_API size_t langnes_json_value_object_get_members_length_s(
    langnes_json_value_t* json_object);
LANGNES_JSON_API langnes_json_error_code_t langnes_json_value_object_get_member(
    langnes_json_value_t* json_object, size_t index,
    langnes_json_object_member_t* result);
LANGNES_JSON_API langnes_json_object_member_t
langnes_json_value_object_get_member_s(langnes_json_value_t* json_object,
                                       size_t index);
LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_object_clear(langnes_json_value_t* json_object);

//
// JSON array
//

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_array_new(langnes_json_value_t** result);
LANGNES_JSON_API langnes_json_value_t* langnes_json_value_array_new_s(void);
LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_array_new_with_elements(langnes_json_value_t** values,
                                           size_t length,
                                           langnes_json_value_t** result);
LANGNES_JSON_API langnes_json_value_t*
langnes_json_value_array_new_with_elements_s(langnes_json_value_t** values,
                                             size_t length);
LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_is_array(langnes_json_value_t* json_value, bool* result);
LANGNES_JSON_API bool
langnes_json_value_is_array_s(langnes_json_value_t* json_value);
LANGNES_JSON_API langnes_json_error_code_t langnes_json_value_array_get_length(
    langnes_json_value_t* json_array, size_t* result);
LANGNES_JSON_API size_t
langnes_json_value_array_get_length_s(langnes_json_value_t* json_array);
LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_array_clear(langnes_json_value_t* json_array);
LANGNES_JSON_API langnes_json_error_code_t langnes_json_value_array_push(
    langnes_json_value_t* json_array, langnes_json_value_t* json_array_element);
LANGNES_JSON_API langnes_json_error_code_t langnes_json_value_array_get_item(
    langnes_json_value_t* value, size_t index, langnes_json_value_t** result);
LANGNES_JSON_API langnes_json_value_t*
langnes_json_value_array_get_item_s(langnes_json_value_t* value, size_t index);
LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_set_array(langnes_json_value_t* json_array);
LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_set_array_with_elements(langnes_json_value_t* json_array,
                                           langnes_json_value_t** values,
                                           size_t length);

#ifdef __cplusplus
} // extern "C"
#endif
