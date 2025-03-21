cmake_minimum_required(VERSION 3.4.1)

add_executable(diag_revealer diag_revealer.c)

target_link_libraries(diag_revealer
        log)

find_library(DL_LIB dl)
if (DL_LIB)
    target_link_libraries(diag_revealer ${DL_LIB})
endif()