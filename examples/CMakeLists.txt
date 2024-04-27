add_executable(langnes_json_example_c main.c)
add_executable(langnes_json_example_cxx main.cpp)
target_link_libraries(langnes_json_example_c
    PRIVATE langnes_json_c_shared $<BUILD_INTERFACE:langnes_json_private>)
target_link_libraries(langnes_json_example_cxx
    PRIVATE langnes_json_cxx_headers $<BUILD_INTERFACE:langnes_json_private>)
