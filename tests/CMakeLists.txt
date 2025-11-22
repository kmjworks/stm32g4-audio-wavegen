cmake_minimum_required(VERSION 3.22)
project(tests)


set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(UNIT_TEST_PATH ${CMAKE_CURRENT_SOURCE_DIR}/unit)

find_package(GTest REQUIRED)
find_package(fmt REQUIRED)

enable_testing()

function(unit_test executableNameInUnitWithoutExtension)
    add_executable(${executableNameInUnitWithoutExtension} ${UNIT_TEST_PATH}/${executableNameInUnitWithoutExtension}.cpp)
    target_link_libraries(${executableNameInUnitWithoutExtension} PUBLIC GTest::gtest_main ${ARGN})
    target_compile_features(${executableNameInUnitWithoutExtension} PUBLIC cxx_std_20)
endfunction()


unit_test(test_ringBuffer utils fmt::fmt)

include(GoogleTest)
gtest_discover_tests(test_ringBuffer)