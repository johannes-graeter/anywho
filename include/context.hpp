#pragma once
#include "fixed_string.hpp"
#include "format.hpp"

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
  uint line{ 0 };
  FixedString<128> file{ "" };

  std::string format() const
  {
    if (static_cast<std::string>(file) == "" && line == 0) {
      return format_ns::format("{}", static_cast<std::string>(message));
    }

    return format_ns::format("{}:{} -> {}", static_cast<std::string>(file), line, static_cast<std::string>(message));
  }
};

}// namespace anywho