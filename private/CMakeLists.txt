# Targets should either link this with PRIVATE visibility or not link it at all.
add_library(langnes_json_private INTERFACE)
add_library(langnes::json::private ALIAS langnes_json_private)
langnes_json_target_enable_warnings(langnes_json_private INTERFACE)

set_target_properties(langnes_json_private PROPERTIES
    EXPORT_NAME private)

if(MSVC)
    target_compile_options(langnes_json_private INTERFACE /utf-8)
endif()

if(LANGNES_JSON_INSTALL)
    install(TARGETS langnes_json_private EXPORT langnes_json_cxx_targets)
endif()
