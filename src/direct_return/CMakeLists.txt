add_executable(example example.cpp)

target_link_libraries(
  example
  PRIVATE anywho::anywho_options
          anywho::anywho_warnings)

target_link_system_libraries(
  example
  PRIVATE
          fmt::fmt
          spdlog::spdlog
          )

target_include_directories(example PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
