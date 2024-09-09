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
#include "langnes_json/json.h"
#include "langnes_json/json.hpp"

#include <cstdlib>
#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

LANGNES_JSON_API const langnes_json_error_code_t
    langnes_json_error_parse_error = static_cast<langnes_json_error_code_t>(
        langnes::json::error_code::parse_error);

LANGNES_JSON_API const langnes_json_error_code_t
    langnes_json_error_out_of_range = static_cast<langnes_json_error_code_t>(
        langnes::json::error_code::out_of_range);

LANGNES_JSON_API const langnes_json_error_code_t langnes_json_error_bad_access =
    static_cast<langnes_json_error_code_t>(
        langnes::json::error_code::bad_access);

LANGNES_JSON_API const langnes_json_error_code_t
    langnes_json_error_invalid_state = static_cast<langnes_json_error_code_t>(
        langnes::json::error_code::invalid_state);

LANGNES_JSON_API const langnes_json_error_code_t
    langnes_json_error_invalid_argument =
        static_cast<langnes_json_error_code_t>(
            langnes::json::error_code::invalid_argument);

LANGNES_JSON_API const langnes_json_error_code_t
    langnes_json_error_unspecified = static_cast<langnes_json_error_code_t>(
        langnes::json::error_code::unspecified);

LANGNES_JSON_API const langnes_json_error_code_t langnes_json_error_ok =
    static_cast<langnes_json_error_code_t>(langnes::json::error_code::ok);

namespace langnes {
namespace json {
namespace detail {

template<typename To, typename From>
To required_dynamic_cast(From p) noexcept {
    if (auto p2{dynamic_cast<To>(p)}) {
        return p2;
    }
    std::terminate();
}

template<typename WorkFn>
langnes_json_error_code_t filter_error(WorkFn do_work) noexcept {
    try {
        do_work();
        return langnes_json_error_ok;
    } catch (const error& e) {
        return static_cast<langnes_json_error_code_t>(e.code());
    } catch (const std::out_of_range&) {
        return langnes_json_error_out_of_range;
    } catch (...) {
        return langnes_json_error_unspecified;
    }
}

void free_object_members(langnes_json_object_member_t* members,
                         size_t length) noexcept {
    for (size_t i{}; i < length; ++i) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        langnes_json_value_free(members[i].value);
    }
}

void free_values(langnes_json_value_t** values, size_t length) noexcept {
    for (size_t i{}; i < length; ++i) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        langnes_json_value_free(values[i]);
    }
}

// NOLINTNEXTLINE(bugprone-exception-escape) - should never throw
void set_object_members(value& object, langnes_json_object_member_t* members,
                        size_t length) noexcept {
    if (!members) {
        std::terminate();
    }
    auto& entries{object.as_object()};
    for (size_t i{}; i < length; ++i) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        auto& member{members[i]};
        entries.emplace(member.name, std::move(*required_dynamic_cast<value*>(
                                         member.value)));
        langnes_json_value_free(member.value);
    }
}

// NOLINTNEXTLINE(bugprone-exception-escape) - should never throw
void set_array_elements(value& array, langnes_json_value_t** elements,
                        size_t length) noexcept {
    if (!elements) {
        std::terminate();
    }
    auto& elements_{array.as_array()};
    for (size_t i{}; i < length; ++i) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        auto* element{required_dynamic_cast<value*>(elements[i])};
        elements_.push_back(std::move(*element));
        langnes_json_value_free(element);
    }
}

} // namespace detail
} // namespace json
} // namespace langnes

extern "C" {

LANGNES_JSON_API void langnes_json_check_error(langnes_json_error_code_t ec) {
    if (langnes_json_failed(ec)) {
        std::terminate();
    }
}

LANGNES_JSON_API bool langnes_json_succeeded(langnes_json_error_code_t ec) {
    return ec >= langnes_json_error_ok;
}

LANGNES_JSON_API bool langnes_json_failed(langnes_json_error_code_t ec) {
    return !langnes_json_succeeded(ec);
}

LANGNES_JSON_API langnes_json_error_code_t langnes_json_load_from_cstring(
    const char* input, langnes_json_value_t** result) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!input || !result) {
            throw invalid_argument{};
        }
        *result = new value{load(input)};
    });
}

