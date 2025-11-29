#pragma once

#include "../AudioEngine.h"
#include <vector>
#include <string>

// Simple WAV file player (supports 16-bit PCM WAV files only)
class WavFileSource : public AudioSource
{
public:
    WavFileSource();
    
    bool loadFile(const std::string& filename);
    
    float generate(double time, int sampleRate) override;
    bool isActive() const override;
    
    void setLooping(bool loop) { m_Looping = loop; }
    bool isLooping() const { return m_Looping; }
    
    void reset() { m_CurrentSample = 0; }
    
    int getFileSampleRate() const { return m_FileSampleRate; }
    int getNumChannels() const { return m_NumChannels; }

private:
    std::vector<float> m_Samples;
    size_t m_CurrentSample;
    int m_FileSampleRate;
    int m_NumChannels;
    bool m_Looping;
};