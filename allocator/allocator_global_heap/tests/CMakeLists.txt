add_executable(
        mp_os_allctr_allctr_glbl_hp_tests
        allocator_global_heap_tests.cpp)

target_link_libraries(
        mp_os_allctr_allctr_glbl_hp_tests
        PRIVATE
        gtest_main)
target_link_libraries(
        mp_os_allctr_allctr_glbl_hp_tests
        PRIVATE
        mp_os_lggr_clnt_lggr)
target_link_libraries(
        mp_os_allctr_allctr_glbl_hp_tests
        PRIVATE
        mp_os_allctr_allctr_glbl_hp)