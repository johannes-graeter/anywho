#pragma once
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-statement-expression"
#endif
/**
 * @brief Macro allowing to return directly the unexpected value or continue with the truth value (without std::expected
 * wrapped around it) the extension "gnu statement expression" is currently the only way to solve this but supported by
 * gcc, clang and msvc
 *
 */
#define ANYWHO(expr)                                                     \
  __extension__({                                                        \
    auto result = expr;                                                  \
    if (!result.has_value()) { return std::unexpected(result.error()); } \
    result.value();                                                      \
  })
#if defined(__clang__)
#pragma clang diagnostic pop
#endif