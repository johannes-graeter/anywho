#pragma once
// gnu statement expression is currently the only way to solve this
#pragma clang diagnostic ignored "-Wgnu-statement-expression"
#define ANYWHO(expr)                                                     \
  ({                                                                     \
    auto result = expr;                                                  \
    if (!result.has_value()) { return std::unexpected(result.error()); } \
    result.value();                                                      \
  })