#pragma once

#include "concepts.hpp"
#include "context.hpp"
#include <expected>

namespace anywho {
template<typename V, anywho::concepts::Error E>
inline std::expected<V, E> with_context(std::expected<V, E> &&exp, Context &&context)
{
  return exp.transform_error([cont = std::move(context)](auto x) mutable {
    x.consume_context(std::move(cont));
    return x;
  });
}
}// namespace anywho