#include "langnes_json/testing.hpp"

#include <langnes_json/json.h>

#include <cstring>

// NOLINTBEGIN(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)
// NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
// NOLINTBEGIN(hicpp-use-nullptr,modernize-use-nullptr)

TEST_CASE("Create empty JSON object and populate it") {
    langnes_json_value_t* json_object = langnes_json_value_object_new_s();

    // Make sure no items are added yet.
    REQUIRE(langnes_json_value_object_get_members_length_s(json_object) == 0);

    // Create new items.
    langnes_json_value_t* pushed_item0 =
        langnes_json_value_string_new_s("hello");
    langnes_json_value_t* pushed_item1 =
        langnes_json_value_number_new_s(3.14159);

    // Add items.
    langnes_json_value_object_set_value(json_object, "item0", pushed_item0);
    langnes_json_value_object_set_value(json_object, "item1", pushed_item1);

    // Make sure that we added the exact amount of items.
    REQUIRE(langnes_json_value_object_get_members_length_s(json_object) == 2);

    // Retrieve items that were added before.
    langnes_json_value_t* got_item0 =
        langnes_json_value_object_get_value_s(json_object, "item0");
    langnes_json_value_t* got_item1 =
        langnes_json_value_object_get_value_s(json_object, "item1");

    // The pointers of the retrieved items are not necessarily the same as the
    // pointers during insertion but we can't reliably check.
    REQUIRE(got_item0 != NULL);
    REQUIRE(got_item1 != NULL);
    REQUIRE(got_item0 != got_item1);

    // Retrieving the same items again will however yield the same pointers as
    // last time.
    REQUIRE(langnes_json_value_object_get_value_s(json_object, "item0") ==
            got_item0);
    REQUIRE(langnes_json_value_object_get_value_s(json_object, "item1") ==
            got_item1);

    // Remove all items.
    langnes_json_value_object_clear(json_object);
    REQUIRE(langnes_json_value_object_get_members_length_s(json_object) == 0);
}

TEST_CASE("Create JSON object with members") {
    langnes_json_object_member_t members[] = {
        {"item0", langnes_json_value_number_new_s(1)},
        {"item1", langnes_json_value_number_new_s(2)}};

    langnes_json_value_t* json_object =
        langnes_json_value_object_new_with_members_s(
            members, sizeof(members) / sizeof(members[0]));

    // Make sure that we added the exact amount of items.
    REQUIRE(langnes_json_value_object_get_members_length_s(json_object) == 2);
}

TEST_CASE("Change type to JSON object with members") {
    langnes_json_value_t* json_value = langnes_json_value_null_new_s();

    langnes_json_object_member_t members[] = {
        {"item0", langnes_json_value_number_new_s(1)},
        {"item1", langnes_json_value_number_new_s(2)}};

    langnes_json_value_set_object_with_members(
        json_value, members, sizeof(members) / sizeof(members[0]));

    // Make sure the type was changed.
    REQUIRE(langnes_json_value_is_object_s(json_value));

    // Make sure that we added the exact amount of items.
    REQUIRE(langnes_json_value_object_get_members_length_s(json_value) == 2);
}

TEST_CASE("Clear JSON object") {
    langnes_json_value_t* json_object = langnes_json_value_object_new_s();
    langnes_json_value_object_set_value(json_object, "n",
                                        langnes_json_value_number_new_s(0));
    REQUIRE(langnes_json_value_object_get_members_length_s(json_object) == 1);
    langnes_json_value_object_clear(json_object);
    REQUIRE(langnes_json_value_object_get_members_length_s(json_object) == 0);
}

