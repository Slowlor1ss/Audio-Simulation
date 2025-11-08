cmake_minimum_required(VERSION 3.22)
project(AudioSimulation)

set(CMAKE_CXX_STANDARD 23)

# source files
set(SOURCE_FILES
    src/main.cpp
)

set(PHYSICS_FILES
    src/physics/FluidSimulation.cpp
    src/physics/FluidSimulation.h
    src/physics/Bubble.cpp
    src/physics/Bubble.h
)

set(AUDIO_FILES
    src/audio/AudioEngine.cpp
    src/audio/AudioEngine.h
    src/audio/BubbleOscillator.cpp
    src/audio/BubbleOscillator.h
    src/audio/AudioAnalyzer.cpp
    src/audio/AudioAnalyzer.h
)

set(VIS_FILES
    src/visualization/Renderer.cpp
    src/visualization/Renderer.h
    src/visualization/Waveform.cpp
    src/visualization/Waveform.h
    src/visualization/Spectrogram.cpp
    src/visualization/Spectrogram.h
)

# Add executable target
add_executable(AudioSimulation
    ${SOURCE_FILES}
    ${PHYSICS_FILES}
    ${AUDIO_FILES}
    ${VIS_FILES}
)

# Create Visual Studio filters
source_group("Physics" FILES ${PHYSICS_FILES})
source_group("Audio" FILES ${AUDIO_FILES})
source_group("Visualization" FILES ${VIS_FILES})


# Linking external libraries

# -----------------------------
# SFML
# -----------------------------
# set(SFML_DIR "${CMAKE_SOURCE_DIR}/external/SFML/lib/cmake/SFML" CACHE PATH "Path to SFML")
# find_package(SFML COMPONENTS graphics window system REQUIRED)
# target_link_libraries(AudioSimulation PRIVATE sfml-graphics sfml-window sfml-system)

target_include_directories(AudioSimulation PRIVATE "${CMAKE_SOURCE_DIR}/external/SFML/include")
target_link_libraries(AudioSimulation PRIVATE
    "${CMAKE_SOURCE_DIR}/external/SFML/lib/sfml-graphics.lib"
    "${CMAKE_SOURCE_DIR}/external/SFML/lib/sfml-window.lib"
    "${CMAKE_SOURCE_DIR}/external/SFML/lib/sfml-system.lib"
)

# Copy the DLL automatically after build
# I love GLOB :)
file(GLOB SFML_DLLS "${CMAKE_SOURCE_DIR}/external/SFML/bin/*.dll")
foreach(SFML_DLL ${SFML_DLLS})
    add_custom_command(TARGET AudioSimulation POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SFML_DLL}"
            $<TARGET_FILE_DIR:AudioSimulation>
    )
endforeach()

# -----------------------------
# RtAudio
# -----------------------------

# Include headers
#target_include_directories(AudioSimulation PRIVATE "${CMAKE_SOURCE_DIR}/external/RtAudio/include")
target_include_directories(AudioSimulation PRIVATE "${CMAKE_SOURCE_DIR}/external/RtAudio")

# Link library based on build type
target_link_libraries(AudioSimulation PRIVATE
    $<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/external/RtAudio/build/Debug/rtaudiod.lib>
    $<$<CONFIG:Release>:${CMAKE_SOURCE_DIR}/external/RtAudio/build/Release/rtaudio.lib>
)

# Copy the DLL automatically after build
add_custom_command(TARGET AudioSimulation POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/external/RtAudio/build/Debug/rtaudiod.dll>
        $<$<CONFIG:Release>:${CMAKE_SOURCE_DIR}/external/RtAudio/build/Release/rtaudio.dll>
        $<TARGET_FILE_DIR:AudioSimulation>
)

