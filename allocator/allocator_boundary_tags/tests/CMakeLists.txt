add_executable(
        mp_os_allctr_allctr_bndr_tgs_tests
        allocator_boundary_tags_tests.cpp)

target_link_libraries(
        mp_os_allctr_allctr_bndr_tgs_tests
        PRIVATE
        gtest_main)
target_link_libraries(
        mp_os_allctr_allctr_bndr_tgs_tests
        PRIVATE
        mp_os_lggr_clnt_lggr)
target_link_libraries(
        mp_os_allctr_allctr_bndr_tgs_tests
        PRIVATE
        mp_os_allctr_allctr_bndr_tgs)