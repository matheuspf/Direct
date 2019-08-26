cmake_minimum_required(VERSION 3.5)

set(lib_name direct)

project(${lib_name} VERSION ${lib_name_version} LANGUAGES CXX)

option(${lib_name_upper}_COVERAGE "If coverage tests will be executed" ON)

add_executable(tests "")

target_include_directories(tests PUBLIC ${PROJECT_SOURCE_DIR})

target_compile_options(tests PRIVATE -std=c++17 -O2)


# if(${lib_name_upper}_COVERAGE AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
#     target_compile_options(tests PRIVATE -g --coverage -fprofile-arcs -ftest-coverage)
#     target_link_libraries(tests PUBLIC -lgcov)
# endif()


target_sources(tests PUBLIC ${PROJECT_SOURCE_DIR}/tests.cpp)

enable_testing()

find_package(GTest QUIET)

if(GTest_FOUND)
   target_link_libraries(tests PUBLIC GTest::GTest GTest::Main)
endif()

add_test(allTests tests)