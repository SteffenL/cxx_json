# Enables compile warnings.
macro(langnes_json_target_enable_warnings TARGET VISIBILITY)
    if(MSVC)
        target_compile_options(${TARGET} ${VISIBILITY} /WX /W4)
    else()
        target_compile_options(${TARGET} ${VISIBILITY} -Werror -Wall -Wextra -Wpedantic)
    endif()
endmacro()

# Link a library with build (local) interface.
macro(langnes_json_target_link_private_build_library TARGET LINK_TARGET)
    if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.26")
        target_link_libraries("${TARGET}" PRIVATE "$<BUILD_LOCAL_INTERFACE:${LINK_TARGET}>")
    else()
        target_link_libraries("${TARGET}" PRIVATE "$<BUILD_INTERFACE:${LINK_TARGET}>")
    endif()
endmacro()

# Sets CMAKE_INSTALL_RPATH.
macro(langnes_json_target_set_install_rpath)
    # Set install RPATH
    if(CMAKE_SYSTEM_NAME STREQUAL Darwin OR CMAKE_SYSTEM_NAME STREQUAL Linux)
        # RPATH/RUNPATH
        if(CMAKE_SYSTEM_NAME STREQUAL Darwin)
            set(RPATH_BASE @loader_path)
        else()
            set(RPATH_BASE $ORIGIN)
        endif()

        file(RELATIVE_PATH RPATH_SUBDIR
            "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}"
            "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
        set(CMAKE_INSTALL_RPATH "${RPATH_BASE}" "${RPATH_BASE}/${RPATH_SUBDIR}")
    endif()
endmacro()

# Adds build type/configuration for profiling/coverage.
macro(langnes_json_target_add_profile_build_type)
    # Add custom build types
    if(CMAKE_CONFIGURATION_TYPES)
        list(APPEND CMAKE_CONFIGURATION_TYPES Profile)
        list(REMOVE_DUPLICATES CMAKE_CONFIGURATION_TYPES)
        set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "" FORCE)
    endif()

    # Use custom compiler/linker flags for the "Profile" build type
    if((CMAKE_C_COMPILER_ID MATCHES "((^GNU)|Clang)$") OR (CMAKE_CXX_COMPILER_ID MATCHES "((^GNU)|Clang)$"))
        set(CMAKE_C_FLAGS_PROFILE "-g -O0 -fprofile-arcs -ftest-coverage")
        set(CMAKE_CXX_FLAGS_PROFILE "-g -O0 -fprofile-arcs -ftest-coverage")
        set(CMAKE_EXE_LINKER_FLAGS_PROFILE "-fprofile-arcs")
        set(CMAKE_SHARED_LINKER_FLAGS_PROFILE "-fprofile-arcs")
    elseif(MSVC)
        # MSVC isn't supported but warnings are emitted if these variables are undefined
        set(CMAKE_C_FLAGS_PROFILE "/Od")
        set(CMAKE_CXX_FLAGS_PROFILE "/Od")
        set(CMAKE_EXE_LINKER_FLAGS_PROFILE "")
        set(CMAKE_SHARED_LINKER_FLAGS_PROFILE "")
    endif()
endmacro()

# Sets default build type for single-config generators.
macro(langnes_json_set_default_build_type)
    get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if(NOT IS_MULTI_CONFIG AND NOT DEFINED CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release CACHE STRING "")
    endif()
endmacro()

# Sets default language standards.
macro(langnes_json_set_language_standards)
    set(CMAKE_C_STANDARD 99 CACHE STRING "")
    set(CMAKE_C_STANDARD_REQUIRED YES)
    set(CMAKE_C_EXTENSIONS NO)
    set(CMAKE_CXX_STANDARD 11 CACHE STRING "")
    set(CMAKE_CXX_STANDARD_REQUIRED YES)
    set(CMAKE_CXX_EXTENSIONS NO)
endmacro()

