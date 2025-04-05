add_subdirectory(tests)

add_library(
        mp_os_allctr_allctr_bndr_tgs
        src/allocator_boundary_tags.cpp)

target_include_directories(
        mp_os_allctr_allctr_bndr_tgs
        PUBLIC
        ./include)

target_link_libraries(
        mp_os_allctr_allctr_bndr_tgs
        PUBLIC
        mp_os_cmmn)
target_link_libraries(
        mp_os_allctr_allctr_bndr_tgs
        PUBLIC
        mp_os_lggr_lggr)
target_link_libraries(
        mp_os_allctr_allctr_bndr_tgs
        PUBLIC
        mp_os_allctr_allctr)