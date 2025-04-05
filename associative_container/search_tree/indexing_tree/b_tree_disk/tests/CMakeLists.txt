add_executable(
        mp_os_assctv_cntnr_srch_tr_indxng_tr_b_tr_dsk_tests
        b_tree_disk_tests.cpp)

target_link_libraries(
        mp_os_assctv_cntnr_srch_tr_indxng_tr_b_tr_dsk_tests
        PRIVATE
        gtest_main)
target_link_libraries(
        mp_os_assctv_cntnr_srch_tr_indxng_tr_b_tr_dsk_tests
        PRIVATE
        mp_os_lggr_clnt_lggr)
target_link_libraries(
        mp_os_assctv_cntnr_srch_tr_indxng_tr_b_tr_dsk_tests
        PRIVATE
        mp_os_assctv_cntnr_srch_tr_indxng_tr_b_tr_dsk)