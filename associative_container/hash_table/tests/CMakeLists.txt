add_executable(
        mp_os_assctv_cntnr_hsh_tbl_tests
        hash_table_tests.cpp)

target_link_libraries(
        mp_os_assctv_cntnr_hsh_tbl_tests
        PRIVATE
        gtest_main)
target_link_libraries(
        mp_os_assctv_cntnr_hsh_tbl_tests
        PUBLIC
        mp_os_lggr_clnt_lggr)
target_link_libraries(
        mp_os_assctv_cntnr_hsh_tbl_tests
        PUBLIC
        mp_os_assctv_cntnr_hsh_tbl)