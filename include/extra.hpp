#pragma once

// In order to compile this you need to set ASAN_OPTIONS=alloc_dealloc_mismatch=0
// Caused by a bug in libc++-17
#include "error_from_exception.hpp"