# anywho

[![ci](https://github.com/johannes-graeter/anywho/actions/workflows/ci.yml/badge.svg)](https://github.com/johannes-graeter/anywho/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/johannes-graeter/anywho/branch/main/graph/badge.svg)](https://codecov.io/gh/johannes-graeter/anywho)
[![CodeQL](https://github.com/johannes-graeter/anywho/actions/workflows/codeql-analysis.yml/badge.svg)](https://github.com/johannes-graeter/anywho/actions/workflows/codeql-analysis.yml)

## About anywho
Error management for c++23 just like rust anyhow.

## Docs
[README with code documentation](https://johannes-graeter.github.io/anywho/index.html)

## Why yet another error handling library
Having dug into rust, I found error management with the anyhow library (https://docs.rs/anyhow/latest/anyhow/) very pleasing and the best I experienced so far.
With c++23's std::expected we are headed in a good direction but we need some syntactic sugar around it.
This repo adds an error type and convenience functions to make it easier working with std::expected.
This project is meant to be for std::expected, what Rust's anyhow is to Result<>.

## Design goals
* no dependencies outside of stdlibc++
* zero overhead compared to using std::expected
* errors are redirected in 90 percent of the cases. This must hence be very lean (max. 1 line of code to add information to the error)
* transition from other error handling strategies to std::expected must be smooth.
* cutting edge c++ features
More details in [here](design.md)

## Short introduction
To add this to your project
```cpp
#include <anywho/anywho.hpp>

// In order to compile this you need to set ASAN_OPTIONS=alloc_dealloc_mismatch=0
// Caused by a bug in libc++-17
#include <anywho/extra.hpp>
```

Use it like

```cpp
// With a function defined like
std::expected<std::string, anywho::GenericError> myFunc(int input);

// The following will directly return std::unexpected if myFunc has error.
std::expected<std::string, anywho::GenericError> execute(int input){
  std::string x = ANYWHO(myFunc(input)); 	
  // ... do something with x 
  return x; 
}

// Also you can give context to the unexpected path
std::expected<std::string, anywho::GenericError> execute_with_context(int input){
  std::string x = ANYWHO(anywho::with_context(myFunc(input), {.message="you may not pass!", .file=__FILE__, .line=__LINE__})); 
  // ... do something with x 
  return x; 
}
```

Also you can (and should!) define your own error types for more finegrained error handling. It must bind to the contract defined by `concepts::Error` (concepts.hpp)
```cpp
struct IOError final {
  [[nodiscard]] std::string format() const
  {
    std::string out = message();
    for (const auto &el : contexts_) { out += "::" + el.format(); }  
    return out;
  }  
  void consume_context(anywho::Context &&context) { contexts_.emplace_back(std::move(context)); }
  [[nodiscard]] virtual constexpr std::string message() const { return "IOError"; }
  [[nodiscard]] size_t id() const { return 1;}

protected:
  std::vector<Context> contexts_{};
};
```
Or easier you can inherit from the base types provided
```cpp
class IOError2 final : public GenericError
{
public:
  [[nodiscard]] constexpr std::string message() const override
  {
    return "Another IO error happened!";
  }
};
```

```cpp
// For errors that should be able to be placed on the stack we provide FixedSizeError
class IOErrorOnStack final : public FixedSizeError<1024>
{
public:
  [[nodiscard]] constexpr std::string message() const override
  {
    return "I am fixed size!";
  }
};
```

Also we provide functionality to use the errors with std::optional in case you need to use it with c++17 code, f.e. in a mixed stack
```cpp
// With a function defined like
std::optional<anywho::GenericError> myFunc(int input);

// The following will directly return anywho::NoError if myFunc has error.
// ErrorState is just naming sugar for std::optional<anywho::GenericError> to express intent better.
anywho::ErrorState<anywho::GenericError> execute(int input){
  std::string x = ANYWHO_OPT(myFunc(input)); 	
  // ... do something with x 
  return x; 
}

// Also you can give context to the unexpected path
anywho::ErrorState<anywho::GenericError> execute_with_context(int input){
  std::string x = ANYWHO_OPT(anywho::with_context(myFunc(input), {.message="you may not pass!", .file=__FILE__, .line=__LINE__})); 
  // ... do something with x 
  return x; 
}
```

To make the transition from your existing error handling mechanism easier we provide error factories for
* functions that have a boolean indicate success:
```cpp
int output = 0;
bool ret = positiveOnlySquare(3, output);
std::expected<int, anywho::GenericError> exp = anywho::make_error(ret, output, anywho::GenericError{});
```
or the lambda variant
```cpp
std::expected<int, anywho::GenericError> exp = anywho::make_error<int, anywho::GenericError>(
  []() {
    int output = 0;
    bool success = myFunc(3, output);
    return std::make_tuple(success, output);
  },
anywho::GenericError{});
```
* std::error_code
```cpp
int output = 0;
std::error_code error_code = myFuncWithErrorCode(3, output);
std::expected<int, anywho::ErrorFromCode> exp = anywho::make_error(error_code, output);
```
or the lambda variant
* exceptions (available through `#include <anywho/extra.hpp>`)
```cpp
std::expected<int, anywho::ErrorFromException> exp = anywho::make_error_from_throwable<int, std::runtime_error>([var]() { return myOtherFunc(var); });
```
Note that caused by a bug in libc++ (as of 2024/02/07) you must set ASAN_OPTIONS=alloc_dealloc_mismatch=0 when using the Address sanitizer (see .devcontainer/Dockerfile)


## Current support
* Ubuntu 22.04: Clang-18
* Ubuntu 22.04: gcc-13
* Ubuntu 20.04: Clang-18
* Ubuntu 20.04: gcc-13

Note: When compiling with clang18 and libstdc++ there is a problem with std::expected, see troubleshooting.


## Build docs
We have experimental ReadTheDocs style docs to build (Doxygen >=1.9.8 is recommended and must be preinstalled)
```bash
pushd docs
pip3 install -r requirements.txt
doxygen Doxyfile.in
make html
popd
```
open `docs/_build/html/index.html` in your browser.

## More Details

 * [Dependency Setup](README_dependencies.md)
 * [Building Details](README_building.md)
 * [Troubleshooting](README_troubleshooting.md)
 * [Docker](README_docker.md)
