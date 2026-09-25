cmake_minimum_required(VERSION 3.12)
project(mdkir
    VERSION 1.1.0
    DESCRIPTION "The Unix (MacOS/Linux) program that insults you for misspelling a command"
    HOMEPAGE_URL "https://github.com/MrSaturnIdk/mdkir"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include(GNUInstallDirs)

option(MDKIR_DEBUG "Debug options on or not" OFF)
if(MDKIR_DEBUG)
    message(WARNING "Debug options are on")
endif()
message(STATUS "Project version is: ${PROJECT_VERSION}")
if (WIN32)
    message(FATAL_ERROR "This is made for Unix, not Windows")
endif()
if(NOT CMAKE_BUILD_TYPE)
    if(MDKIR_DEBUG)
        set(CMAKE_BUILD_TYPE Debug CACHE STRING "CMake build type (what do you think)" FORCE)
    else()
        set(CMAKE_BUILD_TYPE Release CACHE STRING "CMake build type (what do you think)" FORCE)
    endif()
endif()
message(STATUS "CMAKE_BUILD_TYPE is set to: ${CMAKE_BUILD_TYPE}")
if(DEFINED NEEDS_SUDO)
    message(DEPRECATION "This is deprecated, use 'MDKIR_NEEDS_SUDO' instead")
    set(MDKIR_NEEDS_SUDO ${NEEDS_SUDO} CACHE STRING "If you need sudo to edit commands" FORCE)
elseif(NOT DEFINED MDKIR_NEEDS_SUDO)
    if(MDKIR_DEBUG)
        set(MDKIR_NEEDS_SUDO 0 CACHE STRING "If you need sudo to edit commands" FORCE)
    else()
        set(MDKIR_NEEDS_SUDO 1 CACHE STRING "If you need sudo to edit commands" FORCE)
    endif()
endif()
message(STATUS "MDKIR_NEEDS_SUDO is set to: ${MDKIR_NEEDS_SUDO}")
if(CALL_NAME)
    message(DEPRECATION "This is deprecated, use 'MDKIR_CALL_NAME' instead")
    set(MDKIR_CALL_NAME "${CALL_NAME}" CACHE STRING "What its called by to make it configure and not insult" FORCE)
elseif(NOT MDKIR_CALL_NAME)
    set(MDKIR_CALL_NAME "mdkir" CACHE STRING "What its called by to make it configure and not insult" FORCE)
endif()
string(ASCII 14 CH_FF)
string(ASCII 13 CH_VT)
if("${MDKIR_CALL_NAME}" MATCHES "[ ${CH_FF}\n\r\t${CH_VT}]")
    message(SEND_ERROR "MDKIR_CALL_NAME cannot contain whitespace")
endif()
message(STATUS "MDKIR_CALL_NAME is set to: ${MDKIR_CALL_NAME}")
if(MDKIR_DEBUG)
    set(MDKIR_BINDIR "${CMAKE_BINARY_DIR}")
    set(MDKIR_CONFIGDIR "${CMAKE_BINARY_DIR}")
    file(TOUCH "${CMAKE_BINARY_DIR}/list")
else()
    set(MDKIR_BINDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")
    set(MDKIR_CONFIGDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_SYSCONFDIR}/mdkir")
endif()
message(STATUS "Installing symlinks to: ${MDKIR_BINDIR}")
message(STATUS "Writing config to: ${MDKIR_CONFIGDIR}")

add_executable(mdkir
    src/utils/array.c
    src/utils/string.c
    src/list.c
    src/main.c
)
target_include_directories(mdkir PRIVATE include)
target_compile_definitions(mdkir PRIVATE
    CALL_NAME="${MDKIR_CALL_NAME}"
    HOMEPAGE="${PROJECT_HOMEPAGE_URL}"
    PROJECT_VER="${PROJECT_VERSION}"
    BINDIR="${MDKIR_BINDIR}"
    NEEDS_SUDO=${MDKIR_NEEDS_SUDO}
    SYSCONFDIR="${MDKIR_CONFIGDIR}"
)
set_target_properties(mdkir PROPERTIES
    OUTPUT_NAME "${MDKIR_CALL_NAME}"
)

install(TARGETS mdkir
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(CODE "
    if(EXISTS \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_SYSCONFDIR}/mdkir\")
        message(STATUS \"Up-to-date: \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_SYSCONFDIR}/mdkir\")
    else()
        file(MAKE_DIRECTORY \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_SYSCONFDIR}/mdkir\")
        file(TOUCH \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_SYSCONFDIR}/mdkir/list\")
        message(STATUS \"Installing: \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_SYSCONFDIR}/mdkir\")
    endif()
")