macro(langnes_json_extract_version)
    set(LANGNES_JSON_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/lib/cxx/include)

    # Extract version information from source code
    file(READ "${LANGNES_JSON_INCLUDE_DIR}/langnes_json/cxx/json.hpp" LANGNES_JSON_H_CONTENT)
    string(REGEX MATCH "namespace library_version \{.+\}; \/\/ namespace library_version" LANGNES_JSON_VERSION_VARS_CONTENT_MATCH "${LANGNES_JSON_H_CONTENT}")

    if(NOT DEFINED LANGNES_JSON_VERSION_MAJOR)
        string(REGEX MATCH "major{([0-9]+)};" LANGNES_JSON_VERSION_MAJOR_MATCH "${LANGNES_JSON_VERSION_VARS_CONTENT_MATCH}")
        set(LANGNES_JSON_VERSION_MAJOR "${CMAKE_MATCH_1}")
    endif()

    if(NOT DEFINED LANGNES_JSON_VERSION_MINOR)
        string(REGEX MATCH "minor{([0-9]+)};" LANGNES_JSON_VERSION_MINOR_MATCH "${LANGNES_JSON_VERSION_VARS_CONTENT_MATCH}")
        set(LANGNES_JSON_VERSION_MINOR "${CMAKE_MATCH_1}")
    endif()

    if(NOT DEFINED LANGNES_JSON_VERSION_PATCH)
        string(REGEX MATCH "patch{([0-9]+)};" LANGNES_JSON_VERSION_PATCH_MATCH "${LANGNES_JSON_VERSION_VARS_CONTENT_MATCH}")
        set(LANGNES_JSON_VERSION_PATCH "${CMAKE_MATCH_1}")
    endif()

    if(NOT DEFINED LANGNES_JSON_VERSION_PRE_RELEASE)
        string(REGEX MATCH "pre_release{\"([^\"]*)\"};"  LANGNES_JSON_VERSION_PRE_RELEASE_MATCH "${LANGNES_JSON_VERSION_VARS_CONTENT_MATCH}")
        set(LANGNES_JSON_VERSION_PRE_RELEASE "${CMAKE_MATCH_1}")
    endif()

    if(NOT DEFINED LANGNES_JSON_VERSION_BUILD_METADATA)
        string(REGEX MATCH "build_metadata{\"([^\"]*)\"};" LANGNES_JSON_VERSION_BUILD_METADATA_MATCH "${LANGNES_JSON_VERSION_VARS_CONTENT_MATCH}")
        set(LANGNES_JSON_VERSION_BUILD_METADATA "${CMAKE_MATCH_1}")
    endif()

    set(LANGNES_JSON_VERSION_NUMBER "${LANGNES_JSON_VERSION_MAJOR}.${LANGNES_JSON_VERSION_MINOR}.${LANGNES_JSON_VERSION_PATCH}")
    set(LANGNES_JSON_VERSION "${LANGNES_JSON_VERSION_NUMBER}${LANGNES_JSON_VERSION_PRE_RELEASE}${LANGNES_JSON_VERSION_BUILD_METADATA}")

    # Version 0.x of the library can't guarantee backward compatibility.
    if(LANGNES_JSON_VERSION_NUMBER VERSION_LESS 1.0)
        set(LANGNES_JSON_VERSION_COMPATIBILITY "${LANGNES_JSON_VERSION_MAJOR}.${LANGNES_JSON_VERSION_MINOR}")
    else()
        set(LANGNES_JSON_VERSION_COMPATIBILITY "${LANGNES_JSON_VERSION_NUMBER}")
    endif()
endmacro()

# Add CMake options.
macro(langnes_json_add_options)
    option(LANGNES_JSON_BUILD_ALL "Build extras (docs, examples, tests, package)" "${LANGNES_JSON_IS_TOP_LEVEL_BUILD}")
    option(LANGNES_JSON_BUILD_LIBRARY "Build library" "${LANGNES_JSON_BUILD_ALL}")
    option(LANGNES_JSON_BUILD_DOCS "Build documentation" "${LANGNES_JSON_BUILD_ALL}")
    option(LANGNES_JSON_BUILD_EXAMPLES "Build examples" "${LANGNES_JSON_BUILD_ALL}")
    option(LANGNES_JSON_BUILD_TESTS "Build tests" "${LANGNES_JSON_BUILD_ALL}")
    option(LANGNES_JSON_INSTALL "Generate installation rules" "${LANGNES_JSON_IS_TOP_LEVEL_BUILD}")
    option(LANGNES_JSON_PACKAGE "Enable packaging" "${LANGNES_JSON_INSTALL}")
endmacro()

# Call this before project().
macro(langnes_json_init_pre_project)
    if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(LANGNES_JSON_IS_TOP_LEVEL_BUILD TRUE)
    endif()

    if(LANGNES_JSON_IS_TOP_LEVEL_BUILD)
        langnes_json_set_default_build_type()

        # CMAKE_OSX_* should be set prior to the first project() or enable_language()
        # Target macOS version
        set(CMAKE_OSX_DEPLOYMENT_TARGET 10.9 CACHE STRING "")
    endif()

    langnes_json_extract_version()
endmacro()

# Call this after project().
macro(langnes_json_init_post_project)
    langnes_json_add_options()

    if(LANGNES_JSON_IS_TOP_LEVEL_BUILD)
        include(GNUInstallDirs)
        include(CMakePackageConfigHelpers)
        include(CTest)

        langnes_json_target_add_profile_build_type()
        langnes_json_target_set_install_rpath()
        langnes_json_set_language_standards()

        # Hide symbols by default
        set(CMAKE_CXX_VISIBILITY_PRESET hidden)
        set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

        # Useful to build tools and clang-tidy
        set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    endif()
endmacro()
