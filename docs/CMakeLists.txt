find_package(Doxygen REQUIRED doxygen)

set(DOXYGEN_EXCLUDE_PATTERNS */detail/*)
set(DOXYGEN_EXCLUDE_SYMBOLS *::detail)
set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_FILE_PATTERNS *.h *.hpp)
set(DOXYGEN_GENERATE_TREEVIEW YES)
set(DOXYGEN_QUIET YES)
set(DOXYGEN_RECURSIVE YES)
set(DOXYGEN_STRIP_FROM_PATH "${PROJECT_SOURCE_DIR}")

doxygen_add_docs(
    langnes_json_api_docs
    "${PROJECT_SOURCE_DIR}/lib/c/include"
    "${PROJECT_SOURCE_DIR}/lib/cxx/include"
    ALL
    COMMENT "Generate API documentation")

if(LANGNES_JSON_INSTALL)
    install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/html"
        DESTINATION "docs"
        COMPONENT langnes_json_docs)
endif()
