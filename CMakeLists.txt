cmake_minimum_required(VERSION 2.8)

if (${CMAKE_CURRENT_SOURCE_DIR} EQUAL ${CMAKE_SOURCE_DIR})
    message(FATAL_ERROR "GSL must be a subdirectory")
endif()

add_library(gsl INTERFACE)
target_include_directories(gsl INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
