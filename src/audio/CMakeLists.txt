# audio/CMakeLists.txt
add_library(Audio AudioAnalyzer.cpp AudioEngine.cpp BubbleOscillator.cpp)
target_include_directories(Audio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})