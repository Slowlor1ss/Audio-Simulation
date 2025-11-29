
#include "Waveform.h"

WaveformVisualizer::WaveformVisualizer(float x, float y, float width, float height)
    : m_X(x), m_Y(y), m_Width(width), m_Height(height), m_AmplitudeScale(1.0f)
{
    m_Background.setPosition({ x, y });
    m_Background.setSize(sf::Vector2f(width, height));
    m_BackgroundColor = sf::Color(20, 20, 30);
    m_Background.setFillColor(m_BackgroundColor);
    
    m_WaveformColor = sf::Color(100, 200, 255);
    m_Waveform.setPrimitiveType(sf::PrimitiveType::LineStrip);
}

void WaveformVisualizer::update(const std::vector<float>& samples)
{
    if (samples.empty())
        return;
    
    m_Waveform.clear();

    const int stride = std::max(1, static_cast<int>(samples.size() / m_Width));
    const float centerY = m_Y + m_Height / 2.0f;
    const float amplitudeHeight = (m_Height / 2.0f) * m_AmplitudeScale;
    
    for (size_t i = 0; i < samples.size(); i += stride)
    {
        float sample = samples[i];
        sample = std::max(-1.0f, std::min(1.0f, sample)); // Clamp

        const float posX = m_X + (static_cast<float>(i) / samples.size()) * m_Width;
        const float posY = centerY - sample * amplitudeHeight;
        
        m_Waveform.append(sf::Vertex(sf::Vector2f(posX, posY), m_WaveformColor));
    }
}

void WaveformVisualizer::draw(sf::RenderWindow& window) const
{
    window.draw(m_Background);
    window.draw(m_Waveform);
}
