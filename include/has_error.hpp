#pragma once
#include <expected>
#include <optional>
namespace anywho {

// We might want to use the concepts here but than we could not represent error as strings or other types...
template<typename E> inline bool has_error(const std::optional<E> &x) { return x.has_value(); }

#if __cplusplus > 202002L
template<typename T, typename E> inline bool has_error(const std::expected<T, E> &x) { return !x.has_value(); }
#endif
}// namespace anywho