LANGNES_JSON_API langnes_json_error_code_t langnes_json_save_to_string(
    langnes_json_value_t* json_value, langnes_json_string_t** result) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!json_value || !result) {
            throw invalid_argument{};
        }
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        *result = reinterpret_cast<langnes_json_string_t*>(
            new std::string{save(*required_dynamic_cast<value*>(json_value))});
    });
}

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_free(langnes_json_value_t* json_value) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    if (!json_value) {
        return langnes_json_error_invalid_argument;
    }
    delete required_dynamic_cast<value*>(json_value);
    return langnes_json_error_ok;
}

LANGNES_JSON_API langnes_json_error_code_t langnes_json_value_replace(
    langnes_json_value_t* target, langnes_json_value_t* replacement) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!target || !replacement) {
            throw invalid_argument{};
        }
        *required_dynamic_cast<value*>(target) =
            std::move(*required_dynamic_cast<value*>(replacement));
        langnes_json_value_free(replacement);
    });
}

LANGNES_JSON_API langnes_json_error_code_t langnes_json_value_get_type(
    langnes_json_value_t* json_value, langnes_json_value_type_t* result) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!json_value || !result) {
            throw invalid_argument{};
        }
        *result = static_cast<langnes_json_value_type_t>(
            required_dynamic_cast<value*>(json_value)->get_type());
    });
}

LANGNES_JSON_API langnes_json_value_type_t
langnes_json_value_get_type_s(langnes_json_value_t* json_value) {
    langnes_json_value_type_t result{};
    langnes_json_check_error(langnes_json_value_get_type(json_value, &result));
    return result;
}

LANGNES_JSON_API langnes_json_error_code_t langnes_json_value_clone(
    langnes_json_value_t* json_value, langnes_json_value_t** result) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!json_value || !result) {
            throw invalid_argument{};
        }
        *result = static_cast<langnes_json_value_t*>(
            new value{required_dynamic_cast<value*>(json_value)->clone()});
    });
}

LANGNES_JSON_API langnes_json_value_t*
langnes_json_value_clone_s(langnes_json_value_t* json_value) {
    langnes_json_value_t* result{};
    langnes_json_check_error(langnes_json_value_clone(json_value, &result));
    return result;
}

//
// String
//

LANGNES_JSON_API langnes_json_error_code_t langnes_json_string_get_cstring(
    langnes_json_string_t* str, const char** result) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!str || !result) {
            throw invalid_argument{};
        }
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        *result = reinterpret_cast<const std::string*>(str)->c_str();
    });
}

LANGNES_JSON_API const char*
langnes_json_string_get_cstring_s(langnes_json_string_t* str) {
    const char* result{};
    langnes_json_check_error(langnes_json_string_get_cstring(str, &result));
    return result;
}

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_string_get_length(langnes_json_string_t* str, size_t* result) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!str || !result) {
            throw invalid_argument{};
        }
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        *result = reinterpret_cast<const std::string*>(str)->size();
    });
}

LANGNES_JSON_API size_t
langnes_json_string_get_length_s(langnes_json_string_t* str) {
    size_t result{};
    langnes_json_check_error(langnes_json_string_get_length(str, &result));
    return result;
}

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_string_free(langnes_json_string_t* str) {
    if (!str) {
        return langnes_json_error_invalid_argument;
    }
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    delete reinterpret_cast<std::string*>(str);
    return langnes_json_error_ok;
}

//
// JSON string
//

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_string_new_with_string(langnes_json_string_t* str,
                                          langnes_json_value_t** result) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!str || !result) {
            throw invalid_argument{};
        }
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        *result = new value{*reinterpret_cast<std::string*>(str)};
    });
}

