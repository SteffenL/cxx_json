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
#include "langnes_json/c/errors.h"
#include "langnes_json/cxx/errors.hpp"

const langnes_json_error_code_t langnes_json_error_invalid_argument =
    static_cast<langnes_json_error_code_t>(
        langnes::json::error_code::invalid_argument);

const langnes_json_error_code_t langnes_json_error_unspecified =
    static_cast<langnes_json_error_code_t>(
        langnes::json::error_code::unspecified);

const langnes_json_error_code_t langnes_json_error_ok =
    static_cast<langnes_json_error_code_t>(langnes::json::error_code::ok);
