#pragma once
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-statement-expression"
#endif
#include "has_error.hpp"

#if __cplusplus > 202002L
/**
 * @brief Macro allowing to return directly the unexpected value or continue with the truth value (without std::expected
 * wrapped around it) the extension "gnu statement expression" is currently the only way to solve this but supported by
 * gcc, clang and msvc
 *
 */
#define ANYWHO(expr)                                                           \
  __extension__({                                                              \
    auto result = expr;                                                        \
    if (anywho::has_error(result)) { return std::unexpected(result.error()); } \
    result.value();                                                            \
  })

#define ANYWHO_OPT(expr)                                                          \
  __extension__({                                                                 \
    auto result = expr;                                                           \
    if (anywho::has_error(result)) { return std::make_optional(result.error()); } \
    result.value();                                                               \
  })

#endif// c++23 guard
/**
 * @brief Same as ANYWHO but for std::optional<Error>. For projects that are bound to version before cpp23.
 *        Attention: optEror.has_value() means it has an error which is unintuitive.
 *
 */
#define ANYWHO_LEGACY(expr)                                                       \
  __extension__({                                                                 \
    auto result = expr;                                                           \
    if (anywho::has_error(result)) { return std::make_optional(result.value()); } \
  })

#if defined(__clang__)
#pragma clang diagnostic pop
#endif