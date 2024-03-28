add_executable(example main.cpp)
target_link_libraries(example PRIVATE langnes::json)
add_test(NAME example COMMAND example)
