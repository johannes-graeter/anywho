#pragma once

#include <optional>
#if __cplusplus > 202002L
#include "concepts.hpp"
#endif

namespace anywho {
#if __cplusplus > 202002L
template<concepts::Error E>
#else
template<typename E>
#endif
using ErrorState = std::optional<E>;

constexpr auto NoError = std::nullopt;
}// namespace anywho