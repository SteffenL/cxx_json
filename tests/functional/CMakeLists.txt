add_executable(langnes_json_functional_tests
    c_api_tests.cpp
    cxx_api_tests.cpp
)
target_link_libraries(langnes_json_functional_tests PRIVATE langnes::json_static langnes_json_test_driver langnes_json_private)
langnes_json_discover_tests(langnes_json_functional_tests)
