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

message(STATUS "Project version is: ${PROJECT_VERSION}")
if (WIN32)
    message(FATAL_ERROR "This is made for Unix, not Windows")
endif()
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "CMake build type (what do you think)" FORCE)
endif()
message(STATUS "CMAKE_BUILD_TYPE is set to: ${CMAKE_BUILD_TYPE}")
if (CMAKE_BUILD_TYPE STREQUAL Debug)
    message(WARNING "Note CMAKE_BUILD_TYPE being set to 'Debug' will enable developer options.")
endif()
if(NOT CALL_NAME)
    set(CALL_NAME mdkir CACHE STRING "What its called by to make it configure and not insult" FORCE)
endif()
message(STATUS "CALL_NAME is set to: ${CALL_NAME}")

add_executable(mdkir
    src/array.c
    src/list.c
    src/main.c
)
target_include_directories(mdkir PRIVATE include)
target_compile_definitions(mdkir PRIVATE
    CALL_NAME="${CALL_NAME}"
    HOMEPAGE="${PROJECT_HOMEPAGE_URL}"
    PROJECT_VER="${PROJECT_VERSION}"
)
if (CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_definitions(mdkir PRIVATE
        BINDIR="${CMAKE_BINARY_DIR}/fake_bin"
        NEEDS_SUDO=0
        SYSCONFDIR="${CMAKE_BINARY_DIR}/fake_etc"
    )
    message(STATUS  "Installing symlinks to: ${CMAKE_BINARY_DIR}/fake_bin")
    message(STATUS "Writing config to: ${CMAKE_BINARY_DIR}/fake_etc")

    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/fake_bin")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/fake_etc")
    file(TOUCH "${CMAKE_BINARY_DIR}/fake_etc/list")

    message(AUTHOR_WARNING "Due to my laziness, you will have to manually copy mdkir into fake_bin to test it.")
else()
    target_compile_definitions(mdkir PRIVATE
        BINDIR="${CMAKE_INSTALL_FULL_BINDIR}"
        NEEDS_SUDO=1
        SYSCONFDIR="${CMAKE_INSTALL_FULL_SYSCONFDIR}/mdkir"
    )

    message(STATUS "Installing symlinks to: ${CMAKE_INSTALL_FULL_BINDIR}")
    message(STATUS "Writing config to: ${CMAKE_INSTALL_FULL_SYSCONFDIR}/mdkir")

    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/tmp_install/mdkir")
    file(TOUCH "${CMAKE_BINARY_DIR}/tmp_install/mdkir/list")
endif()

install(TARGETS mdkir
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(CODE "
    if(EXISTS \"${CMAKE_INSTALL_FULL_SYSCONFDIR}/mdkir\")
        message(STATUS \"Up-to-date: ${CMAKE_INSTALL_FULL_SYSCONFDIR}/mdkir\")
    else()
        file(INSTALL
            DESTINATION \"${CMAKE_INSTALL_FULL_SYSCONFDIR}\"
            TYPE DIRECTORY
            FILES \"${CMAKE_BINARY_DIR}/tmp_install/mdkir\"
        )
    endif()
")
