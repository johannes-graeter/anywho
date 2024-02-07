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

template<typename T, concepts::Error E>
inline std::expected<T, E> make_error(std::function<std::tuple<bool, T>(void)> callable, E &&error)
{
  const auto &[has_no_error, truth_value] = callable();

  return make_error(has_no_error, truth_value, std::move(error));
}

template<typename T> inline std::expected<T, ErrorFromCode> make_error(std::error_code error_code, T truth_value)
{
  if (!error_code) {
    return truth_value;
  } else {
    return std::unexpected(ErrorFromCode(error_code));
  }
}

template<typename T>
inline std::expected<T, ErrorFromCode> make_error(std::function<std::tuple<std::error_code, T>(void)> callable)
{
  const auto &[error_code, truth_value] = callable();

  return make_error(error_code, truth_value);
}

}// namespace anywho