add_library(
        mp_os_cmmn
        src/not_implemented.cpp
        src/operation_not_supported.cpp)

target_include_directories(
        mp_os_cmmn
        PUBLIC
        ./include)