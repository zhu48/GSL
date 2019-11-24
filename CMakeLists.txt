# CMake 3.0.0 uses the new project() syntax
cmake_minimum_required(VERSION 3.0.0)

project(GSL LANGUAGES CXX VERSION 0.0.1)

option(GSL_CMAKE_CXX_CLANG_TIDY OFF)

if(${GSL_CMAKE_CXX_CLANG_TIDY} AND ${BUILD_TESTING})
    find_program(GSL_CLANG_TIDY_FIND_RESULT "clang-tidy")
    if(EXISTS ${GSL_CLANG_TIDY_FIND_RESULT})
        message(STATUS "Enabling clang-tidy")

        set(CMAKE_CXX_CLANG_TIDY "${GSL_CLANG_TIDY_FIND_RESULT};-checks=-*")

        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},bugprone-*")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},cppcoreguidelines-*")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},clang-analyzer-*")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},misc-*")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},modernize-*")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},performance-*")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},portability-*")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},readability-*")

    else()
        message(STATUS "clang-tidy requested, but no clang-tidy installation found")
    endif()
endif()

add_library(gsl INTERFACE)
target_compile_features(gsl INTERFACE cxx_std_17)
target_include_directories(gsl INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

include(CTest)

if(BUILD_TESTING)
    find_package(Catch2 REQUIRED)

    message(STATUS "Found Catch2, creating test target")

    add_executable(test_span ${CMAKE_CURRENT_LIST_DIR}/test/test_span.cpp)
    target_link_libraries(test_span gsl)
    target_link_libraries(test_span Catch2::Catch2)

    add_test(
        NAME
            test_span
        COMMAND
            test_span --out ${CMAKE_CURRENT_LIST_DIR}/test/test_span.catch2log
    )
endif()
