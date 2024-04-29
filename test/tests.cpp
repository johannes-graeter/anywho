#include "anywho.hpp"
#include "extra.hpp"
#include <catch2/catch_test_macros.hpp>
#include <expected>
#include <format>
#include <type_traits>

namespace {
namespace direct_return_expected {
  std::expected<int, std::string> myfuncUnexpected() { return std::unexpected("my_msg"); }

  std::expected<int, std::string> myfuncValid() { return 3; }

  std::expected<int, std::string> myfuncUnexpectedRaised()
  {
    const int val = ANYWHO(myfuncUnexpected());
    return 3 * val;
  }

  std::expected<int, std::string> myfuncValidRaised()
  {
    const int val = ANYWHO(myfuncValid());
    return 3 * val;
  }
}// namespace direct_return_expected

struct DummyError final
{
  [[nodiscard]] static std::string format() { return "lala"; }
  void consume_context(anywho::Context &&abc) { abc_ = std::move(abc); }
  [[nodiscard]] static std::string message() { return "dummy error happened"; }
  [[nodiscard]] static size_t id() { return std::hash<std::string>{}(message()); }

  anywho::Context abc_;
};

namespace direct_return_optional {
  std::expected<int, DummyError> myfuncUnexpected() { return std::unexpected(DummyError{}); }

  std::expected<int, DummyError> myfuncValid() { return 3; }

  std::optional<DummyError> myfuncUnexpectedRaised(int &val)
  {
    int val_cur = ANYWHO_OPT(myfuncUnexpected());
    val = 3 * val_cur;

    return std::nullopt;
  }

  std::optional<DummyError> myfuncValidRaised(int &val)
  {
    int val_cur = ANYWHO_OPT(myfuncValid());
    val = 3 * val_cur;

    return std::nullopt;
  }

  std::optional<DummyError> myfuncUnexpectedRaised2(int &val)
  {
    ANYWHO_LEGACY(myfuncUnexpectedRaised(val));
    val = 3 * val;

    return std::nullopt;
  }

  std::optional<DummyError> myfuncValidRaised2(int &val)
  {
    ANYWHO_LEGACY(myfuncValidRaised(val));
    val = 3 * val;

    return std::nullopt;
  }
}// namespace direct_return_optional
std::expected<int, DummyError> testError() { return std::unexpected(DummyError{}); }

bool positiveOnlySquare(int num, int &output)
{
  if (num > 0) {
    output = num * num;

    return true;
  }

  return false;
}

std::error_code positiveOnlySquareWithErrorCode(int num, int &output)
{
  if (num > 0) {
    output = num * num;

    return std::error_code{};
  }

  return std::make_error_code(std::errc::result_out_of_range);
}

int positiveOnlySquareWithException(int num)
{
  if (num < 0) {
    throw std::runtime_error("is negative");
  } else {
    return num * num;
  }
}

std::optional<DummyError> positiveOnlySquareWithOptional(int num, int &output)
{
  if (num > 0) {
    output = num * num;

    return std::nullopt;
  }

  return DummyError{};
}

}// namespace

TEST_CASE("error get returned with anywho", "[direct_return]")
{
  auto result = direct_return_expected::myfuncUnexpectedRaised();
  REQUIRE(!result.has_value());
  REQUIRE(result.error() == "my_msg");
}
TEST_CASE("value get returned with anywho", "[direct_return]")
{
  auto result = direct_return_expected::myfuncValidRaised();
  REQUIRE(result.has_value());
  REQUIRE(result.value() == 9);
}

TEST_CASE("error get returned with anywho_opt", "[direct_return]")
{
  int val = 0;
  std::optional<DummyError> result = direct_return_optional::myfuncUnexpectedRaised(val);
  REQUIRE(anywho::has_error(result));
}
TEST_CASE("value get returned with anywho_opt", "[direct_return]")
{
  int val = 0;
  std::optional<DummyError> result = direct_return_optional::myfuncValidRaised(val);
  REQUIRE(!anywho::has_error(result));
  REQUIRE(val == 9);
}

TEST_CASE("error get returned with anywho_legacy", "[direct_return]")
{
  int val = 0;
  std::optional<DummyError> result = direct_return_optional::myfuncUnexpectedRaised2(val);
  REQUIRE(anywho::has_error(result));
}
TEST_CASE("value get returned with anywho_legacy", "[direct_return]")
{
  int val = 0;
  std::optional<DummyError> result = direct_return_optional::myfuncValidRaised2(val);
  REQUIRE(!anywho::has_error(result));
  REQUIRE(val == 27);
}

