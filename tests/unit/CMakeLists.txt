add_executable(langnes_json_unit_tests
    json_tests.cpp
)
target_link_libraries(langnes_json_unit_tests PRIVATE langnes::json Catch2::Catch2WithMain)
catch_discover_tests(langnes_json_unit_tests)
