#pragma once

#if __cplusplus > 202002L
#include "concepts.hpp"
#endif
#include "context.hpp"
#include "has_error.hpp"
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
#if __cplusplus > 202002L
template<typename V, concepts::Error E>
inline std::expected<V, E> with_context(std::expected<V, E> &&exp, Context &&context)
{
  return exp.transform_error([cont = std::move(context)](auto x) mutable {
    x.consume_context(std::move(cont));
    return x;
  });
}
#endif

/**
 * @brief Helper to add context to std::expected holding an error.
 *
 * @tparam V Type of the expected value
 * @tparam E Type of the error
 * @param exp Object to which context will be added
 * @param context Context to add
 * @return std::expected<V, E>
 */
#if __cplusplus > 202002L
template<concepts::Error E>
#else
template<typename E>
#endif
inline std::optional<E> with_context(std::optional<E> &&exp, Context &&context)
{
  if (has_error(exp)) { exp = std::optional<E>{ exp.value().consume_context(std::move(context)) }; }
  return exp;
}
}// namespace anywho