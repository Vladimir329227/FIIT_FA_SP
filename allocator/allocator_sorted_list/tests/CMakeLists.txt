add_executable(
        mp_os_allctr_allctr_srtd_lst_tests
        allocator_sorted_list_tests.cpp)

target_link_libraries(
        mp_os_allctr_allctr_srtd_lst_tests
        PRIVATE
        gtest_main)
target_link_libraries(
        mp_os_allctr_allctr_srtd_lst_tests
        PRIVATE
        mp_os_lggr_clnt_lggr)
target_link_libraries(
        mp_os_allctr_allctr_srtd_lst_tests
        PRIVATE
        mp_os_allctr_allctr_srtd_lst)