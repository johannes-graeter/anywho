#pragma once

#include <expected>
#include <format>

#include "fixed_string.hpp"

namespace anywho {
/**
 * @brief The context that is given as additional information when handing over errors between functions.
 *        Nice to use with designated inititializers like
 *        auto x = ANYWHO(myFunc(input).with_context({.message="you may not pass!", .file=__FILE__, .line=__LINE__}));
 *
 */
struct Context final
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