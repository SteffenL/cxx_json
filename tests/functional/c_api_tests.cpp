#include "langnes_json/testing.hpp"

#include <langnes_json/json.h>

#include <cstring>

// NOLINTBEGIN(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)
// NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
// NOLINTBEGIN(hicpp-use-nullptr,modernize-use-nullptr)

namespace {
const auto bad{langnes_json_failed};
const auto good{langnes_json_succeeded};
const auto load_cstr{langnes_json_load_from_cstring};
} // namespace

TEST_CASE("Create empty JSON object and populate it") {
    langnes_json_value_t* json_object = langnes_json_value_object_new_s();

    // Make sure no items are added yet.
    REQUIRE(langnes_json_value_object_get_members_length_s(json_object) == 0);

    // Create new items.
    langnes_json_value_t* pushed_item0 =
        langnes_json_value_string_new_with_cstring_s("hello");
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
        langnes_json_value_string_new_with_cstring_s("hello");
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
            langnes_json_value_string_new_with_cstring_s("hello");
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
        langnes_json_value_t* json_value =
            langnes_json_value_string_new_with_cstring_s("");
        langnes_json_string_t* str =
            langnes_json_value_get_string_s(json_value);
        REQUIRE(langnes_json_string_get_length_s(str) == 0);
        const char* cstr = langnes_json_string_get_cstring_s(str);
        REQUIRE(strcmp(cstr, "") == 0);
    }

    SECTION("Non-empty string") {
        langnes_json_value_t* json_value =
            langnes_json_value_string_new_with_cstring_s("hello");
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

TEST_CASE("langnes_json_load_from_cstring - argument validity") {
    SECTION("Should fail with NULL data") {
        langnes_json_value_t* result = NULL;
        REQUIRE(bad(load_cstr(NULL, &result)));
    }
    SECTION("Should fail with NULL result") {
        REQUIRE(bad(load_cstr("{}", NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* result = NULL;
        REQUIRE(good(load_cstr("{}", &result)));
    }
}

TEST_CASE("langnes_json_load_from_cstring - UTF-8") {
    langnes_json_value_t* result = NULL;
    REQUIRE(good(load_cstr(
        "\"foo\\u2753\\x24\\u00a3\\u0418\\u0939\\u20ac\\ud55c\"", &result)));
    REQUIRE(langnes_json_value_is_string_s(result));
    langnes_json_string_t* str = langnes_json_value_get_string_s(result);
    const char* cstr = langnes_json_string_get_cstring_s(str);
    REQUIRE(strcmp(cstr, "foo\u2753\x24\u00a3\u0418\u0939\u20ac\ud55c") == 0);
}

TEST_CASE("langnes_json_load_from_cstring - invalid input") {
    langnes_json_value_t* result = NULL;
    REQUIRE(bad(load_cstr("", &result)));
    REQUIRE(bad(load_cstr("x", &result)));
    REQUIRE(bad(load_cstr("1e", &result)));
    REQUIRE(bad(load_cstr("{", &result)));
    REQUIRE(bad(load_cstr("{x", &result)));
    REQUIRE(bad(load_cstr("{\"", &result)));
    REQUIRE(bad(load_cstr("{\"\"", &result)));
    REQUIRE(bad(load_cstr("{\"\":", &result)));
    REQUIRE(bad(load_cstr("{\"\":x", &result)));
    REQUIRE(bad(load_cstr("{\"\":1,", &result)));
    REQUIRE(bad(load_cstr("{\"\":1,}", &result)));
    REQUIRE(bad(load_cstr("[", &result)));
    REQUIRE(bad(load_cstr("[x", &result)));
    REQUIRE(bad(load_cstr("[1,", &result)));
    REQUIRE(bad(load_cstr("[1,}", &result)));
}

TEST_CASE("langnes_json_load_from_cstring - numbers") {
    langnes_json_value_t* result = NULL;
    REQUIRE(bad(load_cstr("1.", &result)));
    REQUIRE(bad(load_cstr(".1", &result)));
    REQUIRE(bad(load_cstr("1e", &result)));
    REQUIRE(bad(load_cstr("00", &result)));
    REQUIRE(bad(load_cstr("1x1", &result)));
    REQUIRE(bad(load_cstr("+1", &result)));
    REQUIRE(bad(load_cstr("1.0ex1", &result)));
    REQUIRE(good(load_cstr("1", &result)));
    REQUIRE(good(load_cstr("-1", &result)));
    REQUIRE(good(load_cstr("1e1", &result)));
    REQUIRE(good(load_cstr("1E1", &result)));
    REQUIRE(good(load_cstr("1.0e10", &result)));
    REQUIRE(good(load_cstr("1.0e-1", &result)));
    REQUIRE(good(load_cstr("1.0e+1", &result)));
}

TEST_CASE("langnes_json_load_from_cstring - special chars in string") {
    langnes_json_value_t* result = NULL;
    REQUIRE(good(load_cstr("\"\\\"\\\\\\b\\f\\n\\r\\t\"", &result)));
    langnes_json_string_t* str = langnes_json_value_get_string_s(result);
    const char* cstr = langnes_json_string_get_cstring_s(str);
    REQUIRE(strcmp(cstr, "\"\\\b\f\n\r\t") == 0);
}

TEST_CASE("langnes_json_save_to_string - argument validity") {
    SECTION("Should fail with NULL value") {
        langnes_json_string_t* result = NULL;
        REQUIRE(bad(langnes_json_save_to_string(NULL, &result)));
    }
    SECTION("Should fail with NULL result") {
        langnes_json_value_t* json_value = langnes_json_value_null_new_s();
        REQUIRE(bad(langnes_json_save_to_string(json_value, NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* json_value = langnes_json_value_null_new_s();
        langnes_json_string_t* result = NULL;
        REQUIRE(good(langnes_json_save_to_string(json_value, &result)));
    }
}

TEST_CASE("langnes_json_value_free - argument validity") {
    SECTION("Should fail with NULL value") {
        REQUIRE(bad(langnes_json_value_free(NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* json_value = langnes_json_value_null_new_s();
        REQUIRE(good(langnes_json_value_free(json_value)));
    }
}

TEST_CASE("langnes_json_value_replace - argument validity") {
    SECTION("Should fail with NULL target") {
        langnes_json_value_t* replacement = langnes_json_value_null_new_s();
        REQUIRE(bad(langnes_json_value_replace(NULL, replacement)));
    }
    SECTION("Should fail with NULL replacement") {
        langnes_json_value_t* target = langnes_json_value_null_new_s();
        REQUIRE(bad(langnes_json_value_replace(target, NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* target = langnes_json_value_null_new_s();
        langnes_json_value_t* replacement = langnes_json_value_number_new_s(0);
        REQUIRE(good(langnes_json_value_replace(target, replacement)));
    }
}

TEST_CASE("langnes_json_value_get_type - argument validity") {
    SECTION("Should fail with NULL value") {
        langnes_json_value_type_t result = langnes_json_value_type_null;
        REQUIRE(bad(langnes_json_value_get_type(NULL, &result)));
    }
    SECTION("Should fail with NULL result") {
        langnes_json_value_t* json_value = langnes_json_value_null_new_s();
        REQUIRE(bad(langnes_json_value_get_type(json_value, NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* json_value = langnes_json_value_null_new_s();
        langnes_json_value_type_t result = langnes_json_value_type_null;
        REQUIRE(good(langnes_json_value_get_type(json_value, &result)));
    }
}

TEST_CASE("langnes_json_value_clone - argument validity") {
    SECTION("Should fail with NULL value") {
        langnes_json_value_t* result = NULL;
        REQUIRE(bad(langnes_json_value_clone(NULL, &result)));
    }
    SECTION("Should fail with NULL result") {
        langnes_json_value_t* json_value = langnes_json_value_number_new_s(123);
        REQUIRE(bad(langnes_json_value_clone(json_value, NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* json_value = langnes_json_value_number_new_s(123);
        langnes_json_value_t* result = NULL;
        REQUIRE(good(langnes_json_value_clone(json_value, &result)));
        REQUIRE(result != NULL);
        REQUIRE(json_value != result);
        REQUIRE(langnes_json_value_get_number_s(result) == 123);
    }
}

TEST_CASE("langnes_json_value_clone") {
    REQUIRE(strcmp(langnes_json_value_get_cstring_s(langnes_json_value_clone_s(
                       langnes_json_value_string_new_with_cstring_s("foo"))),
                   "foo") == 0);
    REQUIRE(langnes_json_value_get_number_s(langnes_json_value_clone_s(
                langnes_json_value_number_new_s(123))) == 123);
    REQUIRE(langnes_json_value_get_boolean_s(langnes_json_value_clone_s(
                langnes_json_value_boolean_new_s(true))) == true);
    REQUIRE(langnes_json_value_get_boolean_s(langnes_json_value_clone_s(
                langnes_json_value_boolean_new_s(false))) == false);
    REQUIRE(langnes_json_value_is_null_s(
        langnes_json_value_clone_s(langnes_json_value_null_new_s())));

    SECTION("object") {
        langnes_json_object_member_t members[] = {
            {"x", langnes_json_value_number_new_s(123)}};
        langnes_json_value_t* object =
            langnes_json_value_object_new_with_members_s(members, 1);
        langnes_json_value_t* clone = langnes_json_value_clone_s(object);
        langnes_json_value_t* value =
            langnes_json_value_object_get_value_s(clone, "x");
        REQUIRE(langnes_json_value_get_number_s(value) == 123);
    }

    SECTION("array") {
        langnes_json_value_t* elements[] = {
            langnes_json_value_number_new_s(123)};
        langnes_json_value_t* array =
            langnes_json_value_array_new_with_elements_s(elements, 1);
        langnes_json_value_t* clone = langnes_json_value_clone_s(array);
        langnes_json_value_t* value =
            langnes_json_value_array_get_item_s(clone, 0);
        REQUIRE(langnes_json_value_get_number_s(value) == 123);
    }
}

TEST_CASE("langnes_json_value_clone_s") {
    langnes_json_value_t* value = langnes_json_value_number_new_s(123);
    langnes_json_value_t* cloned = langnes_json_value_clone_s(value);
    REQUIRE(langnes_json_value_get_number_s(cloned) == 123);
}

TEST_CASE("langnes_json_string_get_cstring - argument validity") {
    SECTION("Should fail with NULL string") {
        const char* result = NULL;
        REQUIRE(bad(langnes_json_string_get_cstring(NULL, &result)));
    }
    SECTION("Should fail with NULL result") {
        langnes_json_value_t* json_value =
            langnes_json_value_string_new_with_cstring_s("");
        langnes_json_string_t* str =
            langnes_json_value_get_string_s(json_value);
        REQUIRE(bad(langnes_json_string_get_cstring(str, NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* json_value =
            langnes_json_value_string_new_with_cstring_s("");
        langnes_json_string_t* str =
            langnes_json_value_get_string_s(json_value);
        const char* result = NULL;
        REQUIRE(good(langnes_json_string_get_cstring(str, &result)));
    }
}

TEST_CASE("langnes_json_string_get_length - argument validity") {
    SECTION("Should fail with NULL string") {
        size_t result = 0;
        REQUIRE(bad(langnes_json_string_get_length(NULL, &result)));
    }
    SECTION("Should fail with NULL result") {
        langnes_json_value_t* json_value =
            langnes_json_value_string_new_with_cstring_s("");
        langnes_json_string_t* str =
            langnes_json_value_get_string_s(json_value);
        REQUIRE(bad(langnes_json_string_get_length(str, NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* json_value =
            langnes_json_value_string_new_with_cstring_s("");
        langnes_json_string_t* str =
            langnes_json_value_get_string_s(json_value);
        size_t result = 0;
        REQUIRE(good(langnes_json_string_get_length(str, &result)));
    }
}

TEST_CASE("langnes_json_string_free - argument validity") {
    SECTION("Should fail with NULL string") {
        REQUIRE(bad(langnes_json_string_free(NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* json_value =
            langnes_json_value_string_new_with_cstring_s("");
        langnes_json_string_t* result = NULL;
        REQUIRE(good(langnes_json_save_to_string(json_value, &result)));
        REQUIRE(good(langnes_json_string_free(result)));
    }
}

TEST_CASE("langnes_json_value_string_new_with_string - argument validity") {
    SECTION("Should fail with NULL data") {
        langnes_json_value_t* result = NULL;
        REQUIRE(bad(langnes_json_value_string_new_with_string(NULL, &result)));
    }
    SECTION("Should fail with NULL result") {
        langnes_json_value_t* value =
            langnes_json_value_string_new_with_cstring_s("{}");
        langnes_json_string_t* str = langnes_json_value_get_string_s(value);
        REQUIRE(bad(langnes_json_value_string_new_with_string(str, NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* value =
            langnes_json_value_string_new_with_cstring_s("{}");
        langnes_json_string_t* str = langnes_json_value_get_string_s(value);
        langnes_json_value_t* result = NULL;
        REQUIRE(good(langnes_json_value_string_new_with_string(str, &result)));
    }
}

TEST_CASE("langnes_json_value_string_new_with_cstring - argument validity") {
    SECTION("Should fail with NULL data") {
        langnes_json_value_t* result = NULL;
        REQUIRE(bad(langnes_json_value_string_new_with_cstring(NULL, &result)));
    }
    SECTION("Should fail with NULL result") {
        REQUIRE(bad(langnes_json_value_string_new_with_cstring("{}", NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* result = NULL;
        REQUIRE(
            good(langnes_json_value_string_new_with_cstring("{}", &result)));
    }
}

TEST_CASE("langnes_json_value_string_new_with_string_s") {
    langnes_json_value_t* value =
        langnes_json_value_string_new_with_cstring_s("{}");
    langnes_json_string_t* str = langnes_json_value_get_string_s(value);
    langnes_json_value_t* result =
        langnes_json_value_string_new_with_string_s(str);
    langnes_json_string_t* result_str = langnes_json_value_get_string_s(result);
    const char* result_cstr = langnes_json_string_get_cstring_s(result_str);
    REQUIRE(strcmp(result_cstr, "{}") == 0);
}

TEST_CASE("langnes_json_value_is_string - argument validity") {
    SECTION("Should fail with NULL value") {
        bool result = false;
        REQUIRE(bad(langnes_json_value_is_string(NULL, &result)));
    }
    SECTION("Should fail with NULL result") {
        langnes_json_value_t* json_value =
            langnes_json_value_string_new_with_cstring_s("");
        REQUIRE(bad(langnes_json_value_is_string(json_value, NULL)));
    }
    SECTION("Should return false with different type") {
        langnes_json_value_t* json_value = langnes_json_value_null_new_s();
        bool result = true;
        REQUIRE(good(langnes_json_value_is_string(json_value, &result)));
        REQUIRE(!result);
    }
    SECTION("Should return true with same type") {
        langnes_json_value_t* json_value =
            langnes_json_value_string_new_with_cstring_s("");
        bool result = false;
        REQUIRE(good(langnes_json_value_is_string(json_value, &result)));
        REQUIRE(result);
    }
}

TEST_CASE("langnes_json_value_get_string - argument validity") {
    SECTION("Should fail with NULL value") {
        langnes_json_string_t* result = NULL;
        REQUIRE(bad(langnes_json_value_get_string(NULL, &result)));
    }
    SECTION("Should fail with NULL result") {
        langnes_json_value_t* json_value =
            langnes_json_value_string_new_with_cstring_s("");
        REQUIRE(bad(langnes_json_value_get_string(json_value, NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* json_value =
            langnes_json_value_string_new_with_cstring_s("");
        langnes_json_string_t* result = NULL;
        REQUIRE(good(langnes_json_value_get_string(json_value, &result)));
    }
}

TEST_CASE("langnes_json_value_get_cstring - argument validity") {
    SECTION("Should fail with NULL value") {
        const char* result = NULL;
        REQUIRE(bad(langnes_json_value_get_cstring(NULL, &result)));
    }
    SECTION("Should fail with NULL result") {
        langnes_json_value_t* json_value =
            langnes_json_value_string_new_with_cstring_s("");
        REQUIRE(bad(langnes_json_value_get_cstring(json_value, NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* json_value =
            langnes_json_value_string_new_with_cstring_s("");
        const char* result = NULL;
        REQUIRE(good(langnes_json_value_get_cstring(json_value, &result)));
    }
}

TEST_CASE("langnes_json_value_set_string - argument validity") {
    SECTION("Should fail with NULL value") {
        REQUIRE(bad(langnes_json_value_set_string(NULL, "")));
    }
    SECTION("Should fail with NULL data") {
        langnes_json_value_t* json_value =
            langnes_json_value_string_new_with_cstring_s("");
        REQUIRE(bad(langnes_json_value_set_string(json_value, NULL)));
    }
    SECTION("Should succeed with valid arguments and same value type") {
        langnes_json_value_t* json_value =
            langnes_json_value_string_new_with_cstring_s("");
        REQUIRE(good(langnes_json_value_set_string(json_value, "")));
    }
    SECTION("Should succeed with valid arguments and different value type") {
        langnes_json_value_t* json_value = langnes_json_value_null_new_s();
        REQUIRE(good(langnes_json_value_set_string(json_value, "")));
    }
}

TEST_CASE("langnes_json_value_number_new - argument validity") {
    SECTION("Should fail with NULL result") {
        REQUIRE(bad(langnes_json_value_number_new(0, NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* result = NULL;
        REQUIRE(good(langnes_json_value_number_new(0, &result)));
    }
}

TEST_CASE("langnes_json_value_is_number - argument validity") {
    SECTION("Should fail with NULL value") {
        bool result = false;
        REQUIRE(bad(langnes_json_value_is_number(NULL, &result)));
    }
    SECTION("Should fail with NULL result") {
        langnes_json_value_t* json_value = langnes_json_value_number_new_s(0);
        REQUIRE(bad(langnes_json_value_is_number(json_value, NULL)));
    }
    SECTION("Should return false with different type") {
        langnes_json_value_t* json_value = langnes_json_value_null_new_s();
        bool result = true;
        REQUIRE(good(langnes_json_value_is_number(json_value, &result)));
        REQUIRE(!result);
    }
    SECTION("Should return true with same type") {
        langnes_json_value_t* json_value = langnes_json_value_number_new_s(0);
        bool result = false;
        REQUIRE(good(langnes_json_value_is_number(json_value, &result)));
        REQUIRE(result);
    }
}

TEST_CASE("langnes_json_value_get_number - argument validity") {
    SECTION("Should fail with NULL value") {
        double result = 0;
        REQUIRE(bad(langnes_json_value_get_number(NULL, &result)));
    }
    SECTION("Should fail with NULL result") {
        langnes_json_value_t* json_value = langnes_json_value_number_new_s(0);
        REQUIRE(bad(langnes_json_value_get_number(json_value, NULL)));
    }
    SECTION("Should fail with different type") {
        langnes_json_value_t* json_value = langnes_json_value_null_new_s();
        double result = 0;
        REQUIRE(bad(langnes_json_value_get_number(json_value, &result)));
    }
    SECTION("Should succeed with same type") {
        langnes_json_value_t* json_value = langnes_json_value_number_new_s(123);
        double result = 0;
        REQUIRE(good(langnes_json_value_get_number(json_value, &result)));
        REQUIRE(result == 123);
    }
}

TEST_CASE("langnes_json_value_set_number - argument validity") {
    SECTION("Should fail with NULL value") {
        REQUIRE(bad(langnes_json_value_set_number(NULL, 0)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* json_value = langnes_json_value_null_new_s();
        REQUIRE(good(langnes_json_value_set_number(json_value, 123)));
    }
}

TEST_CASE("langnes_json_value_boolean_new - argument validity") {
    SECTION("Should fail with NULL result") {
        REQUIRE(bad(langnes_json_value_boolean_new(true, NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* result = NULL;
        REQUIRE(good(langnes_json_value_boolean_new(true, &result)));
    }
}

TEST_CASE("langnes_json_value_is_boolean - argument validity") {
    SECTION("Should fail with NULL value") {
        bool result = false;
        REQUIRE(bad(langnes_json_value_is_boolean(NULL, &result)));
    }
    SECTION("Should fail with NULL result") {
        langnes_json_value_t* json_value =
            langnes_json_value_boolean_new_s(false);
        REQUIRE(bad(langnes_json_value_is_boolean(json_value, NULL)));
    }
    SECTION("Should return false with different type") {
        langnes_json_value_t* json_value = langnes_json_value_null_new_s();
        bool result = true;
        REQUIRE(good(langnes_json_value_is_boolean(json_value, &result)));
        REQUIRE(!result);
    }
    SECTION("Should return true with same type") {
        langnes_json_value_t* json_value =
            langnes_json_value_boolean_new_s(false);
        bool result = false;
        REQUIRE(good(langnes_json_value_is_boolean(json_value, &result)));
        REQUIRE(result);
    }
}

TEST_CASE("langnes_json_value_get_boolean - argument validity") {
    SECTION("Should fail with NULL value") {
        bool result = false;
        REQUIRE(bad(langnes_json_value_get_boolean(NULL, &result)));
    }
    SECTION("Should fail with NULL result") {
        langnes_json_value_t* json_value =
            langnes_json_value_boolean_new_s(false);
        REQUIRE(bad(langnes_json_value_get_boolean(json_value, NULL)));
    }
    SECTION("Should fail with different type") {
        langnes_json_value_t* json_value = langnes_json_value_null_new_s();
        bool result = false;
        REQUIRE(bad(langnes_json_value_get_boolean(json_value, &result)));
    }
    SECTION("Should succeed with same type") {
        langnes_json_value_t* json_value =
            langnes_json_value_boolean_new_s(true);
        bool result = false;
        REQUIRE(good(langnes_json_value_get_boolean(json_value, &result)));
        REQUIRE(result);
    }
}

TEST_CASE("langnes_json_value_set_boolean - argument validity") {
    SECTION("Should fail with NULL value") {
        REQUIRE(bad(langnes_json_value_set_boolean(NULL, false)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* json_value = langnes_json_value_null_new_s();
        REQUIRE(good(langnes_json_value_set_boolean(json_value, false)));
    }
}

TEST_CASE("langnes_json_value_null_new - argument validity") {
    SECTION("Should fail with NULL result") {
        REQUIRE(bad(langnes_json_value_null_new(NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* result = NULL;
        REQUIRE(good(langnes_json_value_null_new(&result)));
    }
}

TEST_CASE("langnes_json_value_is_null - argument validity") {
    SECTION("Should fail with NULL value") {
        bool result = false;
        REQUIRE(bad(langnes_json_value_is_null(NULL, &result)));
    }
    SECTION("Should fail with NULL result") {
        langnes_json_value_t* json_value = langnes_json_value_null_new_s();
        REQUIRE(bad(langnes_json_value_is_null(json_value, NULL)));
    }
    SECTION("Should return false with different type") {
        langnes_json_value_t* json_value = langnes_json_value_number_new_s(0);
        bool result = true;
        REQUIRE(good(langnes_json_value_is_null(json_value, &result)));
        REQUIRE(!result);
    }
    SECTION("Should return true with same type") {
        langnes_json_value_t* json_value = langnes_json_value_null_new_s();
        bool result = false;
        REQUIRE(good(langnes_json_value_is_null(json_value, &result)));
        REQUIRE(result);
    }
}

TEST_CASE("langnes_json_value_set_null - argument validity") {
    SECTION("Should fail with NULL value") {
        REQUIRE(bad(langnes_json_value_set_null(NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* json_value = langnes_json_value_number_new_s(0);
        REQUIRE(good(langnes_json_value_set_null(json_value)));
    }
}

TEST_CASE("langnes_json_value_object_new - argument validity") {
    SECTION("Should fail with NULL result") {
        REQUIRE(bad(langnes_json_value_object_new(NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* result = NULL;
        REQUIRE(good(langnes_json_value_object_new(&result)));
    }
}

TEST_CASE("langnes_json_value_object_new_with_members - argument validity") {
    SECTION("Should fail with NULL members") {
        langnes_json_value_t* result = NULL;
        REQUIRE(
            bad(langnes_json_value_object_new_with_members(NULL, 0, &result)));
    }
    SECTION("Should fail with NULL result") {
        langnes_json_object_member_t object_members[] = {
            {"x", langnes_json_value_number_new_s(1)}};
        REQUIRE(bad(langnes_json_value_object_new_with_members(object_members,
                                                               1, NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_object_member_t object_members[] = {
            {"x", langnes_json_value_number_new_s(1)}};
        langnes_json_value_t* result = NULL;
        REQUIRE(good(langnes_json_value_object_new_with_members(object_members,
                                                                1, &result)));
    }
}

TEST_CASE("langnes_json_value_is_object - argument validity") {
    SECTION("Should fail with NULL value") {
        bool result = false;
        REQUIRE(bad(langnes_json_value_is_object(NULL, &result)));
    }
    SECTION("Should fail with NULL result") {
        langnes_json_value_t* json_value = langnes_json_value_object_new_s();
        REQUIRE(bad(langnes_json_value_is_object(json_value, NULL)));
    }
    SECTION("Should return false with different type") {
        langnes_json_value_t* json_value = langnes_json_value_null_new_s();
        bool result = true;
        REQUIRE(good(langnes_json_value_is_object(json_value, &result)));
        REQUIRE(!result);
    }
    SECTION("Should return true with same type") {
        langnes_json_value_t* json_value = langnes_json_value_object_new_s();
        bool result = false;
        REQUIRE(good(langnes_json_value_is_object(json_value, &result)));
        REQUIRE(result);
    }
}

TEST_CASE("langnes_json_value_object_get_value - argument validity") {
    SECTION("Should fail with NULL value") {
        langnes_json_value_t* result = NULL;
        REQUIRE(bad(langnes_json_value_object_get_value(NULL, "x", &result)));
    }
    SECTION("Should fail with NULL member name") {
        langnes_json_value_t* json_value = langnes_json_value_object_new_s();
        langnes_json_value_t* result = NULL;
        REQUIRE(bad(
            langnes_json_value_object_get_value(json_value, NULL, &result)));
    }
    SECTION("Should fail with NULL result") {
        langnes_json_value_t* json_value = langnes_json_value_object_new_s();
        langnes_json_value_object_set_value(json_value, "x",
                                            langnes_json_value_null_new_s());
        REQUIRE(
            bad(langnes_json_value_object_get_value(json_value, "x", NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* json_value = langnes_json_value_object_new_s();
        langnes_json_value_object_set_value(json_value, "x",
                                            langnes_json_value_null_new_s());
        langnes_json_value_t* result = NULL;
        REQUIRE(good(
            langnes_json_value_object_get_value(json_value, "x", &result)));
    }
}

TEST_CASE("langnes_json_value_object_get_value - key range") {
    langnes_json_value_t* result = NULL;
    langnes_json_value_t* json_value = langnes_json_value_object_new_s();
    REQUIRE(bad(langnes_json_value_object_get_value(json_value, "x", &result)));
    langnes_json_value_object_set_value(json_value, "x",
                                        langnes_json_value_number_new_s(123));
    REQUIRE(
        good(langnes_json_value_object_get_value(json_value, "x", &result)));
    langnes_json_value_object_clear(json_value);
    REQUIRE(bad(langnes_json_value_object_get_value(json_value, "x", &result)));
}

TEST_CASE("langnes_json_value_object_set_value - argument validity") {
    SECTION("Should fail with NULL value") {
        REQUIRE(bad(langnes_json_value_object_set_value(
            NULL, "x", langnes_json_value_null_new_s())));
    }
    SECTION("Should fail with NULL member name") {
        langnes_json_value_t* json_value = langnes_json_value_object_new_s();
        REQUIRE(bad(langnes_json_value_object_set_value(
            json_value, NULL, langnes_json_value_null_new_s())));
    }
    SECTION("Should fail with NULL member value") {
        langnes_json_value_t* json_value = langnes_json_value_object_new_s();
        REQUIRE(
            bad(langnes_json_value_object_set_value(json_value, "x", NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* json_value = langnes_json_value_object_new_s();
        REQUIRE(good(langnes_json_value_object_set_value(
            json_value, "x", langnes_json_value_null_new_s())));
    }
}

TEST_CASE("langnes_json_value_set_object - argument validity") {
    SECTION("Should fail with NULL value") {
        REQUIRE(bad(langnes_json_value_set_object(NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* json_value = langnes_json_value_null_new_s();
        REQUIRE(good(langnes_json_value_set_object(json_value)));
    }
}

TEST_CASE("langnes_json_value_set_object_with_members - argument validity") {
    SECTION("Should fail with NULL value") {
        langnes_json_object_member_t object_members[] = {
            {"x", langnes_json_value_number_new_s(1)}};
        REQUIRE(bad(langnes_json_value_set_object_with_members(
            NULL, object_members, 1)));
    }
    SECTION("Should fail with NULL members") {
        langnes_json_value_t* json_value = langnes_json_value_null_new_s();
        REQUIRE(bad(
            langnes_json_value_set_object_with_members(json_value, NULL, 0)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* json_value = langnes_json_value_null_new_s();
        langnes_json_object_member_t object_members[] = {
            {"x", langnes_json_value_number_new_s(1)}};
        langnes_json_value_t* result = NULL;
        REQUIRE(good(langnes_json_value_set_object_with_members(
            json_value, object_members, 1)));
    }
}

TEST_CASE("langnes_json_value_object_get_members_length - argument validity") {
    SECTION("Should fail with NULL object") {
        size_t result = 0;
        REQUIRE(
            bad(langnes_json_value_object_get_members_length(NULL, &result)));
    }
    SECTION("Should fail with NULL result") {
        langnes_json_value_t* json_value = langnes_json_value_object_new_s();
        REQUIRE(bad(
            langnes_json_value_object_get_members_length(json_value, NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* json_value = langnes_json_value_object_new_s();
        size_t result = 123;
        REQUIRE(good(
            langnes_json_value_object_get_members_length(json_value, &result)));
        REQUIRE(result == 0);
    }
}

TEST_CASE("langnes_json_value_object_get_member - argument validity") {
    SECTION("Should fail with NULL object") {
        langnes_json_object_member_t result = {};
        REQUIRE(bad(langnes_json_value_object_get_member(NULL, 0, &result)));
    }
    SECTION("Should fail with NULL result") {
        langnes_json_value_t* json_value = langnes_json_value_object_new_s();
        langnes_json_value_object_set_value(
            json_value, "x", langnes_json_value_number_new_s(123));
        langnes_json_object_member_t result = {};
        REQUIRE(bad(langnes_json_value_object_get_member(json_value, 0, NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* json_value = langnes_json_value_object_new_s();
        langnes_json_value_object_set_value(
            json_value, "x", langnes_json_value_number_new_s(123));
        langnes_json_object_member_t result = {};
        REQUIRE(
            good(langnes_json_value_object_get_member(json_value, 0, &result)));
        REQUIRE(strcmp(result.name, "x") == 0);
    }
}

TEST_CASE("langnes_json_value_object_get_member - index range") {
    langnes_json_value_t* json_value = langnes_json_value_object_new_s();
    langnes_json_object_member_t result = {};
    REQUIRE(bad(langnes_json_value_object_get_member(json_value, 0, &result)));
    REQUIRE(bad(langnes_json_value_object_get_member(json_value, 1, &result)));
    langnes_json_value_object_set_value(json_value, "x",
                                        langnes_json_value_number_new_s(123));
    REQUIRE(good(langnes_json_value_object_get_member(json_value, 0, &result)));
    REQUIRE(bad(langnes_json_value_object_get_member(json_value, 1, &result)));
    langnes_json_value_object_clear(json_value);
    REQUIRE(bad(langnes_json_value_object_get_member(json_value, 0, &result)));
    REQUIRE(bad(langnes_json_value_object_get_member(json_value, 1, &result)));
}

TEST_CASE("langnes_json_value_object_clear - argument validity") {
    SECTION("Should fail with NULL object") {
        REQUIRE(bad(langnes_json_value_object_clear(NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* json_value = langnes_json_value_object_new_s();
        REQUIRE(good(langnes_json_value_object_clear(json_value)));
    }
}

TEST_CASE("langnes_json_value_array_new - argument validity") {
    SECTION("Should fail with NULL result") {
        REQUIRE(bad(langnes_json_value_array_new(NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* result = NULL;
        REQUIRE(good(langnes_json_value_array_new(&result)));
    }
}

TEST_CASE("langnes_json_value_array_new_with_elements - argument validity") {
    SECTION("Should fail with NULL members") {
        langnes_json_value_t* result = NULL;
        REQUIRE(
            bad(langnes_json_value_array_new_with_elements(NULL, 0, &result)));
    }
    SECTION("Should fail with NULL result") {
        langnes_json_value_t* array_elements[] = {
            langnes_json_value_number_new_s(1)};
        REQUIRE(bad(langnes_json_value_array_new_with_elements(array_elements,
                                                               1, NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* array_elements[] = {
            langnes_json_value_number_new_s(1)};
        langnes_json_value_t* result = NULL;
        REQUIRE(good(langnes_json_value_array_new_with_elements(array_elements,
                                                                1, &result)));
    }
}

TEST_CASE("langnes_json_value_is_array - argument validity") {
    SECTION("Should fail with NULL value") {
        bool result = false;
        REQUIRE(bad(langnes_json_value_is_array(NULL, &result)));
    }
    SECTION("Should fail with NULL result") {
        langnes_json_value_t* json_value = langnes_json_value_array_new_s();
        REQUIRE(bad(langnes_json_value_is_array(json_value, NULL)));
    }
    SECTION("Should return false with different type") {
        langnes_json_value_t* json_value = langnes_json_value_null_new_s();
        bool result = true;
        REQUIRE(good(langnes_json_value_is_array(json_value, &result)));
        REQUIRE(!result);
    }
    SECTION("Should return true with same type") {
        langnes_json_value_t* json_value = langnes_json_value_array_new_s();
        bool result = false;
        REQUIRE(good(langnes_json_value_is_array(json_value, &result)));
        REQUIRE(result);
    }
}

TEST_CASE("langnes_json_value_array_get_length - argument validity") {
    SECTION("Should fail with NULL array") {
        size_t result = 0;
        REQUIRE(bad(langnes_json_value_array_get_length(NULL, &result)));
    }
    SECTION("Should fail with NULL result") {
        langnes_json_value_t* json_value = langnes_json_value_array_new_s();
        REQUIRE(bad(langnes_json_value_array_get_length(json_value, NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* json_value = langnes_json_value_array_new_s();
        size_t result = 0;
        REQUIRE(good(langnes_json_value_array_get_length(json_value, &result)));
    }
}

TEST_CASE("langnes_json_value_array_clear - argument validity") {
    SECTION("Should fail with NULL array") {
        REQUIRE(bad(langnes_json_value_array_clear(NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* json_value = langnes_json_value_array_new_s();
        REQUIRE(good(langnes_json_value_array_clear(json_value)));
    }
}

TEST_CASE("langnes_json_value_array_push - argument validity") {
    SECTION("Should fail with NULL array") {
        langnes_json_value_t* element = langnes_json_value_null_new_s();
        REQUIRE(bad(langnes_json_value_array_push(NULL, element)));
    }
    SECTION("Should fail with NULL element") {
        langnes_json_value_t* json_array = langnes_json_value_array_new_s();
        REQUIRE(bad(langnes_json_value_array_push(json_array, NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* json_array = langnes_json_value_array_new_s();
        langnes_json_value_t* element = langnes_json_value_null_new_s();
        REQUIRE(good(langnes_json_value_array_push(json_array, element)));
    }
}

TEST_CASE("langnes_json_value_array_get_item - argument validity") {
    SECTION("Should fail with NULL object") {
        langnes_json_value_t* result = NULL;
        REQUIRE(bad(langnes_json_value_array_get_item(NULL, 0, &result)));
    }
    SECTION("Should fail with NULL result") {
        langnes_json_value_t* json_value = langnes_json_value_array_new_s();
        langnes_json_value_array_push(json_value,
                                      langnes_json_value_number_new_s(123));
        langnes_json_value_t* result = NULL;
        REQUIRE(bad(langnes_json_value_array_get_item(json_value, 0, NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* json_value = langnes_json_value_array_new_s();
        langnes_json_value_array_push(json_value,
                                      langnes_json_value_number_new_s(123));
        langnes_json_value_t* result = NULL;
        REQUIRE(
            good(langnes_json_value_array_get_item(json_value, 0, &result)));
    }
}

TEST_CASE("langnes_json_value_array_get_item - index range") {
    langnes_json_value_t* result = NULL;
    langnes_json_value_t* json_value = langnes_json_value_array_new_s();
    REQUIRE(bad(langnes_json_value_array_get_item(json_value, 0, &result)));
    REQUIRE(bad(langnes_json_value_array_get_item(json_value, 1, &result)));
    langnes_json_value_array_push(json_value,
                                  langnes_json_value_number_new_s(123));
    REQUIRE(good(langnes_json_value_array_get_item(json_value, 0, &result)));
    REQUIRE(bad(langnes_json_value_array_get_item(json_value, 1, &result)));
    langnes_json_value_array_clear(json_value);
    REQUIRE(bad(langnes_json_value_array_get_item(json_value, 0, &result)));
    REQUIRE(bad(langnes_json_value_array_get_item(json_value, 1, &result)));
}

TEST_CASE("langnes_json_value_set_array - argument validity") {
    SECTION("Should fail with NULL value") {
        REQUIRE(bad(langnes_json_value_set_array(NULL)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* json_value = langnes_json_value_null_new_s();
        REQUIRE(good(langnes_json_value_set_array(json_value)));
    }
}

TEST_CASE("langnes_json_value_set_array_with_elements - argument validity") {
    SECTION("Should fail with NULL value") {
        langnes_json_value_t* array_members[] = {
            langnes_json_value_number_new_s(1)};
        REQUIRE(bad(langnes_json_value_set_array_with_elements(
            NULL, array_members, 1)));
    }
    SECTION("Should fail with NULL members") {
        langnes_json_value_t* json_value = langnes_json_value_null_new_s();
        REQUIRE(bad(
            langnes_json_value_set_array_with_elements(json_value, NULL, 0)));
    }
    SECTION("Should succeed with valid arguments") {
        langnes_json_value_t* json_value = langnes_json_value_null_new_s();
        langnes_json_value_t* array_members[] = {
            langnes_json_value_number_new_s(1)};
        langnes_json_value_t* result = NULL;
        REQUIRE(good(langnes_json_value_set_array_with_elements(
            json_value, array_members, 1)));
    }
}

// NOLINTEND(hicpp-use-nullptr,modernize-use-nullptr)
// NOLINTEND(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
// NOLINTEND(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)
