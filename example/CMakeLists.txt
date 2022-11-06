add_executable(example_01 example_01.cpp)
add_dependencies(example_01 ProfPass)
target_compile_options(example_01 PRIVATE -flegacy-pass-manager "SHELL:-Xclang -load" "SHELL:-Xclang ${PROJECT_BINARY_DIR}/pass/libProfPass.so")
target_link_libraries(example_01 prof)
