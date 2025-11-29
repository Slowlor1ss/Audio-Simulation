#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <deque>

// Great video on spectrograms (non technical)
// https://www.youtube.com/watch?v=_FatxGN3vAM

class SpectrogramVisualizer
{
public:
    SpectrogramVisualizer(float x, float y, float width, float height, int historySize = 100);
    
    void update(const std::vector<float>& spectrum);
    void draw(sf::RenderWindow& window) const;
    
    void setMinFrequency(float freq) { m_MinFreq = freq; }
    void setMaxFrequency(float freq) { m_MaxFreq = freq; }
    void setSampleRate(int rate) { m_SampleRate = rate; }

private:
    sf::Color getColorForMagnitude(float magnitude) const;
    
    sf::RectangleShape m_Background;
    std::deque<std::vector<float>> m_SpectrogramHistory;
    sf::VertexArray m_Pixels;
    
    float m_X, m_Y, m_Width, m_Height;
    int m_HistorySize;
    float m_MinFreq, m_MaxFreq;
    int m_SampleRate;
};
