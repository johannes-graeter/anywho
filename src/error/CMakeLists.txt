include(GenerateExportHeader)


add_library(error INTERFACE concepts.hpp context.hpp fixed_string.hpp)


add_library(anywho::error ALIAS error)

target_link_libraries(error INTERFACE anywho_options anywho_warnings)

target_include_directories(error ${WARNING_GUARD} INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                                                         $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)

target_compile_features(error INTERFACE cxx_std_23)

set_target_properties(
  error
  PROPERTIES VERSION ${PROJECT_VERSION}
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN YES)

generate_export_header(error EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/include/anywho/error_export.hpp)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(error INTERFACE error_STATIC_DEFINE)
endif()
