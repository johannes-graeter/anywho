#pragma once

#include "context.hpp"
#include <concepts>
#include <string>

namespace anywho {
namespace concepts {
  template<typename T>
  concept Error = requires(T &obj, Context c) {
    {
      obj.format()
    } -> std::convertible_to<std::string>;
    {
      obj.message()
    } -> std::convertible_to<std::string>;
    {
      obj.id()
    } -> std::unsigned_integral;
    obj.consume_context(std::move(c));
  };

  template<typename T>
  concept Catchable = requires(T &obj) {
    {
      obj.what()
    } -> std::convertible_to<std::string>;
  };
}// namespace concepts
}// namespace anywho