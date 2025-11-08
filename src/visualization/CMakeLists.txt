# visualization/CMakeLists.txt
add_library(Visualization Renderer.cpp Spectrogram.cpp Waveform.cpp)
target_include_directories(Visualization PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})