TEST_CASE("fixed string", "[FixedString]")
{
  static constexpr int StringSize = 128;
  const anywho::FixedString<StringSize> test_string{ "abc" };
  REQUIRE(static_cast<std::string>(test_string) == "abc");
  const anywho::Context context2{ .message = "test", .line = 10, .file = "la.cpp" };
  REQUIRE(context2.format() == "la.cpp:10 -> test");
}

TEST_CASE("with_context", "[with_context]")
{
  static constexpr int line = 100;
  auto exp = anywho::with_context(testError(), { .message = "test", .line = line, .file = "test.cpp" });
  REQUIRE(!exp.has_value());
  REQUIRE(exp.error().format() == "lala");
}

TEST_CASE("test GenericError", "[errors]")
{
  anywho::GenericError err{};
  REQUIRE(err.format() == err.message());
  constexpr int line = 76;
  err.consume_context({ .message = "abc", .line = line, .file = "tests.cpp" });
  err.consume_context({ .message = "abc2", .line = line + 1, .file = "tests.cpp" });
  REQUIRE(err.format() == err.message() + "::tests.cpp:76 -> abc::tests.cpp:77 -> abc2");
}

TEST_CASE("test FixedSizeError", "[errors]")
{
  constexpr uint Size = 1024;
  anywho::FixedSizeError<Size> err{};
  REQUIRE(err.format() == err.message());
  constexpr int line = 76;
  err.consume_context({ .message = "abc", .line = line, .file = "tests.cpp" });
  err.consume_context({ .message = "abc2", .line = line + 1, .file = "tests.cpp" });
  REQUIRE(err.format() == err.message() + "::tests.cpp:76 -> abc::tests.cpp:77 -> abc2");
}

TEST_CASE("test FixedSizeError overflow", "[errors]")
{
  constexpr uint Size = 11;
  anywho::FixedSizeError<Size> err{};
  constexpr int line = 76;
  err.consume_context({ .message = "abc", .line = line, .file = "tests.cpp" });
  err.consume_context({ .message = "abc2", .line = line + 1, .file = "tests.cpp" });
  REQUIRE(err.format() == err.message() + "::tests.cp");
}

TEST_CASE("test truth/false error factor, false case", "[error_factories]")
{
  int output = 0;
  bool ret = positiveOnlySquare(-3, output);
  const std::expected<int, anywho::GenericError> exp = anywho::make_error(ret, output, anywho::GenericError{});
  REQUIRE(!exp.has_value());
}

TEST_CASE("test truth/false error factor with function, false case", "[error_factories]")
{
  // Somehow template argument deduction does not work here, so we need to give it by hand.
  const std::expected<int, anywho::GenericError> exp = anywho::make_error<int, anywho::GenericError>(
    []() {
      int output = 0;
      bool ret = positiveOnlySquare(-3, output);
      return std::make_tuple(ret, output);
    },
    anywho::GenericError{});
  REQUIRE(!exp.has_value());
}

TEST_CASE("test truth/false error factor, truth case", "[error_factories]")
{
  int output = 0;
  // Originally this was designed to be callable from make_error without saving the retrun value, but this only works
  // for clang like
  // const std::expected<int, anywho::GenericError> exp = anywho::make_error(positiveOnlySquare(3,output), output,
  // anywho::GenericError{});
  bool ret = positiveOnlySquare(3, output);
  const std::expected<int, anywho::GenericError> exp = anywho::make_error(ret, output, anywho::GenericError{});
  REQUIRE(exp.has_value());
  REQUIRE(exp.value() == 9);
}

TEST_CASE("test truth/false error factor with function, truth case", "[error_factories]")
{
  // Somehow template argument deduction does not work here, so we need to give it by hand.
  const std::expected<int, anywho::GenericError> exp = anywho::make_error<int, anywho::GenericError>(
    []() {
      int output = 0;
      bool ret = positiveOnlySquare(3, output);
      return std::make_tuple(ret, output);
    },
    anywho::GenericError{});
  REQUIRE(exp.has_value());
  REQUIRE(exp.value() == 9);
}

TEST_CASE("test error from code factory, false case", "[error_factories]")
{
  int output = 0;
  auto ret = positiveOnlySquareWithErrorCode(-3, output);
  std::expected<int, anywho::ErrorFromCode> exp = anywho::make_error(ret, output);
  REQUIRE(!exp.has_value());
  REQUIRE(exp.error().get_code() == std::errc::result_out_of_range);
}