TEST_CASE("Create empty JSON array and populate it") {
    langnes_json_value_t* json_array = langnes_json_value_array_new_s();

    // Make sure no items are added yet.
    REQUIRE(langnes_json_value_array_get_length_s(json_array) == 0);

    // Create new items.
    langnes_json_value_t* pushed_item0 =
        langnes_json_value_string_new_s("hello");
    langnes_json_value_t* pushed_item1 =
        langnes_json_value_number_new_s(3.14159);

    // Add items.
    langnes_json_value_array_push(json_array, pushed_item0);
    langnes_json_value_array_push(json_array, pushed_item1);

    // Make sure that we added the exact amount of items.
    REQUIRE(langnes_json_value_array_get_length_s(json_array) == 2);

    // Retrieve items that were added before.
    langnes_json_value_t* got_item0 =
        langnes_json_value_array_get_item_s(json_array, 0);
    langnes_json_value_t* got_item1 =
        langnes_json_value_array_get_item_s(json_array, 1);

    // The pointers of the retrieved items are not necessarily the same as the
    // pointers during insertion but we can't reliably check.
    REQUIRE(got_item0 != NULL);
    REQUIRE(got_item1 != NULL);
    REQUIRE(got_item0 != got_item1);

    // Retrieving the same items again will however yield the same pointers as
    // last time.
    REQUIRE(langnes_json_value_array_get_item_s(json_array, 0) == got_item0);
    REQUIRE(langnes_json_value_array_get_item_s(json_array, 1) == got_item1);

    // Make sure the order of insertion is correct.
    REQUIRE(strcmp(langnes_json_value_get_cstring_s(got_item0), "hello") == 0);
    REQUIRE(langnes_json_value_get_number_s(got_item1) == 3.14159);

    // Remove all items.
    langnes_json_value_array_clear(json_array);
    REQUIRE(langnes_json_value_array_get_length_s(json_array) == 0);
}

TEST_CASE("Create JSON array with elements") {
    langnes_json_value_t* elements[] = {langnes_json_value_number_new_s(1),
                                        langnes_json_value_number_new_s(2)};
    langnes_json_value_t* json_array =
        langnes_json_value_array_new_with_elements_s(
            elements, sizeof(elements) / sizeof(elements[0]));

    // Make sure that we added the exact amount of items.
    REQUIRE(langnes_json_value_array_get_length_s(json_array) == 2);

    // Make sure the order of insertion is correct.
    REQUIRE(langnes_json_value_get_number_s(
                langnes_json_value_array_get_item_s(json_array, 0)) == 1);
    REQUIRE(langnes_json_value_get_number_s(
                langnes_json_value_array_get_item_s(json_array, 1)) == 2);
}

TEST_CASE("Change type to JSON array with elements") {
    langnes_json_value_t* json_value = langnes_json_value_null_new_s();

    langnes_json_value_t* elements[] = {langnes_json_value_number_new_s(1),
                                        langnes_json_value_number_new_s(2)};

    langnes_json_value_set_array_with_elements(
        json_value, elements, sizeof(elements) / sizeof(elements[0]));

    // Make sure the type was changed.
    REQUIRE(langnes_json_value_is_array_s(json_value));

    // Make sure that we added the exact amount of items.
    REQUIRE(langnes_json_value_array_get_length_s(json_value) == 2);
}

TEST_CASE("Clear JSON array") {
    langnes_json_value_t* json_array = langnes_json_value_array_new_s();
    langnes_json_value_array_push(json_array,
                                  langnes_json_value_number_new_s(0));
    REQUIRE(langnes_json_value_array_get_length_s(json_array) == 1);
    langnes_json_value_array_clear(json_array);
    REQUIRE(langnes_json_value_array_get_length_s(json_array) == 0);
}

TEST_CASE("Check types of new JSON values") {
    SECTION("New string value should return proper type and value") {
        langnes_json_value_t* json_value =
            langnes_json_value_string_new_s("hello");
        REQUIRE(langnes_json_value_is_string_s(json_value));
    }

    SECTION("New number value should return proper type and value") {
        langnes_json_value_t* json_value = langnes_json_value_number_new_s(1);
        REQUIRE(langnes_json_value_is_number_s(json_value));
    }

    SECTION("New boolean value should return proper type and value") {
        langnes_json_value_t* json_value =
            langnes_json_value_boolean_new_s(true);
        REQUIRE(langnes_json_value_is_boolean_s(json_value));
    }

    SECTION("New null value should return proper type") {
        langnes_json_value_t* json_value = langnes_json_value_null_new_s();
        REQUIRE(langnes_json_value_is_null_s(json_value));
    }

    SECTION("New object value should return proper type") {
        langnes_json_value_t* json_value = langnes_json_value_object_new_s();
        REQUIRE(langnes_json_value_is_object_s(json_value));
    }

    SECTION("New array value should return proper type") {
        langnes_json_value_t* json_value = langnes_json_value_array_new_s();
        REQUIRE(langnes_json_value_is_array_s(json_value));
    }
}

