#include "langnes_json/testing.hpp"

#include <deque>
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

int main(int argc, const char* argv[]) {
    using namespace langnes::json;
    const std::deque<std::string> args{argv, argv + argc};
    if (args.size() < 2) {
        std::cerr << "Usage: program [--list|test_name].\n";
        return 1;
    }
    auto& tests{auto_test_reg::tests()};
    const std::string& arg{args.at(1)};
    if (arg == "--list") {
        if (tests.empty()) {
            std::cerr << "No tests found.\n";
            return failure_exit_codes::no_tests_found;
        }
        for (const auto& test : tests) {
            std::cout << test.second.name << '\n';
        }
        return failure_exit_codes::success;
    }
    const std::string& test_name{arg};
    auto found{tests.find(test_name)};
    if (found == tests.end()) {
        std::cerr << "Test not found: " << test_name << '\n';
        return failure_exit_codes::test_not_found;
    }
    const auto& test{found->second};
    try {
        test.fn();
        return failure_exit_codes::success;
    } catch (const test_failure& e) {
        const auto& info{e.info()};
        std::cerr << "Test \"" << test.name << "\" failed [" << info.condition
                  << "] at " << info.file << ":" << info.line << ".\n";
        return failure_exit_codes::failure;
    } catch (const std::exception& e) {
        std::cerr << "Test \"" << test.name
                  << "\" threw exception with message \"" << e.what()
                  << "\".\n";
        return failure_exit_codes::exception_thrown;
    }
}
