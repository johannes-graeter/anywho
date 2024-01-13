#pragma once

#include <expected>

#include "concepts.hpp"
#include "fixed_string.hpp"

namespace anywho {
struct Context {
  FixedString message;
  uint line;
  FixedString file_name;

  to_string() const {
    return fmt::format("from {}:{} -> {}", file_name, line, message );
  } 
};

template <typename V, anywho::concecpts::Error E>
inline std::expected<V, E> with_context(std::expected<V, E> &&exp,
                                        Context &&cont) {
  return exp.transform_error(
      [](const auto &x) { return x.consume_context(cont) });
}
} // namespace anywho