find_package(fmt)

add_library(error INTERFACE)
add_library(anywho::error ALIAS error)

target_include_directories(error INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                           $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)
target_link_libraries(error INTERFACE anywho_options anywho_warnings fmt::fmt)

target_compile_features(error INTERFACE cxx_std_23)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(error INTERFACE error_ STATIC_DEFINE)
endif()