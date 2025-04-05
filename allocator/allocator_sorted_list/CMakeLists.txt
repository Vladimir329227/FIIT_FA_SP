add_subdirectory(tests)

add_library(
        mp_os_allctr_allctr_srtd_lst
        src/allocator_sorted_list.cpp)

target_include_directories(
        mp_os_allctr_allctr_srtd_lst
        PUBLIC
        ./include)

target_link_libraries(
        mp_os_allctr_allctr_srtd_lst
        PUBLIC
        mp_os_cmmn)
target_link_libraries(
        mp_os_allctr_allctr_srtd_lst
        PUBLIC
        mp_os_lggr_lggr)
target_link_libraries(
        mp_os_allctr_allctr_srtd_lst
        PUBLIC
        mp_os_allctr_allctr)