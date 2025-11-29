#pragma once

#include <SFML/Graphics.hpp>

class WaveformVisualizer
{
public:
    WaveformVisualizer(float x, float y, float width, float height);
    
    void update(const std::vector<float>& samples);
    void draw(sf::RenderWindow& window) const;
    
    void setColor(const sf::Color& color) { m_WaveformColor = color; }
    void setBackgroundColor(const sf::Color& color) { m_BackgroundColor = color; }
    void setAmplitudeScale(float scale) { m_AmplitudeScale = scale; }

private:
    sf::RectangleShape m_Background;
    sf::VertexArray m_Waveform;
    sf::Color m_WaveformColor;
    sf::Color m_BackgroundColor;
    
    float m_X, m_Y, m_Width, m_Height;
    // TODO: maybe make a slider for amplitudeScale
    float m_AmplitudeScale;
};
