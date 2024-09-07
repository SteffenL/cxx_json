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

// NOLINTBEGIN(modernize-use-using)
typedef struct langnes_json_value_t langnes_json_value_t;
typedef struct langnes_json_string_t langnes_json_string_t;

typedef enum {
    langnes_json_value_type_object,
    langnes_json_value_type_array,
    langnes_json_value_type_string,
    langnes_json_value_type_number,
    langnes_json_value_type_boolean,
    langnes_json_value_type_null
} langnes_json_value_type_t;
// NOLINTEND(modernize-use-using)
