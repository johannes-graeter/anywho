#include <expected>
#include <fmt/core.h>
#include "macros.hpp"


std::expected<int, std::string> myfunc() {
  return std::unexpected("lalala");
}

std::expected<int, std::string> myfunc0() {
  auto val = ANYWHO(myfunc());
  return 3 * val;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(){
  auto result = myfunc0();
  if (!result.has_value()) {
    fmt::print("error: {}", result.error());
  }
  return 0;
}
