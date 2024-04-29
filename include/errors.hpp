#pragma once

#include "context.hpp"
#include <memory>
#include <string>
#include <system_error>
#include <vector>

namespace anywho {

/**
 * @brief Most basic form of an error. Use it directly or inherit from it to specialize your errors.
 *        Uses dynamic memory allocation.
 *
 */
class GenericError
{
public:
  virtual ~GenericError() = default;

  [[nodiscard]] std::string format() const
  {
    // This would be nicer with ranges like but we would need more deps
    // #include <algorithm>
    // #include <ranges>
    //     return std::ranges::fold_left(contexts_ | std::views::transform([](const auto &el) { return "::" +
    //     el.format(); }),
    //              "",
    //              std::plus<std::string>())
    //            | std::views::drop(2);
    std::string out = message();
    for (const auto &el : contexts_) { out += "::" + el.format(); }

    return out;
  }

  void consume_context(anywho::Context &&context) { contexts_.emplace_back(std::move(context)); }
  [[nodiscard]] virtual constexpr std::string message() const { return "generic error happened"; }
  [[nodiscard]] virtual size_t id() const { return std::hash<std::string>{}(message()); }

protected:
  std::vector<Context> contexts_{};
};

/**
 * @brief Error without dynamic memory allocation. Can be used as a base for custom errors.
 *        Context that is longer than the specified size will be ommitted.
 *
 * @tparam Size Maximum size of the resulting error message
 */
template<uint Size> class FixedSizeError
{
public:
  virtual ~FixedSizeError() = default;

  [[nodiscard]] std::string format() const { return message() + static_cast<std::string>(message_); }

  void consume_context(anywho::Context &&context)
  {
    message_ = format_ns::format("{}::{}", static_cast<std::string>(message_), context.format());
  }

  [[nodiscard]] virtual constexpr std::string message() const { return "fixed size error happened"; }
  [[nodiscard]] virtual size_t id() const { return std::hash<std::string>{}(message()); }

private:
  FixedString<Size> message_{};
};

/**
 * @brief Error class that is used for functions that return std::error_code
 *
 */
class ErrorFromCode final : public GenericError
{
public:
  ErrorFromCode(const std::error_code &code) : code_{ code } {}
  ErrorFromCode(std::error_code &&code) : code_{ std::move(code) } {}
  [[nodiscard]] constexpr std::string message() const override
  {
    return format_ns::format("error happened with code {} and message {}", code_.value(), code_.message());
  }

  [[nodiscard]] const std::error_code &get_code() const { return code_; }

private:
  std::error_code code_;
};

}// namespace anywho