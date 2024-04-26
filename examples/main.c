#include "langnes_json/c/json.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void indent(size_t level);
bool dump_value(langnes_json_value_t* value_to_dump, size_t level);

// Construct an object and save it to JSON.
void example_save(void) {
    // Create an array and add elements into it dynamically.
    langnes_json_value_t* values = langnes_json_value_array_new_s();
    langnes_json_value_array_push(
        values, langnes_json_value_string_new_with_cstring_s("hello"));
    langnes_json_value_array_push(values,
                                  langnes_json_value_number_new_s(3.14159));
    langnes_json_value_array_push(values,
                                  langnes_json_value_boolean_new_s(true));
    langnes_json_value_array_push(values, langnes_json_value_null_new_s());

    // Initialize an object with members.
    langnes_json_object_member_t object_members[] = {
        {"x", langnes_json_value_number_new_s(1)},
        {"y", langnes_json_value_number_new_s(2)},
    };
    langnes_json_value_array_push(
        values, langnes_json_value_object_new_with_members_s(
                    object_members,
                    sizeof(object_members) / sizeof(object_members[0])));

    // Initialize an array with elements.
    langnes_json_value_t* array_elements[] = {
        langnes_json_value_number_new_s(1), langnes_json_value_number_new_s(2)};
    langnes_json_value_array_push(
        values, langnes_json_value_array_new_with_elements_s(
                    array_elements,
                    sizeof(array_elements) / sizeof(array_elements[0])));

    // Set member value on an object by member name.
    langnes_json_value_t* object = langnes_json_value_object_new_s();
    langnes_json_value_object_set_value(object, "values", values);

    // Save to JSON.
    langnes_json_string_t* json_str = NULL;
    if (langnes_json_succeeded(
            langnes_json_save_to_string(object, &json_str))) {
        const char* json_cstr = langnes_json_string_get_cstring_s(json_str);
        printf("%s\n", json_cstr);
        langnes_json_string_free(json_str);
    }

    langnes_json_value_free(object);
}

// Load an object from JSON.
void example_load(void) {
    const char* json_str = "{\"values\":[\"hello\",3.14159,[true,false,null],{"
                           "\"colors\":[\"red\",\"green\",\"blue\"]},{},[]]}";

    langnes_json_value_t* object = NULL;
    if (langnes_json_succeeded(
            langnes_json_load_from_cstring(json_str, &object))) {
        dump_value(object, 0);
        langnes_json_value_free(object);
    }
}

void indent(size_t level) {
    for (size_t j = 0; j < level * 2; ++j) {
        putchar(' ');
    }
}

// Performs a deep dump of a JSON value with a custom output format.
bool dump_value(langnes_json_value_t* value_to_dump, size_t level) {
    if (level > 100) {
        return false;
    }

    langnes_json_value_type_t type =
        langnes_json_value_get_type_s(value_to_dump);

    switch (type) {
    case langnes_json_value_type_string: {
        const char* str = langnes_json_value_get_cstring_s(value_to_dump);
        (void)fputs(str, stdout);
        break;
    }
    case langnes_json_value_type_number: {
        double value = langnes_json_value_get_number_s(value_to_dump);
        printf("%.6g", value);
        break;
    }
    case langnes_json_value_type_boolean: {
        bool value = langnes_json_value_get_boolean_s(value_to_dump);
        (void)fputs(value ? "true" : "false", stdout);
        break;
    }
    case langnes_json_value_type_null:
        (void)fputs("null", stdout);
        break;
    case langnes_json_value_type_object: {
        size_t members_length =
            langnes_json_value_object_get_members_length_s(value_to_dump);
        if (members_length > 0) {
            putchar('{');
            putchar('\n');
            for (size_t member_index = 0; member_index < members_length;
                 ++member_index) {
                langnes_json_object_member_t member =
                    langnes_json_value_object_get_member_s(value_to_dump,
                                                           member_index);

                indent(level + 1);
                printf("%s: ", member.name);
                if (!dump_value(member.value, level + 1)) {
                    return false;
                }
                putchar('\n');
            }
            indent(level);
            putchar('}');
        } else {
            (void)fputs("{}", stdout);
        }
        break;
    }
    case langnes_json_value_type_array: {
        size_t values_length =
            langnes_json_value_array_get_length_s(value_to_dump);
        if (values_length > 0) {
            putchar('[');
            putchar('\n');
            for (size_t i = 0; i < values_length; ++i) {
                indent(level + 1);
                langnes_json_value_t* element =
                    langnes_json_value_array_get_item_s(value_to_dump, i);
                if (!dump_value(element, level + 1)) {
                    return false;
                }
                putchar('\n');
            }
            indent(level);
            putchar(']');
        } else {
            (void)fputs("[]", stdout);
        }
        break;
    }
    default:
        abort();
        break;
    }
    return true;
}

int main(void) {
    example_save();
    example_load();
    return 0;
}
