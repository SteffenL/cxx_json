add_subdirectory(cxx)
add_subdirectory(c)

#if(LANGNES_JSON_INSTALL)
#    configure_package_config_file(
#        langnes_json-config.cmake.in
#        "${CMAKE_CURRENT_BINARY_DIR}/langnes_json-config.cmake"
#        INSTALL_DESTINATION "lib/cmake/langnes_json"
#        NO_SET_AND_CHECK_MACRO
#        NO_CHECK_REQUIRED_COMPONENTS_MACRO)
#
#    install(
#        FILES
#            "${CMAKE_CURRENT_BINARY_DIR}/langnes_json-config.cmake"
#        DESTINATION "lib/cmake/langnes_json"
#        COMPONENT langnes_json_library_cmake)
#endif()
