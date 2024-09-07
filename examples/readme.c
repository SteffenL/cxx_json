#include "langnes_json/json.h"
#include <stdio.h>

int main(void) {
    langnes_json_value_t* object = NULL;
    langnes_json_load_from_cstring("{\"color\":\"red\"}", &object);

    langnes_json_value_t* color =
        langnes_json_value_object_get_value_s(object, "color");
    printf("%s\n", langnes_json_value_get_cstring_s(color)); // red

    langnes_json_value_set_cstring(color, "blue");

    langnes_json_value_t* size = langnes_json_value_number_new_s(10);
    langnes_json_value_object_set_value(object, "size", size);

    langnes_json_string_t* json_str = NULL;
    langnes_json_save_to_string(object, &json_str);
    printf("%s\n", langnes_json_string_get_cstring_s(
                       json_str)); // {"size":10,"color":"blue"}
    langnes_json_string_free(json_str);

    langnes_json_value_free(object);
    return 0;
}
