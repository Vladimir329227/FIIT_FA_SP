add_subdirectory(tests)

add_library(
        mp_os_allctr_allctr_bdds_sstm
        src/allocator_buddies_system.cpp)

target_include_directories(
        mp_os_allctr_allctr_bdds_sstm
        PUBLIC
        ./include)

target_link_libraries(
        mp_os_allctr_allctr_bdds_sstm
        PUBLIC
        mp_os_cmmn)
target_link_libraries(
        mp_os_allctr_allctr_bdds_sstm
        PUBLIC
        mp_os_lggr_lggr)
target_link_libraries(
        mp_os_allctr_allctr_bdds_sstm
        PUBLIC
        mp_os_allctr_allctr)