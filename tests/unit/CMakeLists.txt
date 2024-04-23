add_executable(langnes_json_unit_tests
    utf8_tests.cpp
)
target_link_libraries(langnes_json_unit_tests PRIVATE langnes::json langnes_json_test_driver)
langnes_json_discover_tests(langnes_json_unit_tests)
