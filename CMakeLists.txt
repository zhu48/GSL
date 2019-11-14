cmake_minimum_required(VERSION 2.8)

project(GSL CXX)

include(CTest)

if (${CMAKE_CURRENT_SOURCE_DIR} EQUAL ${CMAKE_SOURCE_DIR} AND NOT ${BUILD_TESTING})
    message(FATAL_ERROR "GSL must be a subdirectory")
endif()

# set the policy to enable inter-procedural optimization if available
if(POLICY CMP0069)
    cmake_policy(SET CMP0069 NEW)
endif()
if(NOT DEFINED CMAKE_INTERPROCEDURAL_OPTIMIZATION)
    include(CheckIPOSupported)
    message(STATUS "Determining inter-procedural optimization support")
    check_ipo_supported(RESULT CMAKE_INTERPROCEDURAL_OPTIMIZATION)
    if(CMAKE_INTERPROCEDURAL_OPTIMIZATION)
        message(STATUS "Inter-procedural optimization enabled")
    else()
        message(STATUS "Inter-procedural optimization disabled")
    endif()
    set(
        CMAKE_INTERPROCEDURAL_OPTIMIZATION
            ${CMAKE_INTERPROCEDURAL_OPTIMIZATION}
        CACHE INTERNAL "" FORCE
    )
endif()

add_library(gsl INTERFACE)
target_include_directories(gsl INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(BUILD_TESTING)
    find_package(Catch2 QUIET)

    if(Catch2_FOUND)
        message(STATUS "Found Catch2, creating test target." )

        add_executable(test_span ${CMAKE_CURRENT_LIST_DIR}/test/test_span.cpp)
        target_link_libraries(test_span gsl)
        target_link_libraries(test_span Catch2::Catch2)

        add_test(NAME test_span COMMAND test_span)
    else()
        message(STATUS "Could not find Catch2, skipping test target." )
    endif()
endif()
