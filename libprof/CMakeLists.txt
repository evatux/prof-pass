add_library(prof SHARED prof.c)
target_compile_options(prof PRIVATE "-fvisibility=hidden")
