#pragma once

#include "concepts.hpp"
#include "context.hpp"
#include <expected>

namespace anywho {
/**
 * @brief Helper to add context to std::expected holding an error.
 *
 * @tparam V Type of the expected value
 * @tparam E Type of the error
 * @param exp Object to which context will be added
 * @param context Context to add
 * @return std::expected<V, E>
 */
template<typename V, concepts::Error E>
inline std::expected<V, E> with_context(std::expected<V, E> &&exp, Context &&context)
{
  return exp.transform_error([cont = std::move(context)](auto x) mutable {
    x.consume_context(std::move(cont));
    return x;
  });
}
}// namespace anywho