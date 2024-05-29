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
#define ANYWHO(expr)                                                               \
  __extension__({                                                                  \
    auto __result = expr;                                                          \
    if (anywho::has_error(__result)) { return std::unexpected(__result.error()); } \
    __result.value();                                                              \
  })

// Alias that is shorter
#define TRY ANYWHO

#define ANYWHO_OPT(expr)                                                              \
  __extension__({                                                                     \
    auto __result = expr;                                                             \
    if (anywho::has_error(__result)) { return std::make_optional(__result.error()); } \
    __result.value();                                                                 \
  })

#define TRY_O ANYWHO_OPT

#endif// c++23 guard
/**
 * @brief Same as ANYWHO but for std::optional<Error>. For projects that are bound to version before cpp23.
 *
 */
#define ANYWHO_LEGACY(expr)                                                           \
  __extension__({                                                                     \
    auto __result = expr;                                                             \
    if (anywho::has_error(__result)) { return std::make_optional(__result.value()); } \
  })

#define TRY_LEG ANYWHO_LEGACY

#if defined(__clang__)
#pragma clang diagnostic pop
#endif