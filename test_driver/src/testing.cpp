#include "langnes_json/testing.hpp"

#include <deque>
#include <iomanip>
#include <iostream>
#include <string>

namespace langnes {
namespace json {

std::map<std::string, test_reg>& auto_test_reg::tests() {
    static std::map<std::string, test_reg> instance;
    return instance;
}

} // namespace json
} // namespace langnes

struct failure_exit_codes {
    enum type {
        success = 0,
        failure = 1,
        exception_thrown = 2,
        no_tests_found = 3,
        test_not_found = 4
    };
};

int cmd_help() {
    std::cout << "Usage: program [--[help|list]|test_name].\n";
    return 0;
}

int cmd_list() {
    using namespace langnes::json;
    auto& tests{auto_test_reg::tests()};
    if (tests.empty()) {
        std::cerr << "No tests found.\n";
        return failure_exit_codes::no_tests_found;
    }
    for (const auto& test : tests) {
        std::cout << test.second.name() << '\n';
    }
    return failure_exit_codes::success;
}

int cmd_run_test(const std::string& test_name) {
    using namespace langnes::json;
    auto& tests{auto_test_reg::tests()};
    auto found{tests.find(test_name)};
    if (found == tests.end()) {
        std::cerr << "Test not found: " << test_name << '\n';
        return failure_exit_codes::test_not_found;
    }
    auto exit_code{failure_exit_codes::failure};
    const auto& test{found->second};
    try {
        test.invoke();
        exit_code = failure_exit_codes::success;
    } catch (const test_failure& e) {
        const auto& info{e.info()};
        std::cerr << "Test \"" << test.name() << "\" failed [" << info.condition
                  << "] at " << info.file << ":" << info.line << ".\n";
        exit_code = failure_exit_codes::failure;
    } catch (const std::exception& e) {
        std::cerr << "Test \"" << test.name()
                  << "\" threw exception with message \"" << e.what()
                  << "\".\n";
        exit_code = failure_exit_codes::exception_thrown;
    }
    return exit_code;
}

int cmd_run_all_tests() {
    using namespace langnes::json;
    auto& tests{auto_test_reg::tests()};
    if (tests.empty()) {
        return failure_exit_codes::no_tests_found;
    }
    size_t name_width{};
    for (const auto& it : tests) {
        const auto& test{it.second};
        const auto name_size{test.name().size()};
        if (name_size > name_width) {
            name_width = name_size;
        }
    }
    size_t success_count{};
    for (const auto& it : tests) {
        const auto& test{it.second};
        std::cout << test.name() << std::setfill('.')
                  << std::setw(
                         static_cast<int>(name_width - test.name().size()))
                  << "" << std::setw(0) << ": ";
        try {
            test.invoke();
            ++success_count;
            std::cout << "OK\n";
        } catch (const test_failure& e) {
            const auto& info{e.info()};
            std::cout << "FAIL\n";
            std::cerr << "Test \"" << test.name() << "\" failed ["
                      << info.condition << "] at " << info.file << ":"
                      << info.line << ".\n";
        } catch (const std::exception& e) {
            std::cout << "FAIL\n";
            std::cerr << "Test \"" << test.name()
                      << "\" threw exception with message \"" << e.what()
                      << "\".\n";
        }
    }
    if (success_count == tests.size()) {
        return failure_exit_codes::success;
    }
    return failure_exit_codes::failure;
}

int main(int argc, const char* argv[]) {
    try {
        const std::deque<std::string> args{argv, argv + argc};
        if (args.size() > 1) {
            const std::string& arg{args.at(1)};
            if (arg == "--help") {
                return cmd_help();
            }
            if (arg == "--list") {
                return cmd_list();
            }
            return cmd_run_test(arg);
        }
        return cmd_run_all_tests();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return failure_exit_codes::exception_thrown;
    } catch (...) {
        std::cerr << "Unknown error.\n";
        return failure_exit_codes::exception_thrown;
    }
}
