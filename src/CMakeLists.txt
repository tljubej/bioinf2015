file(GLOB_RECURSE SOURCES "*.cpp *.h")
include_directories("./")
add_executable(ssamem "main.cc" ${SOURCES})