LANGNES_JSON_API langnes_json_value_t*
langnes_json_value_string_new_with_string_s(langnes_json_string_t* str) {
    langnes_json_value_t* result{};
    langnes_json_check_error(
        langnes_json_value_string_new_with_string(str, &result));
    return result;
}

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_string_new_with_cstring(const char* data,
                                           langnes_json_value_t** result) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!data || !result) {
            throw invalid_argument{};
        }
        *result = new value{data};
    });
}

LANGNES_JSON_API langnes_json_value_t*
langnes_json_value_string_new_with_cstring_s(const char* data) {
    langnes_json_value_t* result{};
    langnes_json_check_error(
        langnes_json_value_string_new_with_cstring(data, &result));
    return result;
}

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_is_string(langnes_json_value_t* json_value, bool* result) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!json_value || !result) {
            throw invalid_argument{};
        }
        *result = required_dynamic_cast<value*>(json_value)->is_string();
    });
}

LANGNES_JSON_API bool
langnes_json_value_is_string_s(langnes_json_value_t* json_value) {
    bool result{};
    langnes_json_check_error(langnes_json_value_is_string(json_value, &result));
    return result;
}

LANGNES_JSON_API langnes_json_error_code_t langnes_json_value_get_string(
    langnes_json_value_t* json_value, langnes_json_string_t** result) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!json_value || !result) {
            throw invalid_argument{};
        }
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        *result = reinterpret_cast<langnes_json_string_t*>(std::addressof(
            required_dynamic_cast<value*>(json_value)->as_string()));
    });
}

LANGNES_JSON_API langnes_json_string_t*
langnes_json_value_get_string_s(langnes_json_value_t* json_value) {
    langnes_json_string_t* result{};
    langnes_json_check_error(
        langnes_json_value_get_string(json_value, &result));
    return result;
}

LANGNES_JSON_API langnes_json_error_code_t langnes_json_value_get_cstring(
    langnes_json_value_t* json_value, const char** result) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!json_value || !result) {
            throw invalid_argument{};
        }
        *result =
            required_dynamic_cast<value*>(json_value)->as_string().c_str();
    });
}

LANGNES_JSON_API const char*
langnes_json_value_get_cstring_s(langnes_json_value_t* json_value) {
    const char* result{};
    langnes_json_check_error(
        langnes_json_value_get_cstring(json_value, &result));
    return result;
}

LANGNES_JSON_API langnes_json_error_code_t langnes_json_value_set_cstring(
    langnes_json_value_t* json_value, const char* cstr) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!json_value || !cstr) {
            throw invalid_argument{};
        }
        *required_dynamic_cast<value*>(json_value) = cstr;
    });
}

//
// JSON number
//

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_number_new(double value, langnes_json_value_t** result) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!result) {
            throw invalid_argument{};
        }
        *result = new langnes::json::value{value};
    });
}

LANGNES_JSON_API langnes_json_value_t*
langnes_json_value_number_new_s(double value) {
    langnes_json_value_t* result{};
    langnes_json_check_error(langnes_json_value_number_new(value, &result));
    return result;
}

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_is_number(langnes_json_value_t* json_value, bool* result) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!json_value || !result) {
            throw invalid_argument{};
        }
        *result = required_dynamic_cast<value*>(json_value)->is_number();
    });
}

LANGNES_JSON_API bool
langnes_json_value_is_number_s(langnes_json_value_t* json_value) {
    bool result{};
    langnes_json_check_error(langnes_json_value_is_number(json_value, &result));
    return result;
}

LANGNES_JSON_API langnes_json_error_code_t langnes_json_value_get_number(
    langnes_json_value_t* json_value, double* result) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!json_value || !result) {
            throw invalid_argument{};
        }
        *result = required_dynamic_cast<value*>(json_value)->as_number();
    });
}

