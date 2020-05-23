# ****************** talkc_lib ******************
add_library(compileus_app_lib
  src/compileus_app/entry.cpp
)

target_include_directories(compileus_app_lib PUBLIC include PRIVATE src)
enable_clang_tidy(compileus_app_lib)


# ****************** talkc app ******************
add_executable(compileus
    src/main.cpp
)

target_link_libraries(compileus compileus_app_lib)
enable_clang_tidy(compileus)

add_subdirectory(tests)