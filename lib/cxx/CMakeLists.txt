add_library(langnes_json_cxx_headers INTERFACE)
add_library(langnes::json::cxx ALIAS langnes_json_cxx_headers)

target_include_directories(
    langnes_json_cxx_headers
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/lib/cxx/include>"
        "$<INSTALL_INTERFACE:include>")
set_target_properties(langnes_json_cxx_headers PROPERTIES
    EXPORT_NAME headers)

target_compile_features(langnes_json_cxx_headers INTERFACE cxx_std_11)

if(LANGNES_JSON_INSTALL)
    install(DIRECTORY "include/langnes_json/cxx"
        DESTINATION "include/langnes_json"
        COMPONENT langnes_json_headers)

    install(TARGETS langnes_json_cxx_headers EXPORT langnes_json_cxx_header_targets)

    install(EXPORT langnes_json_cxx_header_targets
        DESTINATION "lib/cmake/langnes_json/cxx"
        NAMESPACE langnes::json::cxx::
        FILE header_targets.cmake
        COMPONENT langnes_json_cxx_cmake_cross_platform)
endif()