LANGNES_JSON_API double
langnes_json_value_get_number_s(langnes_json_value_t* json_value) {
    double result{};
    langnes_json_check_error(
        langnes_json_value_get_number(json_value, &result));
    return result;
}

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_set_number(langnes_json_value_t* json_value, double value) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!json_value) {
            throw invalid_argument{};
        }
        auto* value_{required_dynamic_cast<class value*>(json_value)};
        *value_ = value;
    });
}

//
// JSON boolean
//

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_boolean_new(bool value, langnes_json_value_t** result) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!result) {
            throw invalid_argument{};
        }
        *result = new langnes::json::value{value};
    });
}

LANGNES_JSON_API langnes_json_value_t*
langnes_json_value_boolean_new_s(bool value) {
    langnes_json_value_t* result{};
    langnes_json_check_error(langnes_json_value_boolean_new(value, &result));
    return result;
}

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_is_boolean(langnes_json_value_t* json_value, bool* result) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!json_value || !result) {
            throw invalid_argument{};
        }
        *result = required_dynamic_cast<value*>(json_value)->is_boolean();
    });
}

LANGNES_JSON_API bool
langnes_json_value_is_boolean_s(langnes_json_value_t* json_value) {
    bool result{};
    langnes_json_check_error(
        langnes_json_value_is_boolean(json_value, &result));
    return result;
}

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_get_boolean(langnes_json_value_t* json_value, bool* result) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!json_value || !result) {
            throw invalid_argument{};
        }
        *result = required_dynamic_cast<value*>(json_value)->as_boolean();
    });
}

LANGNES_JSON_API bool
langnes_json_value_get_boolean_s(langnes_json_value_t* json_value) {
    bool result{};
    langnes_json_check_error(
        langnes_json_value_get_boolean(json_value, &result));
    return result;
}

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_set_boolean(langnes_json_value_t* json_value, bool value) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!json_value) {
            throw invalid_argument{};
        }
        *required_dynamic_cast<class value*>(json_value) = value;
    });
}

//
// JSON null
//

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_null_new(langnes_json_value_t** result) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!result) {
            throw invalid_argument{};
        }
        *result = new langnes::json::value{nullptr};
    });
}

LANGNES_JSON_API langnes_json_value_t* langnes_json_value_null_new_s() {
    langnes_json_value_t* result{};
    langnes_json_check_error(langnes_json_value_null_new(&result));
    return result;
}

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_is_null(langnes_json_value_t* json_value, bool* result) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!json_value || !result) {
            throw invalid_argument{};
        }
        *result = required_dynamic_cast<value*>(json_value)->is_null();
    });
}

LANGNES_JSON_API bool
langnes_json_value_is_null_s(langnes_json_value_t* json_value) {
    bool result{};
    langnes_json_check_error(langnes_json_value_is_null(json_value, &result));
    return result;
}

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_set_null(langnes_json_value_t* json_value) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!json_value) {
            throw invalid_argument{};
        }
        *required_dynamic_cast<value*>(json_value) = nullptr;
    });
}

//
// JSON object
//

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_object_new(langnes_json_value_t** result) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!result) {
            throw invalid_argument{};
        }
        *result = new value{make_object({})};
    });
}

LANGNES_JSON_API langnes_json_value_t* langnes_json_value_object_new_s() {
    langnes_json_value_t* result{};
    langnes_json_check_error(langnes_json_value_object_new(&result));
    return result;
}

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_object_new_with_members(
    langnes_json_object_member_t* members, size_t length,
    langnes_json_value_t** result) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!members || !result) {
            // Free the member values since this function normally moves them.
            if (members) {
                free_object_members(members, length);
            }
            throw invalid_argument{};
        }
        auto object{make_object({})};
        set_object_members(object, members, length);
        *result = new value{std::move(object)};
    });
}

