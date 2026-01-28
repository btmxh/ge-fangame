#pragma once

#include <cassert>
#include <cstdint>

#define GE_ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

namespace ge {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using usize = std::uintptr_t;
using isize = std::intptr_t;

using f32 = float;
// f64 is not available on the targeted platform

// assert_se: assert in side-effect context
template <class T> inline void assert_se(T condition) { assert(condition); }

} // namespace ge
