include(FindPackageHandleStandardArgs)

function(find_cplex_guess_version CPLEX_BINARY_DIR)
    # hopefully, in the binary directory, there is also the shared library indicating the version!
    file(GLOB CPLEX_FILES_IN_BINARY_DIR_ "${CPLEX_EXECUTABLE_DIR_}/*")

    foreach (F IN LISTS CPLEX_FILES_IN_BINARY_DIR_)
        get_filename_component(fname ${F} NAME)
        if (fname MATCHES "^(lib)?cplex([0-9]+)[^0-9\\.]*\\.(so|dylib|dynlib|dll)$")
            set(CPLEX_VERSION_ ${CMAKE_MATCH_2})
            break()
        endif ()
    endforeach ()

    if (CPLEX_VERSION_)
        if (NOT CPLEX_FIND_QUIETLY)
            message(STATUS "CPLEX_VERSION determined by shared library name: ${CPLEX_VERSION_}")
        endif ()
        set(CPLEX_VERSION ${CPLEX_VERSION_} PARENT_SCOPE)
    endif ()
endfunction(find_cplex_guess_version)

# if CPLEX_ROOT is not given, try to find CPLEX using the binary which hopefully resides in the path
if (NOT CPLEX_ROOT)
    find_program(CPLEX_EXECUTABLE_BINARY_ cplex)

    if (CPLEX_EXECUTABLE_BINARY_)
        # get directory component
        get_filename_component(CPLEX_EXECUTABLE_DIR_ ${CPLEX_EXECUTABLE_BINARY_} DIRECTORY)

        # get CPLEX_ROOT
        string(REGEX MATCH "^(.+)/cplex/bin(.*)$" CPLEX_MATCH_OUTPUT_ ${CPLEX_EXECUTABLE_DIR_})
        if (CPLEX_MATCH_OUTPUT_)
            if (NOT CPLEX_FIND_QUIETLY)
                message(STATUS "CPLEX_ROOT determined by cplex executable: ${CMAKE_MATCH_1}")
            endif ()
            set(CPLEX_ROOT ${CMAKE_MATCH_1})
        endif ()
    endif ()
endif ()

if (NOT CPLEX_ROOT)
    if (NOT CPLEX_FIND_QUIETLY)
        message(STATUS "CPLEX_ROOT is not set. Finding CPLEX will most likely fail.")
    endif ()
endif ()

if (NOT CPLEX_VERSION AND CPLEX_ROOT)
    find_cplex_guess_version("${CPLEX_ROOT}/cplex/bin")
endif ()

if (NOT CPLEX_VERSION)
    if (NOT CPLEX_FIND_QUIETLY)
        message(STATUS "CPLEX_VERSION is not set. Finding CPLEX will most likely fail.")
    endif ()
endif ()

find_path(
        ILCPLEX_INCLUDE_DIR_
        NAMES
        ilcplex/cplex.h
        HINTS
        "${CPLEX_ROOT}/include"
        "${CPLEX_ROOT}/cplex/include"
        PATH_SUFFIXES
        cplex
)

if (NOT ILCPLEX_INCLUDE_DIR_)
    if (NOT CPLEX_FIND_QUIETLY)
        message(STATUS "Could not find include directory for ilcplex/cplex.h")
    endif ()
else ()
    set(CPLEX_INCLUDE_DIRS_ ${ILCPLEX_INCLUDE_DIR_})
    if (NOT CPLEX_FIND_QUIETLY)
        message(STATUS "CPLEX include directory for ilcplex/cplex.h: ${ILCPLEX_INCLUDE_DIR_}")
    endif ()
endif ()

find_path(
        ILCONCERT_INCLUDE_DIR_
        NAMES
        ilconcert/iloenv.h
        HINTS
        ${ILCPLEX_INCLUDE_DIR_}
        "${CPLEX_ROOT}/include"
        "${CPLEX_ROOT}/concert/include"
        PATH_SUFFIXES
        cplex
        concert
)

if (NOT ILCONCERT_INCLUDE_DIR_)
    if (NOT CPLEX_FIND_QUIETLY)
        message(STATUS "Could not find directory for ilconcert/iloenv.h")
    endif ()
