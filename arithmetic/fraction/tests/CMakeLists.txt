# fraction/tests/CMakeLists.txt
cmake_minimum_required(VERSION 3.21)
project(mp_os_tests_fractions)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(
        mp_os_tests_fractions
        tests_fractions.cpp
)

target_include_directories(
        mp_os_tests_fractions
        PRIVATE
        ../include
        ../../big_integer/include
)

target_link_libraries(
        mp_os_tests_fractions
        PRIVATE
        gtest_main
        mp_os_arthmtc_frctn
        mp_os_cmmn
        mp_os_lggr_clnt_lggr
        mp_os_arthmtc_bg_intgr
)

set_target_properties(
        mp_os_tests_fractions PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)