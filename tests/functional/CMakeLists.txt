add_executable(langnes_json_functional_tests
    c_api_tests.cpp
    cxx_api_tests.cpp
)
target_link_libraries(langnes_json_functional_tests PRIVATE langnes::json_static Catch2::Catch2WithMain)
catch_discover_tests(langnes_json_functional_tests)
