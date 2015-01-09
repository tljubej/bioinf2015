file(GLOB SuffixArray "sa/*.cc")
file(GLOB MEM "mem/*.cc")
include_directories("./")
add_executable(suffix_array "programs/suffix_array.cc" ${SuffixArray} "input_util.cc")
add_executable(memer "programs/memer.cc" ${SuffixArray} ${MEM} "input_util.cc" "reference_string.cc")
add_executable(sa_checker "programs/sa_checker.cc" ${SuffixArray} "input_util.cc")

