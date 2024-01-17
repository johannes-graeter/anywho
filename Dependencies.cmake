include(cmake/CPM.cmake)

# Done as a function so that updates to variables like
# CMAKE_CXX_FLAGS don't propagate out to other
# targets
function(anywho_setup_dependencies)
  # For each dependency, see if it's
  # already been provided to us by a parent project
  if(NOT TARGET Catch2::Catch2WithMain)
  # Only set the cxx_standard if it is not set by someone else
set(CMAKE_CXX_STANDARD 23)

# TODO (johannes-graeter) remove this once libc++ is shipped with std::expected
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    cpmaddpackage("gh:catchorg/Catch2@3.3.2")
  endif()
endfunction()
