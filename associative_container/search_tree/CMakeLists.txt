add_subdirectory(binary_search_tree)
add_subdirectory(indexing_tree)

add_library(
        mp_os_assctv_cntnr_srch_tr
        include/search_tree.h
        src/hhh.cpp)

target_include_directories(
        mp_os_assctv_cntnr_srch_tr
        PUBLIC
        ./include)

target_link_libraries(
        mp_os_assctv_cntnr_srch_tr
        PUBLIC
        mp_os_lggr_lggr)
target_link_libraries(
        mp_os_assctv_cntnr_srch_tr
        PUBLIC
        mp_os_allctr_allctr)
target_link_libraries(
        mp_os_assctv_cntnr_srch_tr
        PUBLIC
        mp_os_assctv_cntnr)