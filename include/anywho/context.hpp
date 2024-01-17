#pragma once

#include <expected>
#include <format>

#include "fixed_string.hpp"

namespace anywho {
struct Context
{
  FixedString<128> message;
  uint line;
  FixedString<128> file;

  std::string format() const
  {
    return std::format("{}:{} -> {}", static_cast<std::string>(file), line, static_cast<std::string>(message));
  }
};

}// namespace anywho