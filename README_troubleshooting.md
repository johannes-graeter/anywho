# Troubleshooting
* I want to use the error factories for exception based error handling:
  A bug in libc++ (https://github.com/llvm/llvm-project/issues/52771) causes alloc-dealloc mismatch when executing tests with adress sanitizer. Set the environment variable `ASAN_OPTIONS=alloc_dealloc_mismatch=0` on unix or `set ASAN_OPTIONS=alloc_dealloc_mismatch=0` on windows