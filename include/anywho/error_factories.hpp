#pragma once

#include "concepts.hpp"
#include "errors.hpp"
#include "with_context.hpp"
#include <expected>
#include <functional>

namespace anywho {

template<typename T, concepts::Error E>
inline std::expected<T, E> make_error(bool has_no_error, T truth_value, E &&error)
{
  if (has_no_error) {
    return truth_value;
  } else {
    return std::unexpected(error);
  }
}

template<typename T> inline std::expected<T, ErrorFromCode> make_error(std::error_code error_code, T truth_value)
{
  if (!error_code) {
    return truth_value;
  } else {
    return std::unexpected(ErrorFromCode(error_code));
  }
}

template<typename T, concepts::Error E, concepts::Catchable Exc>
inline std::expected<T, E> make_any_error_from_throwable(std::function<T(void)> invocable, E &&error)
{
  try {
    return invocable();
  } catch (const Exc &exc) {
    std::expected<T, E> exp = std::unexpected(std::move(error));
    return with_context(std::move(exp), { .message = exc.what(), .line = __LINE__, .file = std::string(__FILE__) });
  }
}

template<typename T, concepts::Catchable Exc>
inline std::expected<T, ErrorFromException> make_error_from_throwable(std::function<T(void)> invocable)
{
  try {
    return invocable();
  } catch (const Exc &exc) {
    return std::unexpected(ErrorFromException(std::make_shared<std::exception>(exc)));
  }
}


}// namespace anywho