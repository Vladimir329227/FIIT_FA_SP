add_executable(
        mp_os_assctv_cntnr_srch_tr_indxng_tr_b_pls_tr_tests
        b_plus_tree_tests.cpp)

target_link_libraries(
        mp_os_assctv_cntnr_srch_tr_indxng_tr_b_pls_tr_tests
        PRIVATE
        gtest_main)
target_link_libraries(
        mp_os_assctv_cntnr_srch_tr_indxng_tr_b_pls_tr_tests
        PRIVATE
        mp_os_assctv_cntnr_srch_tr_indxng_tr_b_pls_tr)