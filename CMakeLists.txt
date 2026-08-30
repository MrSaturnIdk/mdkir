cmake_minimum_required(VERSION 3.12)
project(mdkir
    VERSION 1.0.0
    DESCRIPTION "The Unix (MacOS/Linux) program that insults you for misspelling a command"
    HOMEPAGE_URL "https://github.com/MrSaturnIdk/mdkir"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()
include(GNUInstallDirs)
add_custom_target(mdkirtests)

if (WIN32)
    message(FATAL_ERROR "This is made for Unix, not Windows")
endif()
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "CMake build type (what do you think)" FORCE)
endif()
message(STATUS "CMAKE_BUILD_TYPE is set to \"${CMAKE_BUILD_TYPE}\"")
if (CMAKE_BUILD_TYPE STREQUAL Debug)
    message(WARNING "Note CMAKE_BUILD_TYPE being set to \"Debug\" will enable developer options.")
endif()
if(NOT CALL_NAME)
    set(CALL_NAME mdkir CACHE STRING "What its called by to make it configure and not insult" FORCE)
endif()
message(STATUS "CALL_NAME is set to \"${CALL_NAME}\"")

add_library(mdkirhelpers STATIC
    src/helpers/list.cpp
)
target_include_directories(mdkirhelpers PUBLIC include)
if (CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_definitions(mdkirhelpers PUBLIC
        SYSCONFDIR="${CMAKE_BINARY_DIR}/fake_etc"
    )

    message(STATUS  "Installing symlinks to \"${CMAKE_BINARY_DIR}/fake_bin\"")
    message(STATUS "Writing config to \"${CMAKE_BINARY_DIR}/fake_etc\"")

    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/fake_bin")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/fake_etc")
    file(TOUCH "${CMAKE_BINARY_DIR}/fake_etc/list")
else()
    target_compile_definitions(mdkirhelpers PUBLIC
        SYSCONFDIR="${CMAKE_INSTALL_FULL_SYSCONFDIR}"
    )

    message(STATUS "Installing symlinks to \"${CMAKE_INSTALL_FULL_BINDIR}\"")
    message(STATUS "Writing config to \"${CMAKE_INSTALL_FULL_SYSCONFDIR}/mdkir\"")

    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/tmp_install/mdkir")
    file(TOUCH "${CMAKE_BINARY_DIR}/tmp_install/mdkir/list")
endif()

add_executable(mdkir
    src/main.cpp
)
target_compile_definitions(mdkir PRIVATE
    CALL_NAME="${CALL_NAME}"
    HOMEPAGE="${PROJECT_HOMEPAGE_URL}"
    PROJECT_VER="${PROJECT_VERSION}"
)
if (CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_definitions(mdkir PRIVATE
        BINDIR="${CMAKE_BINARY_DIR}/fake_bin"
        NEEDS_SUDO=false
    )
    message(AUTHOR_WARNING "Due to my laziness, you will have to manually copy mdkir into fake_bin to test it.")
else()
    target_compile_definitions(mdkir PRIVATE
        BINDIR="${CMAKE_INSTALL_FULL_BINDIR}"
        NEEDS_SUDO=true
    )
endif()
target_link_libraries(mdkir mdkirhelpers)

add_executable(mdkirlisttests tests/list.cpp)
target_link_libraries(mdkirlisttests mdkirhelpers)
add_test(NAME ListTest COMMAND mdkirlisttests)
add_dependencies(mdkirtests mdkirlisttests)

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
