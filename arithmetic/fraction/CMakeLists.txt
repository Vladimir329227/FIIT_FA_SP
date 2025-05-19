# fraction/CMakeLists.txt
cmake_minimum_required(VERSION 3.21)
project(mp_os_arthmtc_frctn)

add_library(
        mp_os_arthmtc_frctn
        src/fraction.cpp
)

target_include_directories(
        mp_os_arthmtc_frctn
        PUBLIC
        include
        ../big_integer/include
)

target_link_libraries(
        mp_os_arthmtc_frctn
        PUBLIC
        mp_os_cmmn
        mp_os_allctr_allctr
        mp_os_arthmtc_bg_intgr
)

set_target_properties(
        mp_os_arthmtc_frctn PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)

add_subdirectory(tests)