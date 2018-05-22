/*
 * Copyright (c) 2018 Trail of Bits, Inc.
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

#ifndef FCD_COMPAT_MEMORYSSA_H_
#define FCD_COMPAT_MEMORYSSA_H_

#include "remill/BC/Version.h"

#if LLVM_VERSION_NUMBER < LLVM_VERSION(3, 9)

#elif LLVM_VERSION_NUMBER < LLVM_VERSION(5, 0)

#include <llvm/Transforms/Utils/MemorySSA.h>

#elif LLVM_VERSION_NUMBER >= LLVM_VERSION(5, 0)

#include <llvm/Analysis/MemorySSA.h>
#include <llvm/Analysis/MemorySSAUpdater.h>

#endif

#endif  // FCD_COMPAT_MEMORYSSA_H_