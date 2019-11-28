# CMake 3.12.0 CPack uses the project version instead of requiring a variable to be set
cmake_minimum_required(VERSION 3.12.0)

project(GSL LANGUAGES CXX VERSION 0.0.1)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(CTest)

option(GSL_CMAKE_CXX_CLANG_TIDY OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

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
target_include_directories(
    gsl
        INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/GSL/include>
)

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

################
# Installation #
################

add_library(GSL::gsl ALIAS gsl)

set(GSL_CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/GSL")

configure_package_config_file(
    ${CMAKE_CURRENT_LIST_DIR}/CMake/GSLConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/GSLConfig.cmake
    INSTALL_DESTINATION
        ${GSL_CMAKE_CONFIG_DESTINATION}
)

install(TARGETS gsl EXPORT GSLTargets DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(
    EXPORT
        GSLTargets
    NAMESPACE
        GSL::
    DESTINATION
        ${GSL_CMAKE_CONFIG_DESTINATION}
)

set(GSL_CMAKE_SIZEOF_VOID_P ${CMAKE_SIZEOF_VOID_P})
set(CMAKE_SIZEOF_VOID_P "")
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/GSLConfigVersion.cmake"
    COMPATIBILITY
        SameMajorVersion
)
set(CMAKE_SIZEOF_VOID_P ${GSL_CMAKE_SIZEOF_VOID_P})

install(DIRECTORY "include/" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/GSL")

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/GSLConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/GSLConfigVersion.cmake"
    DESTINATION
        ${GSL_CMAKE_CONFIG_DESTINATION}
)

set(
    PKGCONFIG_INSTALL_DIR
        ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig
    CACHE PATH "Path to installed gsl.pc"
)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/CMake/gsl.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gsl.pc
    @ONLY
)
install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/gsl.pc"
    DESTINATION
        ${PKGCONFIG_INSTALL_DIR}
)

set(CPACK_PACKAGE_CONTACT "https://github.com/zhu48/GSL/")
include(CPack)
