#pragma once

#include <exception>
#include <functional>
#include <map>
#include <string>

namespace langnes {
namespace json {

struct failure_info {
    const char* condition;
    const char* file;
    int line;
};

struct test_failure : public std::exception {
    explicit test_failure(const failure_info& info) noexcept : m_info{info} {}
    const char* what() const noexcept override { return "test failure"; }
    const failure_info& info() const noexcept { return m_info; }

private:
    failure_info m_info;
};

class test_reg {
public:
    test_reg() = default;

    test_reg(const char* name, std::function<void()> fn) noexcept
        : m_name{name},
          m_fn{std::move(fn)} {}

    const std::string& name() const noexcept { return m_name; }
    void invoke() const { m_fn(); }

private:
    std::string m_name;
    std::function<void()> m_fn;
};

struct auto_test_reg {
    explicit auto_test_reg(test_reg reg) noexcept {
        tests()[reg.name()] = std::move(reg);
    }

    static std::map<std::string, test_reg>& tests();
};

// NOLINTBEGIN(cppcoreguidelines-macro-usage)

#define MAKE_TEST_CASE_NAME2(name, counter) name##counter
#define MAKE_TEST_CASE_NAME(name, counter) MAKE_TEST_CASE_NAME2(name, counter)

#define TEST_CASE_INTERNAL(name, counter)                                      \
    /* NOLINTNEXTLINE(misc-use-anonymous-namespace) */                         \
    static void MAKE_TEST_CASE_NAME(langnes_json_test_driver_case_,            \
                                    counter)();                                \
    namespace {                                                                \
    const langnes::json::auto_test_reg MAKE_TEST_CASE_NAME(                    \
        langnes_json_test_driver_case_reg_, counter){                          \
        {name, MAKE_TEST_CASE_NAME(langnes_json_test_driver_case_, counter)}}; \
    }                                                                          \
    /* NOLINTNEXTLINE(misc-use-anonymous-namespace) */                         \
    static void MAKE_TEST_CASE_NAME(langnes_json_test_driver_case_, counter)()

#define TEST_CASE(name) TEST_CASE_INTERNAL(name, __LINE__)

#define REQUIRE(condition)                                                     \
    if (!static_cast<bool>(condition)) {                                       \
        throw langnes::json::test_failure{                                     \
            langnes::json::failure_info{#condition, __FILE__, __LINE__}};      \
    }

#define REQUIRE_FALSE(condition)                                               \
    if (static_cast<bool>(condition)) {                                        \
        throw langnes::json::test_failure{                                     \
            langnes::json::failure_info{#condition, __FILE__, __LINE__}};      \
    }

#define SECTION(name)

// NOLINTEND(cppcoreguidelines-macro-usage)

} // namespace json
} // namespace langnes
