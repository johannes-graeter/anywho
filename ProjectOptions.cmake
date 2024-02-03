include(cmake/SystemLink.cmake)
include(cmake/LibFuzzer.cmake)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)


macro(anywho_supports_sanitizers)
  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND NOT WIN32)
    set(SUPPORTS_UBSAN ON)
  else()
    set(SUPPORTS_UBSAN OFF)
  endif()

  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND WIN32)
    set(SUPPORTS_ASAN OFF)
  else()
    set(SUPPORTS_ASAN ON)
  endif()
endmacro()

macro(anywho_setup_options)
  option(anywho_ENABLE_HARDENING "Enable hardening" ON)
  option(anywho_ENABLE_COVERAGE "Enable coverage reporting" OFF)
  cmake_dependent_option(
    anywho_ENABLE_GLOBAL_HARDENING
    "Attempt to push hardening options to built dependencies"
    ON
    anywho_ENABLE_HARDENING
    OFF)

  anywho_supports_sanitizers()

  if(NOT PROJECT_IS_TOP_LEVEL OR anywho_PACKAGING_MAINTAINER_MODE)
    option(anywho_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(anywho_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(anywho_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(anywho_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(anywho_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(anywho_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(anywho_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(anywho_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(anywho_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(anywho_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(anywho_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(anywho_ENABLE_PCH "Enable precompiled headers" OFF)
    option(anywho_ENABLE_CACHE "Enable ccache" OFF)
  else()
    option(anywho_ENABLE_IPO "Enable IPO/LTO" ON)
    option(anywho_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(anywho_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(anywho_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" ${SUPPORTS_ASAN})
    option(anywho_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(anywho_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" ${SUPPORTS_UBSAN})
    option(anywho_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(anywho_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(anywho_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(anywho_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(anywho_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(anywho_ENABLE_PCH "Enable precompiled headers" OFF)
    option(anywho_ENABLE_CACHE "Enable ccache" ON)
  endif()

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      anywho_ENABLE_IPO
      anywho_WARNINGS_AS_ERRORS
      anywho_ENABLE_USER_LINKER
      anywho_ENABLE_SANITIZER_ADDRESS
      anywho_ENABLE_SANITIZER_LEAK
      anywho_ENABLE_SANITIZER_UNDEFINED
      anywho_ENABLE_SANITIZER_THREAD
      anywho_ENABLE_SANITIZER_MEMORY
      anywho_ENABLE_UNITY_BUILD
      anywho_ENABLE_CLANG_TIDY
      anywho_ENABLE_CPPCHECK
      anywho_ENABLE_COVERAGE
      anywho_ENABLE_PCH
      anywho_ENABLE_CACHE)
  endif()

  anywho_check_libfuzzer_support(LIBFUZZER_SUPPORTED)
  # if(LIBFUZZER_SUPPORTED AND (anywho_ENABLE_SANITIZER_ADDRESS OR anywho_ENABLE_SANITIZER_THREAD OR anywho_ENABLE_SANITIZER_UNDEFINED))
  #   set(DEFAULT_FUZZER ON)
  # else()
    set(DEFAULT_FUZZER OFF)
  # endif()

  option(anywho_BUILD_FUZZ_TESTS "Enable fuzz testing executable" ${DEFAULT_FUZZER})

endmacro()

macro(anywho_global_options)
  if(anywho_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    anywho_enable_ipo()
  endif()

  anywho_supports_sanitizers()

  if(anywho_ENABLE_HARDENING AND anywho_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR anywho_ENABLE_SANITIZER_UNDEFINED
       OR anywho_ENABLE_SANITIZER_ADDRESS
       OR anywho_ENABLE_SANITIZER_THREAD
       OR anywho_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    message("${anywho_ENABLE_HARDENING} ${ENABLE_UBSAN_MINIMAL_RUNTIME} ${anywho_ENABLE_SANITIZER_UNDEFINED}")
    anywho_enable_hardening(anywho_options ON ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()
endmacro()

macro(anywho_local_options)
  if(PROJECT_IS_TOP_LEVEL)
    include(cmake/StandardProjectSettings.cmake)
  endif()

  add_library(anywho_warnings INTERFACE)
  add_library(anywho_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  anywho_set_project_warnings(
    anywho_warnings
    ${anywho_WARNINGS_AS_ERRORS}
    ""
    ""
    ""
    "")

  if(anywho_ENABLE_USER_LINKER)
    include(cmake/Linker.cmake)
    configure_linker(anywho_options)
  endif()

  include(cmake/Sanitizers.cmake)
  anywho_enable_sanitizers(
    anywho_options
    ${anywho_ENABLE_SANITIZER_ADDRESS}
    ${anywho_ENABLE_SANITIZER_LEAK}
    ${anywho_ENABLE_SANITIZER_UNDEFINED}
    ${anywho_ENABLE_SANITIZER_THREAD}
    ${anywho_ENABLE_SANITIZER_MEMORY})

  set_target_properties(anywho_options PROPERTIES UNITY_BUILD ${anywho_ENABLE_UNITY_BUILD})

  if(anywho_ENABLE_PCH)
    target_precompile_headers(
      anywho_options
      INTERFACE
      <vector>
      <string>
      <utility>)
  endif()

  if(anywho_ENABLE_CACHE)
    include(cmake/Cache.cmake)
    anywho_enable_cache()
  endif()

  include(cmake/StaticAnalyzers.cmake)
  if(anywho_ENABLE_CLANG_TIDY)
    anywho_enable_clang_tidy(anywho_options ${anywho_WARNINGS_AS_ERRORS})
  endif()

  if(anywho_ENABLE_CPPCHECK)
    anywho_enable_cppcheck(${anywho_WARNINGS_AS_ERRORS} "" # override cppcheck options
    )
  endif()

  if(anywho_ENABLE_COVERAGE)
    include(cmake/Tests.cmake)
    anywho_enable_coverage(anywho_options)
  endif()

  if(anywho_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(anywho_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()

  if(anywho_ENABLE_HARDENING AND NOT anywho_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR anywho_ENABLE_SANITIZER_UNDEFINED
       OR anywho_ENABLE_SANITIZER_ADDRESS
       OR anywho_ENABLE_SANITIZER_THREAD
       OR anywho_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    anywho_enable_hardening(anywho_options OFF ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()

endmacro()
