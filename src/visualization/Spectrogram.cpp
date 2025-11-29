#include "Spectrogram.h"

SpectrogramVisualizer::SpectrogramVisualizer(float x, float y, float width, float height, int historySize)
    : m_X(x), m_Y(y), m_Width(width), m_Height(height), m_HistorySize(historySize),
      m_MinFreq(20.0f), m_MaxFreq(8000.0f), m_SampleRate(44100)
{
    m_Background.setPosition({x, y});
    m_Background.setSize(sf::Vector2f(width, height));
    m_Background.setFillColor(sf::Color(20, 20, 30));
    
    m_Pixels.setPrimitiveType(sf::PrimitiveType::Points);
}

sf::Color SpectrogramVisualizer::getColorForMagnitude(float magnitude) const
{
    // Logarithmic scaling for better visualization (+ 1e-10f to avoid log of 0)
    const float db = 20.0f * std::log10(magnitude + 1e-10f);
    float normalized = (db + 60.0f) / 60.0f; // Map -60dB to 0dB to 0-1 (works better for clean audio)
    //float normalized = (db + 80.0f) / 60.0f; // Map -80dB to -20dB -> 0 to 1 (works better for my not so great microphone)
    normalized = std::max(0.0f, std::min(1.0f, normalized));

    // TODO: not sure if I want to keep this in the long run when we eventually generate sound ourselves
    // Apply gamma correction for better perceptual scaling
    //normalized = std::pow(normalized, 0.5f);
    
    // Color gradient: black -> blue -> cyan -> yellow -> white
    if (normalized < 0.25f)
    {
        float t = normalized / 0.25f;
        // Black -> Blue
        return sf::Color(0, 0, static_cast<std::uint8_t>(t * 255));
    }
    else if (normalized < 0.5f)
    {
        float t = (normalized - 0.25f) / 0.25f;
        // Blue -> Cyan
        return sf::Color(0, static_cast<std::uint8_t>(t * 255), 255);
    }
    else if (normalized < 0.75f)
    {
        float t = (normalized - 0.5f) / 0.25f;
        // Cyan -> Yellow
        return sf::Color(static_cast<std::uint8_t>(t * 255), 255, static_cast<std::uint8_t>(255 * (1.0f - t)));
    }
    else
    {
        float t = (normalized - 0.75f) / 0.25f;
        auto val = static_cast<std::uint8_t>(255);
        // Yellow -> White
        return sf::Color(val, val, static_cast<std::uint8_t>(255 * (1.0f - t) + val * t));
    }
}

void SpectrogramVisualizer::update(const std::vector<float>& spectrum)
{
    if (spectrum.empty())
        return;
    
    m_SpectrogramHistory.push_back(spectrum);
    
    if (m_SpectrogramHistory.size() > static_cast<size_t>(m_HistorySize))
    {
        m_SpectrogramHistory.pop_front();
    }
    
    m_Pixels.clear();

    const int numFreqBins = static_cast<int>(m_Height);
    const float freqPerBin = static_cast<float>(m_SampleRate) / (spectrum.size() * 2);
    
    for (size_t timeIdx = 0; timeIdx < m_SpectrogramHistory.size(); timeIdx++)
    {
        const auto& frame = m_SpectrogramHistory[timeIdx];
        const float posX = m_X + (static_cast<float>(timeIdx) / m_HistorySize) * m_Width;
        
        for (int freqIdx = 0; freqIdx < numFreqBins; freqIdx++)
        {
            // Map frequency bin to spectrum index (logarithmic scale)
            const float freqRatio = static_cast<float>(freqIdx) / numFreqBins;
            const float targetFreq = m_MinFreq * std::pow(m_MaxFreq / m_MinFreq, freqRatio);
            const int spectrumIdx = static_cast<int>(targetFreq / freqPerBin);
            
            if (spectrumIdx >= 0 && spectrumIdx < static_cast<int>(frame.size()))
            {
                const float magnitude = frame[spectrumIdx];
                const sf::Color color = getColorForMagnitude(magnitude);
                
                // Draw from bottom (low freq) to top (high freq)
                const float posY = m_Y + m_Height - (static_cast<float>(freqIdx) / numFreqBins) * m_Height;
                m_Pixels.append(sf::Vertex(sf::Vector2f(posX, posY), color));
            }
        }
    }
}

void SpectrogramVisualizer::draw(sf::RenderWindow& window) const
{
    window.draw(m_Background);
    window.draw(m_Pixels);
}