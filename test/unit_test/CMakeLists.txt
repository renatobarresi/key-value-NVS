################################################
#             testSuite CMakeLists.txt                   
################################################

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_BUILD_TYPE Debug)

set(this unitTests)
set(sourceDirectory ${CMAKE_CURRENT_SOURCE_DIR}/../../source)

set(sources
    tests.cpp
    ${sourceDirectory}/hardware/mx25_mock/src/mx25_flash_driver_mock.c
    ${sourceDirectory}/app/src/map.c
    ${sourceDirectory}/app/src/storage.c
)

set(includes
    ${sourceDirectory}/hardware/mx25_mock/inc/      
    ${sourceDirectory}/app/inc/
)

add_executable(${this}
    ${sources}
)

target_include_directories(${this} PRIVATE
    ${includes}
)

target_link_libraries(${this} PUBLIC
    gtest_main
)