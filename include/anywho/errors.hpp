#pragma once

#include "context.hpp"
#include <string>
#include <vector>

namespace anywho {

class GenericError
{
public:
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
  [[nodiscard]] static std::string message() { return "generic error happened"; }
  [[nodiscard]] static size_t id() { return std::hash<std::string>{}(message()); }

private:
  std::vector<Context> contexts_{};
};

template<uint Size> class FixedSizeError
{
public:
  [[nodiscard]] std::string format() const { return message() + static_cast<std::string>(message_); }

  void consume_context(anywho::Context &&context)
  {
    message_ = std::format("{}::{}", static_cast<std::string>(message_), context.format());
  }

  [[nodiscard]] static std::string message() { return "fixed size error happened"; }
  [[nodiscard]] static size_t id() { return std::hash<std::string>{}(message()); }

private:
  FixedString<Size> message_{};
};
}// namespace anywho