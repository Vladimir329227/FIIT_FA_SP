add_subdirectory(tests)

add_library(
        mp_os_allctr_allctr_rb_tr
        src/allocator_red_black_tree.cpp)

target_include_directories(
        mp_os_allctr_allctr_rb_tr
        PUBLIC
        ./include)

target_link_libraries(
        mp_os_allctr_allctr_rb_tr
        PUBLIC
        mp_os_cmmn)
target_link_libraries(
        mp_os_allctr_allctr_rb_tr
        PUBLIC
        mp_os_lggr_lggr)
target_link_libraries(
        mp_os_allctr_allctr_rb_tr
        PUBLIC
        mp_os_allctr_allctr)