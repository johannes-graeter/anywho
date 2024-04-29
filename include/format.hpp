#pragma once
#if __cplusplus > 202002L
// For C++20 and later, use std::format
#include <format>
namespace anywho {
namespace format_ns = std;
}// namespace anywho
#else
// For versions before C++20, use fmt::format
#include <fmt/core.h>
namespace anywho {
namespace format_ns = fmt;
}// namespace anywho
#endif
