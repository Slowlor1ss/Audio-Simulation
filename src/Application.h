#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <sstream>
#include <iomanip>
#include "audio/AudioEngine.h"
#include "visualization/Waveform.h"
#include "visualization/Spectrogram.h"

class Application
{
public:
    Application(int width = 1280, int height = 720);
    ~Application() = default;
    
    bool initialize();
    void run();
    
    AudioEngine& getAudioEngine() { return m_AudioEngine; }

private:
    void processEvents();
    void update(const float dt);
    void render();
    void updateVisualizations();
    
    sf::RenderWindow m_Window;
    AudioEngine m_AudioEngine;
    
    std::unique_ptr<WaveformVisualizer> m_WaveformViz;
    std::unique_ptr<SpectrogramVisualizer> m_SpectrogramViz;

    float m_LastVolume = 1.f;
    
    int m_WindowWidth;
    int m_WindowHeight;
    
    static constexpr float SIDEBAR_RATIO = 0.3f;
};
