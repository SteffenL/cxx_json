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

#ifndef LANGNES_JSON_API
#if defined(LANGNES_JSON_SHARED) || defined(LANGNES_JSON_BUILD_SHARED)
#if defined(_WIN32) || defined(__CYGWIN__)
#if defined(LANGNES_JSON_BUILD_SHARED)
#define LANGNES_JSON_API extern __declspec(dllexport)
#else
#define LANGNES_JSON_API extern __declspec(dllimport)
#endif
#else
#define LANGNES_JSON_API extern __attribute__((visibility("default")))
#endif
#else
#define LANGNES_JSON_API extern
#endif
#endif
