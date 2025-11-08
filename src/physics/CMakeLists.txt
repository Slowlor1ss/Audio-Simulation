# physics/CMakeLists.txt
add_library(Physics FluidSimulation.cpp Bubble.cpp)
target_include_directories(Physics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
