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

// NOLINTNEXTLINE(modernize-use-using)
typedef int langnes_json_error_code_t;

extern const langnes_json_error_code_t langnes_json_error_parse_error;
extern const langnes_json_error_code_t langnes_json_error_out_of_range;
extern const langnes_json_error_code_t langnes_json_error_bad_access;
extern const langnes_json_error_code_t langnes_json_error_invalid_state;
extern const langnes_json_error_code_t langnes_json_error_invalid_argument;
extern const langnes_json_error_code_t langnes_json_error_unspecified;
extern const langnes_json_error_code_t langnes_json_error_ok;
