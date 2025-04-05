add_executable(
        mp_os_allctr_allctr_rb_tr_tests
        allocator_red_black_tree_tests.cpp)

target_link_libraries(
        mp_os_allctr_allctr_rb_tr_tests
        PRIVATE
        gtest_main)
target_link_libraries(
        mp_os_allctr_allctr_rb_tr_tests
        PRIVATE
        mp_os_lggr_clnt_lggr)
target_link_libraries(
        mp_os_allctr_allctr_rb_tr_tests
        PRIVATE
        mp_os_allctr_allctr_rb_tr)