TEST_CASE("test error from code factory, truth case", "[error_factories]")
{
  int output = 0;
  auto ret = positiveOnlySquareWithErrorCode(3, output);
  std::expected<int, anywho::ErrorFromCode> exp = anywho::make_error(ret, output);
  REQUIRE(exp.has_value());
  REQUIRE(exp.value() == 9);
}

TEST_CASE("test error from code factory with function, false case", "[error_factories]")
{
  // Somehow template argument deduction does not work here, so we need to give it by hand.
  std::expected<int, anywho::ErrorFromCode> exp = anywho::make_error<int>([]() {
    int output = 0;
    auto ret = positiveOnlySquareWithErrorCode(-3, output);
    return std::make_tuple(ret, output);
  });
  REQUIRE(!exp.has_value());
  REQUIRE(exp.error().get_code() == std::errc::result_out_of_range);
}

TEST_CASE("test error from code factory with function, truth case", "[error_factories]")
{
  // Somehow template argument deduction does not work here, so we need to give it by hand.
  std::expected<int, anywho::ErrorFromCode> exp = anywho::make_error<int>([]() {
    int output = 0;
    auto ret = positiveOnlySquareWithErrorCode(3, output);
    return std::make_tuple(ret, output);
  });
  REQUIRE(exp.has_value());
  REQUIRE(exp.value() == 9);
}

TEST_CASE("test error from throwable factory, false case", "[error_factories]")
{
  // because of a bug in libc++ and clang we need to deactivate AddressSanitizer: alloc-dealloc-mismatch (see
  // https://github.com/llvm/llvm-project/issues/52771)
  {
    const std::expected<int, anywho::ErrorFromException> exp =
      anywho::make_error_from_throwable<int, std::runtime_error>([]() { return positiveOnlySquareWithException(-3); });
    REQUIRE(!exp.has_value());
  }

  {
    const std::expected<int, anywho::GenericError> exp =
      anywho::make_any_error_from_throwable<int, anywho::GenericError, std::runtime_error>(
        []() { return positiveOnlySquareWithException(-3); }, anywho::GenericError{});
    REQUIRE(!exp.has_value());
  }
}


TEST_CASE("test error from throwable factory, truth case", "[error_factories]")
{
  {
    const std::expected<int, anywho::ErrorFromException> exp =
      anywho::make_error_from_throwable<int, std::runtime_error>([]() { return positiveOnlySquareWithException(3); });
    REQUIRE(exp.has_value());
    REQUIRE(exp.value() == 9);
  }

  {
    const std::expected<int, anywho::GenericError> exp =
      anywho::make_any_error_from_throwable<int, anywho::GenericError, std::runtime_error>(
        []() { return positiveOnlySquareWithException(3); }, anywho::GenericError{});
    REQUIRE(exp.has_value());
    REQUIRE(exp.value() == 9);
  }
}

TEST_CASE("test optional<Error> factory, truth case", "[error_factories]")
{
  int output = 0;
  auto ret = positiveOnlySquareWithOptional(3, output);
  REQUIRE(!anywho::has_error(ret));
  std::expected<int, DummyError> exp = anywho::make_error(std::move(ret), output);
  REQUIRE(exp.has_value());
  REQUIRE(exp.value() == 9);
}

TEST_CASE("test optional<Error> factory, error case", "[error_factories]")
{
  int output = 0;
  auto ret = positiveOnlySquareWithOptional(-3, output);
  std::expected<int, DummyError> exp = anywho::make_error(std::move(ret), output);
  REQUIRE(!exp.has_value());
}


TEST_CASE("test optional<Error> factory with function, truth case", "[error_factories]")
{
  // Somehow template argument deduction does not work here, so we need to give it by hand.
  std::expected<int, DummyError> exp = anywho::make_error<int, DummyError>([]() {
    int output = 0;
    auto ret = positiveOnlySquareWithOptional(3, output);
    return std::make_tuple(ret, output);
  });
  REQUIRE(exp.has_value());
  REQUIRE(exp.value() == 9);
}

TEST_CASE("test optional<Error> factory with function, error case", "[error_factories]")
{
  // Somehow template argument deduction does not work here, so we need to give it by hand.
  std::expected<int, DummyError> exp = anywho::make_error<int, DummyError>([]() {
    int output = 0;
    auto ret = positiveOnlySquareWithOptional(-3, output);
    return std::make_tuple(ret, output);
  });
  REQUIRE(!exp.has_value());
}
