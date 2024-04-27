add_subdirectory(cxx)
add_subdirectory(c)

if(LANGNES_JSON_INSTALL)
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/langnes_json-config-version.cmake"
        VERSION "${LANGNES_JSON_VERSION_COMPATIBILITY}"
        COMPATIBILITY SameMinorVersion)

    configure_package_config_file(
        langnes_json-config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/langnes_json-config.cmake"
        INSTALL_DESTINATION "lib/cmake/langnes_json"
        NO_SET_AND_CHECK_MACRO
        NO_CHECK_REQUIRED_COMPONENTS_MACRO)

    install(
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/langnes_json-config.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/langnes_json-config-version.cmake"
        DESTINATION "lib/cmake/langnes_json"
        COMPONENT langnes_json_cmake)
endif()
