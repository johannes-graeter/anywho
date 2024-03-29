cmake_minimum_required(VERSION 3.27)

# This template attempts to be "fetch_content"-able
# so that it works well with tools like CPM or other
# manual dependency management

# Only set the cxx_standard if it is not set by someone else
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# strongly encouraged to enable this globally to avoid conflicts between
# -Wpedantic being enabled and -std=c++23 and -std=gnu++23 for example
# when compiling with PCH enabled
set(CMAKE_CXX_EXTENSIONS OFF)

# Set the project name and language
project(
  anywho
  VERSION 0.0.7
  DESCRIPTION ""
  HOMEPAGE_URL "https://github.com/johannes-graeter/anywho"
  LANGUAGES CXX C)

include(cmake/PreventInSourceBuilds.cmake)
include(ProjectOptions.cmake)

# TODO (johannes-graeter) remove this once libc++ is shipped with std::expected
message(WARNING ${CMAKE_CXX_COMPILER_ID})
if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*")
  message(WARNING "CLANG with c++")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

anywho_setup_options()

anywho_global_options()
include(Dependencies.cmake)
anywho_setup_dependencies()

anywho_local_options()

# don't know if this should be set globally from here or not...
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

set(GIT_SHA
    "Unknown"
    CACHE STRING "SHA this build was generated from")
string(
  SUBSTRING "${GIT_SHA}"
            0
            8
            GIT_SHORT_SHA)

target_compile_features(anywho_options INTERFACE cxx_std_${CMAKE_CXX_STANDARD})

add_library(anywho::anywho_options ALIAS anywho_options)
add_library(anywho::anywho_warnings ALIAS anywho_warnings)

#add_library(anywho::anywho_options INTERFACE IMPORTED)
#add_library(anywho::anywho_warnings INTERFACE IMPORTED)

# configure files based on CMake configuration options
add_subdirectory(configured_files)

# Adding the include:
add_subdirectory(include)
# # Adding the src:
# add_subdirectory(src)

# Don't even look at tests if we're not top level
if(NOT PROJECT_IS_TOP_LEVEL)
  return()
endif()

# Adding the tests:
include(CTest)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()


if(anywho_BUILD_FUZZ_TESTS)
  message(AUTHOR_WARNING "Building Fuzz Tests, using fuzzing sanitizer https://www.llvm.org/docs/LibFuzzer.html")
  if (NOT anywho_ENABLE_ADDRESS_SANITIZER AND NOT anywho_ENABLE_THREAD_SANITIZER)
    message(WARNING "You need asan or tsan enabled for meaningful fuzz testing")
  endif()
  add_subdirectory(fuzz_test)

endif()

# If MSVC is being used, and ASAN is enabled, we need to set the debugger environment
# so that it behaves well with MSVC's debugger, and we can run the target from visual studio
if(MSVC)
  get_all_installable_targets(all_targets)
  message("all_targets=${all_targets}")
  set_target_properties(${all_targets} PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=$(VC_ExecutablePath_x64);%PATH%")
endif()

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT intro)

if(CMAKE_SKIP_INSTALL_RULES)
  return()
endif()

include(cmake/PackageProject.cmake)

# Add other targets that you want installed here, by default we just package the one executable
# we know we want to ship
anywho_package_project(
  TARGETS
  anywho_warnings
  anywho_options
  anywho_core
  # PUBLIC_INCLUDES
  # "./include"
  
)

execute_process(COMMAND lsb_release -cs
                OUTPUT_VARIABLE UBUNTU_VERSION_CODENAME
                OUTPUT_STRIP_TRAILING_WHITESPACE)
# Set package name including the Ubuntu codename
# Experience shows that explicit package naming can help make it easier to sort
# out potential ABI related issues before they start, while helping you
# track a build to a specific GIT SHA
set(CPACK_PACKAGE_FILE_NAME
    "${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-${GIT_SHORT_SHA}-${CMAKE_SYSTEM_NAME}-${UBUNTU_VERSION_CODENAME}-${CMAKE_BUILD_TYPE}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}"
)
set(CPACK_PACKAGE_NAME "anywho")
set(CPACK_PACKAGE_VERSION ${CMAKE_PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "std::expected on steroids")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Johannes Graeter")

include(CPack)
