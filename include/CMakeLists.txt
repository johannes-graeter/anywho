add_library(anywho_core INTERFACE)
add_library (anywho::core ALIAS anywho_core)

target_include_directories(anywho_core INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                           $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)
target_link_libraries(anywho_core INTERFACE anywho_options anywho_warnings)

target_compile_features(anywho_core INTERFACE cxx_std_23)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(anywho_core INTERFACE error_ STATIC_DEFINE)
endif()

file(GLOB anywhoSRC
    "*.hpp"
)

install(TARGETS anywho_core EXPORT AnywhoTargets)
install(FILES
  ${anywhoSRC}
  DESTINATION include/anywho/)