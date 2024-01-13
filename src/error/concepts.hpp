#pragma once

#include <string>

namespace anywho {
namespace concepts {
template <typename T, typename Context>
concept Error = requires(const T &obj, Context &&c) {
  { obj.format(); } -> std::convertible_to<std::string>;
  obj.consume_context(static_cast<Context &&>(c));
};
} // namespace concepts
} // namespace anywho