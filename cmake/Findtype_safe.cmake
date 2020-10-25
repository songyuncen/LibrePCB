set(TYPE_SAFE_SUBMODULE_BASEPATH "${PROJECT_SOURCE_DIR}/libs/type_safe")
if(EXISTS "${TYPE_SAFE_SUBMODULE_BASEPATH}")
    message(STATUS "Using vendored type_safe")

    # Include local submodule
    add_subdirectory("${TYPE_SAFE_SUBMODULE_BASEPATH}" build EXCLUDE_FROM_ALL)

    # Alias static lib to namespaced variant
    add_library(type_safe::type_safe ALIAS type_safe)

    # Stop here, we're done
    return()
endif()

message(FATAL_ERROR "Did not find type_safe library submodule, did you clone recursively?")
