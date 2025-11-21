#pragma once

#include <string_view>
#include <optional>
#include <array>
#include <utility> // For std::pair

// // --- Include the XEnum template from above ---
// template <typename T> struct XEnum {
//     T value;

//     // Constructors
//     constexpr XEnum() = default;
//     constexpr XEnum(T v) : value(v) {}

//     // Comparisons with another wrapper (of the same type)
//     friend constexpr bool operator==(const XEnum &a, const XEnum &b) { return a.value == b.value; }
//     friend constexpr bool operator!=(const XEnum &a, const XEnum &b) { return a.value != b.value; }

//     // Comparisons with the raw enum value
//     friend constexpr bool operator==(const XEnum &a, T v) { return a.value == v; }
//     friend constexpr bool operator!=(const XEnum &a, T v) { return a.value != v; }
//     friend constexpr bool operator==(T v, const XEnum &a) { return a.value == v; }
//     friend constexpr bool operator!=(T v, const XEnum &a) { return a.value != v; }
// };
