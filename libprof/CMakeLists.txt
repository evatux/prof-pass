file(GLOB SOURCES *.c)
add_library(prof SHARED ${SOURCES})
target_compile_options(prof PRIVATE "-fvisibility=hidden")
target_link_libraries(prof PRIVATE dl)
