# Troubleshooting
* I want to use the error factories for exception based error handling:
  A bug in libc++ (https://github.com/llvm/llvm-project/issues/52771) causes alloc-dealloc mismatch when executing tests with adress sanitizer. Set the environment variable `ASAN_OPTIONS=alloc_dealloc_mismatch=0` on unix or `set ASAN_OPTIONS=alloc_dealloc_mismatch=0` on windows
* I want to compile with Clang<=18 and libstdc++ (gcc):
  Clang as of 28.Mai 2024 defines the __cpp_concepts == 2019xxxx this is old, the needed concepts for std::expected are implemented (see https://github.com/llvm/llvm-project/issues/62801)
  use 
  ```cpp
  #if !defined(__cpp_concepts) || __cpp_concepts < 202002L
  #define __cpp_concepts 202002L
  #endif
  #include <expected>
  ```
  to hack this. Clang-19 is supposed to fix this.
  For a more elaborate guard (the bare definition of _cpp_concepts may cause problems in bigger projects) use
  ```cpp
  #if defined(__clang__) && __cplusplus > 202002L
  #if defined(__cpp_concepts)
  #define ORIGINAL_CPP_CONCEPTS __cpp_concepts
  #endif
  #undef __cpp_concepts
  #define __cpp_concepts 202002L
  #endif

  #include <anywho/anywho.hpp>
  #include <expected>

  #if defined(__clang__)
  #if defined(ORIGINAL_CPP_CONCEPTS)
  #undef __cpp_concepts
  #define __cpp_concepts ORIGINAL_CPP_CONCEPTS
  #undef ORIGINAL_CPP_CONCEPTS
  #elif defined(__cpp_concepts)
  #undef __cpp_concepts
  #endif
  #endif
  ```