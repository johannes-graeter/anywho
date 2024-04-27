#pragma once

#include "concepts.hpp"
#include "errors.hpp"
#include "with_context.hpp"
#include <expected>
#include <functional>

namespace anywho {

/**
 * @brief Factory function for functions using boolean success flags
 *
 * @tparam T Type of the expected value
 * @tparam E Error type
 * @param has_no_error Value of the boolean flag
 * @param truth_value Expected value
 * @param error Instance of the error that shall be returned
 * @return std::expected<T, E>
 */
template<typename T, concepts::Error E>
inline std::expected<T, E> make_error(bool has_no_error, T truth_value, E &&error)
{
  if (has_no_error) {
    return std::expected<T, E>{ truth_value };
  } else {
    return std::unexpected(error);
  }
}

/**
 * @brief Factory function for functions using boolean success flags
 *
 * @tparam T Type of the expected value
 * @tparam E Error type
 * @param callable Callable in which the function that shall be evaluated is wrapped
 * @param error Instance of the error that shall be returned
 * @return std::expected<T, E>
 */
template<typename T, concepts::Error E>
inline std::expected<T, E> make_error(std::function<std::tuple<bool, T>(void)> callable, E &&error)
{
  const auto [has_no_error, truth_value] = callable();

  return make_error(has_no_error, truth_value, std::move(error));
}

/**
 * @brief Factory function for functions std::optional<Error>
 *
 * @tparam T Type of the expected value
 * @tparam E Error type
 * @param error optional error, having error or not
 * @param truth_value Expected value
 * @return std::expected<T, E>
 */
template<typename T, concepts::Error E> inline std::expected<T, E> make_error(std::optional<E> &&error, T truth_value)
{
  if (!has_error(error)) {
    return std::expected<T, E>{ truth_value };
  } else {
    return std::unexpected(error.value());
  }
}

/**
 * @brief Factory function for functions std::optional<Error>
 *
 * @tparam T Type of the expected value
 * @tparam E Error type
 * @param callable Callable in which the function that shall be evaluated is wrapped
 * @return std::expected<T, E>
 */
template<typename T, concepts::Error E>
inline std::expected<T, E> make_error(std::function<std::tuple<std::optional<E>, T>(void)> callable)
{
  auto [error, truth_value] = callable();

  return make_error(std::move(error), truth_value);
}

/**
 * @brief Factory function for functions using std::error_code
 *
 * @tparam T Type of the expected value
 * @param error_code Error code that was returned by the function to be evaluated
 * @param truth_value Expected value
 * @return std::expected<T, E>
 */
template<typename T> inline std::expected<T, ErrorFromCode> make_error(std::error_code error_code, T truth_value)
{
  if (!error_code) {
    return std::expected<T, ErrorFromCode>{ truth_value };
  } else {
    return std::unexpected(ErrorFromCode(error_code));
  }
}

/**
 * @brief Factory function for functions using std::error_code
 *
 * @tparam T Type of the expected value
 * @param callable Callable in which the function that shall be evaluated is wrapped
 * @param truth_value Expected value
 * @return std::expected<T, E>
 */
template<typename T>
inline std::expected<T, ErrorFromCode> make_error(std::function<std::tuple<std::error_code, T>(void)> callable)
{
  const auto [error_code, truth_value] = callable();

  return make_error(error_code, truth_value);
}

}// namespace anywho