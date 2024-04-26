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
        COMPONENT langnes_json_library_headers)

    install(TARGETS langnes_json_cxx_headers EXPORT langnes_json_cxx_targets)

    install(EXPORT langnes_json_cxx_targets
        DESTINATION "lib/cmake/langnes_json_cxx"
        NAMESPACE langnes::json::cxx::
        FILE langnes_json_cxx-targets.cmake
        COMPONENT langnes_json_cxx_library_cmake)

    export(EXPORT langnes_json_cxx_targets FILE "${CMAKE_CURRENT_BINARY_DIR}/langnes_json_cxx-targets.cmake")

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/langnes_json_cxx-config-version.cmake"
        VERSION "${LANGNES_JSON_VERSION_COMPATIBILITY}"
        COMPATIBILITY SameMinorVersion
        ARCH_INDEPENDENT)

    configure_package_config_file(
        langnes_json_cxx-config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/langnes_json_cxx-config.cmake"
        INSTALL_DESTINATION "lib/cmake/langnes_json_cxx"
        NO_SET_AND_CHECK_MACRO
        NO_CHECK_REQUIRED_COMPONENTS_MACRO)

    install(
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/langnes_json_cxx-config.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/langnes_json_cxx-config-version.cmake"
        DESTINATION "lib/cmake/langnes_json_cxx"
        COMPONENT langnes_json_cxx_library_cmake)
endif()
