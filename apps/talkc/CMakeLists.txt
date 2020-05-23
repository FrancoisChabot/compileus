# ****************** talkc_lib ******************
add_library(talkc_app_lib
    src/talkc/compile.cpp
    src/talkc/entry.cpp
    src/talkc/options.cpp
)

target_include_directories(talkc_app_lib PUBLIC include PRIVATE src)
enable_clang_tidy(talkc_app_lib)
target_link_libraries(talkc_app_lib libtalk)


# ****************** talkc app ******************
add_executable(talkc 
    src/main.cpp
)

target_link_libraries(talkc talkc_app_lib)
enable_clang_tidy(talkc)

add_subdirectory(tests)