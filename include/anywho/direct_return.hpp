#pragma once
// gnu statement expression is currently the only way to solve this
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-statement-expression"
#endif
#define ANYWHO(expr)                                                     \
  __extension__({                                                        \
    auto result = expr;                                                  \
    if (!result.has_value()) { return std::unexpected(result.error()); } \
    result.value();                                                      \
  })
#if defined(__clang__)
#pragma clang diagnostic pop
#endif