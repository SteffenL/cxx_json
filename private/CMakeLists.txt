# Targets should either link this with PRIVATE visibility or not link it at all.
add_library(langnes_json_private INTERFACE)
langnes_json_target_enable_warnings(langnes_json_private INTERFACE)

if(MSVC)
    target_compile_options(langnes_json_private INTERFACE /utf-8)
endif()