else ()
    if (ILCPLEX_INCLUDE_DIR_ STREQUAL ILCONCERT_INCLUDE_DIR_)
        if (NOT CPLEX_FIND_QUIETLY)
            message(STATUS "CPLEX include directory for ilconcert/iloenv.h matches.")
        endif ()
    else ()
        set(CPLEX_INCLUDE_DIRS_ ${CPLEX_INCLUDE_DIRS_} ${ILCONCERT_INCLUDE_DIR_})
        if (NOT CPLEX_FIND_QUIETLY)
            message(STATUS "CPLEX include directory for ilconcert/iloenv.h: ${ILCONCERT_INCLUDE_DIR_}")
        endif ()
    endif ()
endif ()

if (MSVC)
    # yes, people still build for 32-bit.
    if ("${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
        set(CPLEX_MSVC_ARCHITECTURE_ "x64_windows_vs")
    else ()
        set(CPLEX_MSVC_ARCHITECTURE_ "x86_windows_vs")
    endif ()

    # make sure to use the right libs
    if ("${MSVC_VERSION}" STREQUAL "1600")
        set(CPLEX_MSVC_VARIANT_ "${CPLEX_MSVC_ARCHITECTURE_}2010")
    elseif ("${MSVC_VERSION}" STREQUAL "1700")
        set(CPLEX_MSVC_VARIANT_ "${CPLEX_MSVC_ARCHITECTURE_}2012")
    elseif ("${MSVC_VERSION}" STREQUAL "1800")
        set(CPLEX_MSVC_VARIANT_ "${CPLEX_MSVC_ARCHITECTURE_}2013")
    else ()
        if (NOT CPLEX_FIND_QUIETLY)
            message(STATUS "This version (MSVC_VERSION: ${MSVC_VERSION}) is not supported by CPLEX (currently, only VS2010-2013 are supported)!")
        endif ()
    endif ()

    if (NOT CPLEX_FIND_QUIETLY)
        message(STATUS "MSVC variant detected: ${CPLEX_MSVC_VARIANT_}")
    endif ()
elseif (WIN32)
    if (NOT CPLEX_FIND_QUIETLY)
        message(STATUS "This compiler is probably not supported by CPLEX!")
    endif ()
endif ()

if (CPLEX_MSVC_VARIANT_)
    set(CPLEX_MSVC_HINTDIR_RELEASE_ "${CPLEX_ROOT}/cplex/lib/${CPLEX_MSVC_VARIANT_}/stat_mda")
    set(CONCERT_MSVC_HINTDIR_RELEASE_ "${CPLEX_ROOT}/concert/lib/${CPLEX_MSVC_VARIANT_}/stat_mda")
    set(CPLEX_MSVC_HINTDIR_DEBUG_ "${CPLEX_ROOT}/cplex/lib/${CPLEX_MSVC_VARIANT_}/stat_mdd")
    set(CONCERT_MSVC_HINTDIR_DEBUG_ "${CPLEX_ROOT}/concert/lib/${CPLEX_MSVC_VARIANT_}/stat_mdd")
endif ()

if (CPLEX_MSVC_VARIANT_)
    find_library(
            ILOCPLEX_LIBRARY_DEBUG_
            NAMES
            ilocplex
            HINTS
            ${CPLEX_MSVC_HINTDIR_DEBUG_}
    )

    find_library(
            ILOCPLEX_LIBRARY_RELEASE_
            NAMES
            ilocplex
            HINTS
            ${CPLEX_MSVC_HINTDIR_RELEASE_}
    )

    if (ILOCPLEX_LIBRARY_DEBUG_)
        set(ILOCPLEX_LIBRARY_ "debug" ${ILOCPLEX_LIBRARY_DEBUG_})
    endif ()

    if (ILOCPLEX_LIBRARY_RELEASE_)
        set(ILOCPLEX_LIBRARY_ ${ILOCPLEX_LIBRARY_} "optimized" ${ILOCPLEX_LIBRARY_RELEASE_})
    endif ()
else ()
    find_library(
            ILOCPLEX_LIBRARY_
            NAMES
            ilocplex
            HINTS
            "${CPLEX_ROOT}/cplex"
            "${CPLEX_ROOT}/cplex/lib"
            "${CPLEX_ROOT}/cplex/lib/x86-64_linux/static_pic"
            "${CPLEX_ROOT}/cplex/lib/x86-64_osx/static_pic"
            "${CPLEX_ROOT}/cplex/lib/x86-64_darwin/static_pic"
            "${CPLEX_ROOT}/cplex/lib/x86-64_sles10_4.1/static_pic"
            "$ENV{LIBRARY_PATH}"
            "$ENV{LD_LIBRARY_PATH}"
    )
endif ()

if (NOT ILOCPLEX_LIBRARY_)
    if (NOT CPLEX_FIND_QUIETLY)
        message(STATUS "Could not find library ilocplex!")
    endif ()
else ()
    if (NOT CPLEX_FIND_QUIETLY)
        message(STATUS "CPLEX library ilocplex: ${ILOCPLEX_LIBRARY_}")
    endif ()
    set(CPLEX_LIBRARIES_ ${ILOCPLEX_LIBRARY_})
endif ()

if (CPLEX_MSVC_VARIANT_)
    find_library(
            CPLEX_A_LIBRARY_DEBUG_
            NAMES
            cplex
            cplex${CPLEX_VERSION}
            HINTS
            ${CPLEX_MSVC_HINTDIR_DEBUG_}
    )

    find_library(
            CPLEX_A_LIBRARY_RELEASE_
            NAMES
            cplex
            cplex${CPLEX_VERSION}
            HINTS
            ${CPLEX_MSVC_HINTDIR_RELEASE_}
    )

    if (CPLEX_A_LIBRARY_DEBUG_)
        set(CPLEX_A_LIBRARY_ "debug" ${CPLEX_A_LIBRARY_DEBUG_})
    endif ()

    if (CPLEX_A_LIBRARY_RELEASE_)
        set(CPLEX_A_LIBRARY_ ${CPLEX_A_LIBRARY_} "optimized" ${CPLEX_A_LIBRARY_RELEASE_})
    endif ()
else ()
    find_library(
            CPLEX_A_LIBRARY_
            NAMES
            cplex
            cplex${CPLEX_VERSION}
            HINTS
            "${CPLEX_ROOT}/cplex"
            "${CPLEX_ROOT}/cplex/lib"
            "${CPLEX_ROOT}/cplex/lib/x86-64_linux/static_pic"
            "${CPLEX_ROOT}/cplex/lib/x86-64_osx/static_pic"
            "${CPLEX_ROOT}/cplex/lib/x86-64_darwin/static_pic"
            "${CPLEX_ROOT}/cplex/lib/x86-64_sles10_4.1/static_pic"
            "$ENV{LIBRARY_PATH}"
            "$ENV{LD_LIBRARY_PATH}"
    )
endif ()

if (NOT CPLEX_A_LIBRARY_)
    if (NOT CPLEX_FIND_QUIETLY)
        message(STATUS "Could not find static library cplex!")
    endif ()
else ()
    if (NOT CPLEX_FIND_QUIETLY)
        message(STATUS "CPLEX static library: ${CPLEX_A_LIBRARY_}")
    endif ()
    set(CPLEX_LIBRARIES_ ${CPLEX_LIBRARIES_} ${CPLEX_A_LIBRARY_})
endif ()

if (CPLEX_MSVC_VARIANT_)
    find_library(
            ILCONCERT_LIBRARY_DEBUG_
            NAMES
            concert
            HINTS
            ${CONCERT_MSVC_HINTDIR_DEBUG_}
    )

    find_library(
            ILCONCERT_LIBRARY_RELEASE_
            NAMES
            concert
            HINTS
            ${CONCERT_MSVC_HINTDIR_RELEASE_}
    )

    if (ILCONCERT_LIBRARY_DEBUG_)
        set(ILCONCERT_LIBRARY_ "debug" ${ILCONCERT_LIBRARY_DEBUG_})
    endif ()

    if (ILCONCERT_LIBRARY_RELEASE_)
        set(ILCONCERT_LIBRARY_ ${ILCONCERT_LIBRARY_} "optimized" ${ILCONCERT_LIBRARY_RELEASE_})
    endif ()
else ()
    find_library(
            ILCONCERT_LIBRARY_
            NAMES
            concert
            HINTS
            "${CPLEX_ROOT}/concert"
            "${CPLEX_ROOT}/concert/lib"
            "${CPLEX_ROOT}/concert/lib/x86-64_linux/static_pic"
            "${CPLEX_ROOT}/concert/lib/x86-64_osx/static_pic"
            "${CPLEX_ROOT}/concert/lib/x86-64_darwin"
            "${CPLEX_ROOT}/concert/lib/x86-64_darwin/static_pic"
            "${CPLEX_ROOT}/concert/lib/x86-64_sles10_4.1/static_pic"
            "$ENV{LIBRARY_PATH}"
            "$ENV{LD_LIBRARY_PATH}"
    )
endif ()

if (NOT ILCONCERT_LIBRARY_)
    if (NOT CPLEX_FIND_QUIETLY)
        message(STATUS "Could not find library concert!")
    endif ()
else ()
    if (NOT CPLEX_FIND_QUIETLY)
        message(STATUS "CPLEX library concert: ${ILCONCERT_LIBRARY_}")
    endif ()
    set(CPLEX_LIBRARIES_ ${CPLEX_LIBRARIES_} ${ILCONCERT_LIBRARY_})
endif ()

# This library is not really present in all distributions; whether its needed depends on the system
if (NOT CPLEX_MSVC_VARIANT_)
    find_library(
            CPLEX_SO_LIBRARY_
            NAMES
            cplex${CPLEX_VERSION}
            HINTS
            "${CPLEX_ROOT}/cplex"
            "${CPLEX_ROOT}/cplex/bin"
            "${CPLEX_ROOT}/cplex/bin/x86-64_linux"
            "${CPLEX_ROOT}/cplex/bin/x86-64_osx"
            "${CPLEX_ROOT}/cplex/bin/x86-64_darwin"
            "${CPLEX_ROOT}/cplex/lib"
            "${CPLEX_ROOT}/cplex/lib/x86-64_linux/static_pic"
            "${CPLEX_ROOT}/cplex/lib/x86-64_osx/static_pic"
            "${CPLEX_ROOT}/cplex/lib/x86-64_darwin/static_pic"
            "${CPLEX_ROOT}/cplex/lib/x86-64_sles10_4.1/static_pic"
            "$ENV{LIBRARY_PATH}"
            "$ENV{LD_LIBRARY_PATH}"
    )
endif ()

if (NOT CPLEX_SO_LIBRARY_)
    if (NOT CPLEX_FIND_QUIETLY)
        message(STATUS "Could not find library libcplex${CPLEX_VERSION} - assuming it is not needed!")
    endif ()
else ()
    if (NOT CPLEX_FIND_QUIETLY)
        message(STATUS "CPLEX library libcplex${CPLEX_VERSION}: ${CPLEX_SO_LIBRARY_}")
    endif ()
    set(CPLEX_LIBRARIES_ ${CPLEX_LIBRARIES_} ${CPLEX_SO_LIBRARY_})
endif ()

find_package_handle_standard_args(
        CPLEX
        DEFAULT_MSG
        CPLEX_ROOT
        ILCPLEX_INCLUDE_DIR_
        ILCONCERT_INCLUDE_DIR_
        ILOCPLEX_LIBRARY_
        ILCONCERT_LIBRARY_
        CPLEX_A_LIBRARY_
)

set(CPLEX_LIBRARIES ${CPLEX_LIBRARIES_})
set(CPLEX_INCLUDE_DIRS ${CPLEX_INCLUDE_DIRS_})

mark_as_advanced(
        CPLEX_CMDLINE_PROG_
        ILCPLEX_INCLUDE_DIR_
        ILCONCERT_INCLUDE_DIR_
        ILOCPLEX_LIBRARY_
        ILCONCERT_LIBRARY_
        CPLEX_A_LIBRARY_
        CPLEX_SO_LIBRARY_
        CPLEX_LIBRARIES_
        CPLEX_INCLUDE_DIRS_
)

