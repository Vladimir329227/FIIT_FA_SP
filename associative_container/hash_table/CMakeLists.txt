add_subdirectory(tests)

add_library(
        mp_os_assctv_cntnr_hsh_tbl
        include/hash_table.h
        src/hhh.cpp)

target_include_directories(
        mp_os_assctv_cntnr_hsh_tbl
        PUBLIC
        ./include)

target_link_libraries(
        mp_os_assctv_cntnr_hsh_tbl
        PUBLIC
        mp_os_lggr_lggr)
target_link_libraries(
        mp_os_assctv_cntnr_hsh_tbl
        PUBLIC
        mp_os_allctr_allctr)
target_link_libraries(
        mp_os_assctv_cntnr_hsh_tbl
        PUBLIC
        mp_os_assctv_cntnr)