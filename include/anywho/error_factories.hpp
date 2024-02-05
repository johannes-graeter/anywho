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

}// namespace anywho