TEST_CASE("Change types of JSON values") {
    SECTION("Change type to string") {
        langnes_json_value_t* json_value = langnes_json_value_null_new_s();
        REQUIRE_FALSE(langnes_json_value_is_string_s(json_value));
        langnes_json_value_set_string(json_value, "hello");
        REQUIRE(langnes_json_value_is_string_s(json_value));
    }

    SECTION("Change type to number") {
        langnes_json_value_t* json_value = langnes_json_value_null_new_s();
        REQUIRE_FALSE(langnes_json_value_is_number_s(json_value));
        langnes_json_value_set_number(json_value, 0);
        REQUIRE(langnes_json_value_is_number_s(json_value));
    }

    SECTION("Change type to boolean") {
        langnes_json_value_t* json_value = langnes_json_value_null_new_s();
        REQUIRE_FALSE(langnes_json_value_is_boolean_s(json_value));
        langnes_json_value_set_boolean(json_value, false);
        REQUIRE(langnes_json_value_is_boolean_s(json_value));
    }

    SECTION("Change type to null") {
        langnes_json_value_t* json_value = langnes_json_value_number_new_s(0);
        REQUIRE_FALSE(langnes_json_value_is_null_s(json_value));
        langnes_json_value_set_null(json_value);
        REQUIRE(langnes_json_value_is_null_s(json_value));
    }

    SECTION("Change type to object") {
        langnes_json_value_t* json_value = langnes_json_value_null_new_s();
        REQUIRE_FALSE(langnes_json_value_is_object_s(json_value));
        langnes_json_value_set_object(json_value);
        REQUIRE(langnes_json_value_is_object_s(json_value));
    }

    SECTION("Change type to array") {
        langnes_json_value_t* json_value = langnes_json_value_null_new_s();
        REQUIRE_FALSE(langnes_json_value_is_array_s(json_value));
        langnes_json_value_set_array(json_value);
        REQUIRE(langnes_json_value_is_array_s(json_value));
    }
}

TEST_CASE("Get string from JSON string value") {
    SECTION("Empty string") {
        langnes_json_value_t* json_value = langnes_json_value_string_new_s("");
        langnes_json_string_t* str =
            langnes_json_value_get_string_s(json_value);
        REQUIRE(langnes_json_string_get_length_s(str) == 0);
        const char* cstr = langnes_json_string_get_cstring_s(str);
        REQUIRE(strcmp(cstr, "") == 0);
    }

    SECTION("Non-empty string") {
        langnes_json_value_t* json_value =
            langnes_json_value_string_new_s("hello");
        langnes_json_string_t* str =
            langnes_json_value_get_string_s(json_value);
        REQUIRE(langnes_json_string_get_length_s(str) == 5);
        const char* cstr = langnes_json_string_get_cstring_s(str);
        REQUIRE(strcmp(cstr, "hello") == 0);
    }
}

TEST_CASE("Replace a JSON value with another JSON value") {
    langnes_json_value_t* json_value = langnes_json_value_number_new_s(1);
    REQUIRE(langnes_json_value_get_number_s(json_value) == 1);
    langnes_json_value_replace(json_value, langnes_json_value_number_new_s(2));
    REQUIRE(langnes_json_value_get_number_s(json_value) == 2);
}

// NOLINTEND(hicpp-use-nullptr,modernize-use-nullptr)
// NOLINTEND(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
// NOLINTEND(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)
