add_library(loader GraphLoader.cpp)
add_library(FMAlgo FiducciaMatthesisAlgo.cpp)
add_library(bucket Part_t.cpp)

target_link_libraries(FMAlgo PUBLIC bucket loader)
