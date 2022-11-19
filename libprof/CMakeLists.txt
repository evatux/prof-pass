add_library(prof SHARED prof.c demangler.c)
target_compile_options(prof PRIVATE "-fvisibility=hidden")
target_link_libraries(prof PRIVATE dl)
