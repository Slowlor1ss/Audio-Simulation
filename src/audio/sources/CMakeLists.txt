# audio/sources/CMakeLists.txt
add_library(Audio MicrophoneSource.cpp WavFileSource.cpp)
target_include_directories(Audio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})