LANGNES_JSON_API langnes_json_value_t*
langnes_json_value_object_new_with_members_s(
    langnes_json_object_member_t* members, size_t length) {
    langnes_json_value_t* result{};
    langnes_json_check_error(
        langnes_json_value_object_new_with_members(members, length, &result));
    return result;
}

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_is_object(langnes_json_value_t* json_value, bool* result) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!json_value || !result) {
            throw invalid_argument{};
        }
        *result = required_dynamic_cast<value*>(json_value)->is_object();
    });
}

LANGNES_JSON_API bool
langnes_json_value_is_object_s(langnes_json_value_t* json_value) {
    bool result{};
    langnes_json_check_error(langnes_json_value_is_object(json_value, &result));
    return result;
}

LANGNES_JSON_API langnes_json_error_code_t langnes_json_value_object_get_value(
    langnes_json_value_t* json_object, const char* member_name,
    langnes_json_value_t** result) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!json_object || !member_name || !result) {
            throw invalid_argument{};
        }
        *result = std::addressof(required_dynamic_cast<value*>(json_object)
                                     ->as_object()
                                     .at(member_name));
    });
}

LANGNES_JSON_API langnes_json_value_t*
langnes_json_value_object_get_value_s(langnes_json_value_t* json_object,
                                      const char* member_name) {
    langnes_json_value_t* result{};
    langnes_json_check_error(
        langnes_json_value_object_get_value(json_object, member_name, &result));
    return result;
}

LANGNES_JSON_API langnes_json_error_code_t langnes_json_value_object_set_value(
    langnes_json_value_t* json_object, const char* member_name,
    langnes_json_value_t* member_value) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!json_object || !member_name || !member_value) {
            throw invalid_argument{};
        }
        required_dynamic_cast<value*>(json_object)->as_object()[member_name] =
            std::move(*required_dynamic_cast<value*>(member_value));
        langnes_json_value_free(member_value);
    });
}

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_set_object(langnes_json_value_t* json_object) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!json_object) {
            throw invalid_argument{};
        }
        *required_dynamic_cast<value*>(json_object) = make_object({});
    });
}

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_set_object_with_members(
    langnes_json_value_t* json_object, langnes_json_object_member_t* members,
    size_t length) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!json_object || !members) {
            // Free the member values since this function normally moves them.
            if (members) {
                free_object_members(members, length);
            }
            throw invalid_argument{};
        }
        auto& object{*required_dynamic_cast<value*>(json_object)};
        object = make_object({});
        set_object_members(object, members, length);
    });
}

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_object_get_members_length(langnes_json_value_t* json_object,
                                             size_t* result) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!json_object || !result) {
            throw invalid_argument{};
        }
        *result =
            required_dynamic_cast<value*>(json_object)->as_object().size();
    });
}

LANGNES_JSON_API size_t langnes_json_value_object_get_members_length_s(
    langnes_json_value_t* json_object) {
    size_t result{};
    langnes_json_check_error(
        langnes_json_value_object_get_members_length(json_object, &result));
    return result;
}

LANGNES_JSON_API langnes_json_error_code_t langnes_json_value_object_get_member(
    langnes_json_value_t* json_object, size_t index,
    langnes_json_object_member_t* result) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!json_object || !result) {
            throw invalid_argument{};
        }
        auto& entry{required_dynamic_cast<value*>(json_object)
                        ->as_object()
                        .entry_at(index)};
        *result = langnes_json_object_member_t{entry.first.c_str(),
                                               std::addressof(entry.second)};
    });
}

LANGNES_JSON_API langnes_json_object_member_t
langnes_json_value_object_get_member_s(langnes_json_value_t* json_object,
                                       size_t index) {
    langnes_json_object_member_t result{};
    langnes_json_check_error(
        langnes_json_value_object_get_member(json_object, index, &result));
    return result;
}

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_object_clear(langnes_json_value_t* json_object) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!json_object) {
            throw invalid_argument{};
        }
        required_dynamic_cast<value*>(json_object)->as_object().clear();
    });
}

//
// JSON array
//

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_array_new(langnes_json_value_t** result) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!result) {
            throw invalid_argument{};
        }
        *result = new value{make_array()};
    });
}

