# Enables compile warnings.
macro(langnes_json_target_enable_warnings TARGET VISIBILITY)
    if(MSVC)
        target_compile_options(${TARGET} ${VISIBILITY} /WX /W4)
    else()
        target_compile_options(${TARGET} ${VISIBILITY} -Werror -Wall -Wextra -Wpedantic)
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
    set(LANGNES_JSON_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

    # Extract version information from source code
    file(READ "${LANGNES_JSON_INCLUDE_DIR}/langnes_json/json.hpp" LANGNES_JSON_H_CONTENT)
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

# Installs targets, etc.
macro(langnes_json_install)
    # Install headers
    install(DIRECTORY "${LANGNES_JSON_INCLUDE_DIR}/langnes_json"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        COMPONENT langnes_json_development)

    # Install targets
    install(TARGETS langnes_json_private EXPORT langnes_json_targets)
    install(TARGETS langnes_json_headers EXPORT langnes_json_targets)
    list(APPEND LANGNES_JSON_INSTALL_TARGET_NAMES langnes_json_shared)
    list(APPEND LANGNES_JSON_INSTALL_TARGET_NAMES langnes_json_static)

    install(TARGETS ${LANGNES_JSON_INSTALL_TARGET_NAMES}
        EXPORT langnes_json_targets
        RUNTIME
            DESTINATION "${CMAKE_INSTALL_BINDIR}"
            COMPONENT langnes_json_runtime
        LIBRARY
            DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            COMPONENT langnes_json_runtime
            NAMELINK_COMPONENT langnes_json_development
        ARCHIVE
            DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            COMPONENT langnes_json_development)

    install(EXPORT langnes_json_targets
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/langnes_json"
        NAMESPACE langnes_json::
        FILE langnes_json-targets.cmake
        COMPONENT langnes_json_development)

    export(EXPORT langnes_json_targets FILE "${CMAKE_CURRENT_BINARY_DIR}/langnes_json-targets.cmake")

    # Install package config
    configure_package_config_file(
        langnes_json-config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/langnes_json-config.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/langnes_json"
        NO_SET_AND_CHECK_MACRO
        NO_CHECK_REQUIRED_COMPONENTS_MACRO)

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/langnes_json-config-version.cmake"
        VERSION "${LANGNES_JSON_VERSION_COMPATIBILITY}"
        COMPATIBILITY SameMinorVersion)

    install(
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/langnes_json-config.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/langnes_json-config-version.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/langnes_json"
        COMPONENT langnes_json_development)
endmacro()

# Declare a dependency with FetchContent_Declare.
function(langnes_json_declare_dependency VAR_NAME NAME GIT_REPO GIT_VERSION)
    set(SYSTEM_ARG)
    if(${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.25)
        set(SYSTEM_ARG SYSTEM)
    endif()

    FetchContent_Declare(
        "${NAME}"
        GIT_REPOSITORY "${GIT_REPO}"
        GIT_TAG "${GIT_VERSION}"
        ${SYSTEM_ARG}
    )

    list(APPEND "${VAR_NAME}" "${NAME}")
    set(LANGNES_JSON_DEPENDENCIES "${LANGNES_JSON_DEPENDENCIES}" PARENT_SCOPE)
endfunction()

# Fetch dependencies.
macro(langnes_json_fetch_dependencies)
    if(LANGNES_JSON_IS_TOP_LEVEL_BUILD)
        langnes_json_declare_dependency(
            LANGNES_JSON_DEPENDENCIES
            Catch2
            https://github.com/catchorg/Catch2.git
            b5373dadca40b7edc8570cf9470b9b1cb1934d40 # v3.5.4
        )

        FetchContent_MakeAvailable(${LANGNES_JSON_DEPENDENCIES})

        list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
        include(Catch)
    endif()
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
    if(LANGNES_JSON_IS_TOP_LEVEL_BUILD)
        include(GNUInstallDirs)
        include(CMakePackageConfigHelpers)
        include(CTest)
        include(FetchContent)

        langnes_json_target_add_profile_build_type()
        langnes_json_target_set_install_rpath()
        langnes_json_set_language_standards()

        # Hide symbols by default
        set(CMAKE_CXX_VISIBILITY_PRESET hidden)
        set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

        # Use debug postfix to separate debug/release binaries for the library
        set(CMAKE_DEBUG_POSTFIX d)

        # Useful to build tools and clang-tidy
        set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

        langnes_json_fetch_dependencies()
    endif()
endmacro()
