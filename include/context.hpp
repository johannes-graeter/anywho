#pragma once

#if __cplusplus >= 202002L
#include <source_location>
#endif

#include "fixed_string.hpp"
#include "format.hpp"

namespace anywho {

static constexpr size_t CONTEXT_STRING_SIZE{ 128 };
using ContextString = FixedString<CONTEXT_STRING_SIZE>;


/**
 * @brief Intermediate struct that will allow for use with desiganted initilializers
 *
 */
struct ContextParameterProxy
{
  ContextString message;
  uint line;
  ContextString file;
};

/**
 * @brief The context that is given as additional information when handing over errors between functions.
 *        Use like
 *        auto x = ANYWHO(myFunc(input).with_context({"you may not pass!"}));
 *        In c++17 this will not give location but in c++20 it will by the magic of source_location
 *
 *        Also nice to use with designated inititializers if you want more expressiveness like
 *        auto x = ANYWHO(myFunc(input).with_context(Context{{.message="you may not pass!", .file="myfile",
 *                                                            .line=1234}}));
 *
 */
class Context final
{
public:
  Context() = default;

  explicit Context(ContextParameterProxy &&init)
    : message_{ std::move(init.message) }, line_{ std::move(init.line) }, file_{ std::move(init.file) }
  {}

  Context(ContextString &&msg, uint line, ContextString &&file)
    : message_{ std::move(msg) }, line_{ std::move(line) }, file_{ std::move(file) }
  {}

#if __cplusplus >= 202002L
  Context(ContextString &&msg, std::source_location &&location = std::source_location::current())
    : message_{ std::move(msg) }, line_{ std::move(location.line()) }, file_{ std::move(location.file_name()) }
  {}
#elif
  Context(ContextString &&msg) : message_{ std::move(msg) }, line_{ 0 }, file_{ "" } {}
#endif

  std::string format() const
  {
    if (static_cast<std::string>(file_) == "" && line_ == 0) {
      return format_ns::format("{}", static_cast<std::string>(message_));
    }

    return format_ns::format("{}:{} -> {}", static_cast<std::string>(file_), line_, static_cast<std::string>(message_));
  }

private:
  ContextString message_;
  uint line_;
  ContextString file_;
};

}// namespace anywho