LANGNES_JSON_API langnes_json_value_t* langnes_json_value_array_new_s() {
    langnes_json_value_t* result{};
    langnes_json_check_error(langnes_json_value_array_new(&result));
    return result;
}

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_array_new_with_elements(langnes_json_value_t** values,
                                           size_t length,
                                           langnes_json_value_t** result) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!values || !result) {
            // Free the values since this function normally moves them.
            if (values) {
                free_values(values, length);
            }
            throw invalid_argument{};
        }
        auto array = make_array();
        set_array_elements(array, values, length);
        *result = new value{std::move(array)};
    });
}

LANGNES_JSON_API langnes_json_value_t*
langnes_json_value_array_new_with_elements_s(langnes_json_value_t** values,
                                             size_t length) {
    langnes_json_value_t* result{};
    langnes_json_check_error(
        langnes_json_value_array_new_with_elements(values, length, &result));
    return result;
}

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_is_array(langnes_json_value_t* json_value, bool* result) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!json_value || !result) {
            throw invalid_argument{};
        }
        *result = required_dynamic_cast<value*>(json_value)->is_array();
    });
}

LANGNES_JSON_API bool
langnes_json_value_is_array_s(langnes_json_value_t* json_value) {
    bool result{};
    langnes_json_check_error(langnes_json_value_is_array(json_value, &result));
    return result;
}

LANGNES_JSON_API langnes_json_error_code_t langnes_json_value_array_get_length(
    langnes_json_value_t* json_array, size_t* result) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!json_array || !result) {
            throw invalid_argument{};
        }
        *result = required_dynamic_cast<value*>(json_array)->as_array().size();
    });
}

LANGNES_JSON_API size_t
langnes_json_value_array_get_length_s(langnes_json_value_t* json_array) {
    size_t result{};
    langnes_json_check_error(
        langnes_json_value_array_get_length(json_array, &result));
    return result;
}

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_array_clear(langnes_json_value_t* json_array) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!json_array) {
            throw invalid_argument{};
        }
        required_dynamic_cast<value*>(json_array)->as_array().clear();
    });
}

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_array_push(langnes_json_value_t* json_array,
                              langnes_json_value_t* json_array_element) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!json_array || !json_array_element) {
            throw invalid_argument{};
        }
        required_dynamic_cast<value*>(json_array)
            ->as_array()
            .push_back(
                std::move(*required_dynamic_cast<value*>(json_array_element)));
        langnes_json_value_free(json_array_element);
    });
}

LANGNES_JSON_API langnes_json_error_code_t langnes_json_value_array_get_item(
    langnes_json_value_t* value, size_t index, langnes_json_value_t** result) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!value || !result) {
            throw invalid_argument{};
        }
        *result = std::addressof(
            required_dynamic_cast<class value*>(value)->as_array().at(index));
    });
}

LANGNES_JSON_API langnes_json_value_t*
langnes_json_value_array_get_item_s(langnes_json_value_t* value, size_t index) {
    langnes_json_value_t* result{};
    langnes_json_check_error(
        langnes_json_value_array_get_item(value, index, &result));
    return result;
}

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_set_array(langnes_json_value_t* json_array) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!json_array) {
            throw invalid_argument{};
        }
        *required_dynamic_cast<value*>(json_array) =
            langnes::json::make_array();
    });
}

LANGNES_JSON_API langnes_json_error_code_t
langnes_json_value_set_array_with_elements(langnes_json_value_t* json_array,
                                           langnes_json_value_t** values,
                                           size_t length) {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    return filter_error([&] {
        if (!json_array || !values) {
            // Free the values since this function normally moves them.
            if (values) {
                free_values(values, length);
            }
            throw invalid_argument{};
        }
        auto& array{*required_dynamic_cast<value*>(json_array)};
        array = make_array();
        set_array_elements(array, values, length);
    });
}

} // extern "C"
