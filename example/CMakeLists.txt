function(add name)
    add_executable(${name} ${ARGN})
    add_dependencies(${name} ProfPass)
    target_compile_options(${name} PRIVATE -flegacy-pass-manager "SHELL:-Xclang -load" "SHELL:-Xclang ${PROJECT_BINARY_DIR}/pass/libProfPass.so")
    target_link_libraries(${name} prof)
endfunction()

add(example_01 example_01.cpp)
