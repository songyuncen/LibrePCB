set(SEXPRESSO_SUBMODULE_BASEPATH "${PROJECT_SOURCE_DIR}/libs/sexpresso")
if(EXISTS "${SEXPRESSO_SUBMODULE_BASEPATH}")
    message(STATUS "Using vendored Sexpresso")

    # Add library
    # TODO: Move sources into subdirectory
    add_library(sexpresso STATIC
        "${SEXPRESSO_SUBMODULE_BASEPATH}/sexpresso/sexpresso.cpp"
        "${SEXPRESSO_SUBMODULE_BASEPATH}/sexpresso/sexpresso.hpp"
    )
    target_include_directories(sexpresso PUBLIC
        "${SEXPRESSO_SUBMODULE_BASEPATH}"
    )

    # Alias lib to namespaced variant
    add_library(Sexpresso::Sexpresso ALIAS sexpresso)

    # Stop here, we're done
    return()
endif()

message(FATAL_ERROR "Did not find Sexpresso library submodule, did you clone recursively?")
