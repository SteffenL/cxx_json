#include "langnes_json/detail/utf8.hpp"
#include "langnes_json/test_driver.hpp"

TEST_CASE("to_utf8_char") {
    using namespace langnes::json;
    using namespace langnes::json::detail;
    SECTION("1-4 byte characters") {
        REQUIRE(to_utf8_char(0x24) == "\x24");
        REQUIRE(to_utf8_char(0xa3) == "\xc2\xa3");
        REQUIRE(to_utf8_char(0x418) == "\xd0\x98");
        REQUIRE(to_utf8_char(0x939) == "\xe0\xa4\xb9");
        REQUIRE(to_utf8_char(0x20ac) == "\xe2\x82\xac");
        REQUIRE(to_utf8_char(0xd55c) == "\xed\x95\x9c");
        REQUIRE(to_utf8_char(0x10348) == "\xf0\x90\x8d\x88");
        REQUIRE(to_utf8_char(0x1096b3) == "\xf4\x89\x9a\xb3");
    }
    SECTION("Should throw when input is out of range") {
        bool errored{};
        try {
            to_utf8_char(0x110000);
        } catch (const out_of_range&) {
            errored = true;
        }
        REQUIRE(errored);
    }
}
