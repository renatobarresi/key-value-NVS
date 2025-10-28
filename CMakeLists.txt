cmake_minimum_required(VERSION 3.28)
set(CMAKE_BUILD_TYPE Debug)

# set(CMAKE_C_STANDARD C99)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(this resilientMap)
set(projectPath source/)

include(cmake/CPM.cmake)
CPMAddPackage(
    NAME googletest
    GITHUB_REPOSITORY google/googletest
    GIT_TAG v1.15.2
    DOWNLOAD_ONLY ON
)

enable_testing()
add_subdirectory(build/_deps/googletest-src/)
add_subdirectory(test/unit_test/)

add_executable(${this}
               ${projectPath}/app/src/main.c
               ${projectPath}/app/src/map.c
               ${projectPath}/app/src/storage.c
               ${projectPath}/hardware/mx25_mock/src/mx25_flash_driver_mock.c
)

target_include_directories(${this} PRIVATE
                ${projectPath}/app/inc/
                ${projectPath}/hardware/mx25_mock/inc/
)