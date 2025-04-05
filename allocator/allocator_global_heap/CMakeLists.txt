add_subdirectory(tests)

add_library(
        mp_os_allctr_allctr_glbl_hp
        src/allocator_global_heap.cpp)

target_include_directories(
        mp_os_allctr_allctr_glbl_hp
        PUBLIC
        ./include)

target_link_libraries(
        mp_os_allctr_allctr_glbl_hp
        PUBLIC
        mp_os_cmmn)
target_link_libraries(
        mp_os_allctr_allctr_glbl_hp
        PUBLIC
        mp_os_lggr_lggr)
target_link_libraries(
        mp_os_allctr_allctr_glbl_hp
        PUBLIC
        mp_os_allctr_allctr)