add_subdirectory(hash_table)
add_subdirectory(search_tree)

add_library(
        mp_os_assctv_cntnr
        include/associative_container.h
        src/hhh.cpp)

target_include_directories(
        mp_os_assctv_cntnr
        PUBLIC
        ./include)

target_link_libraries(
        mp_os_assctv_cntnr
        PUBLIC
        mp_os_cmmn)