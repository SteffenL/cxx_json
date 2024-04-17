add_test(NAME langnes_json_example_c_test COMMAND $<TARGET_FILE:langnes_json_example_c>)
add_test(NAME langnes_json_example_cxx_test COMMAND $<TARGET_FILE:langnes_json_example_cxx>)

# Tests need to find the shared library at runtime on Windows.
if(WIN32)
    set(LANGNES_JSON_SHARED_LIBRARY_DIR $<SHELL_PATH:$<TARGET_FILE_DIR:langnes_json_shared>>)
    set_property(TEST langnes_json_example_c_test APPEND PROPERTY
        ENVIRONMENT_MODIFICATION
        PATH=path_list_prepend:${LANGNES_JSON_SHARED_LIBRARY_DIR})
endif()
