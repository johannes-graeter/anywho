#pragma once

#include "errors.hpp"
#include "format.hpp"
#include <exception>
#include <memory>
#include <string>

namespace anywho {

/**
 * @brief Build an error from an exception. This serves as a brdige from exception based error handling to anywho.
 * Due to a bug in libc++ and clang, we need to set ASAN_OPTIONS=alloc_dealloc_mismatch=0.
 * Since this might be inadequte for some projects we seperate this class from the rest.
 *
 */
class ErrorFromException final : public GenericError
{
public:
  ErrorFromException(const std::shared_ptr<std::exception> &exc) : exc_{ exc } {}
  [[nodiscard]] constexpr std::string message() const override
  {
    return format_ns::format("error happened with exception '{}'", exc_->what());
  }

  [[nodiscard]] const std::shared_ptr<std::exception> &get_exception_ptr() const { return exc_; }

private:
  ///@brief Pointer to exception interface. Needs to be shared_ptr, since std::expected needs copyiability.
  std::shared_ptr<std::exception> exc_;
};

#if __cplusplus > 202002L
/**
 * @brief Factory for ErrorFromException. This can be used as a bridge from exception bsed error handling.
 * Example:
 *  const std::expected<int, anywho::ErrorFromException> exp =
 *    anywho::make_error_from_throwable<int, std::runtime_error>([]() { return myFunc(); });
 * This would be great to have with the other error_factories, but due to a bug in clang we need to seperate it
 * (see ErrorFromException)
 *
 * @tparam T Return type of the wrapping callable
 * @tparam Exc Exception that you want to catch
 * @param invocable Wrap the function that throws into this callable.
 * @return std::expected<T, ErrorFromException>
 */
template<typename T, concepts::Catchable Exc>
inline std::expected<T, ErrorFromException> make_error_from_throwable(std::function<T(void)> invocable)
{
  try {
    return invocable();
  } catch (const Exc &exc) {
    return std::unexpected(ErrorFromException(std::make_shared<std::exception>(exc)));
  }
}

/**
 * @brief Factory for any Error from a function that throws and exception.
 * This can be used as a bridge from exception based error handling. Only use this if you can not use
 * ErrorFramException but need a specialized error.
 * Example:
 *  const std::expected<int, MyError> exp =
 *    anywho::make_any_error_from_throwable<int, MyError, std::runtime_error>([]() { return myFunc(); }, MyError{});
 * This would be great to have with the other error_factories, but due to a bug in clang we need to seperate it
 * (see ErrorFromException)
 *
 * @tparam T Return type of the wrapping callable
 * @tparam E Error that you want to generate
 * @tparam Exc Exception that you want to catch
 * @param invocable Wrap the function that throws into this callable.
 * @param error Instance of the error that shall be returned.
 * @return std::expected<T, E>
 */
template<typename T, concepts::Error E, concepts::Catchable Exc>
inline std::expected<T, E> make_any_error_from_throwable(std::function<T(void)> invocable, E &&error)
{
  try {
    return invocable();
  } catch (const Exc &exc) {
    std::expected<T, E> exp = std::unexpected(std::move(error));
    return with_context(std::move(exp), { exc.what() });
  }
}
#endif


}// namespace anywho