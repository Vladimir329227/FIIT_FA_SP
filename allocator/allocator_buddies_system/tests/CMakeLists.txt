add_executable(
        mp_os_allctr_allctr_bdds_sstm_tests
        allocator_buddies_system_tests.cpp)

target_link_libraries(
        mp_os_allctr_allctr_bdds_sstm_tests
        PRIVATE
        gtest_main)
target_link_libraries(
        mp_os_allctr_allctr_bdds_sstm_tests
        PRIVATE
        mp_os_lggr_clnt_lggr)
target_link_libraries(
        mp_os_allctr_allctr_bdds_sstm_tests
        PRIVATE
        mp_os_allctr_allctr_bdds_sstm)