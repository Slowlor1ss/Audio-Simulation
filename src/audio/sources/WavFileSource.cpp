#include "WavFileSource.h"
#include <fstream>
#include <iostream>
#include <cstring>

WavFileSource::WavFileSource()
    : m_CurrentSample(0), m_FileSampleRate(44100), m_NumChannels(1), m_Looping(true)
{
}

bool WavFileSource::loadFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open WAV file: " << filename << "\n";
        return false;
    }
    
    // Read RIFF header
    char chunkId[4];
    file.read(chunkId, 4);
    if (std::strncmp(chunkId, "RIFF", 4) != 0)
    {
        std::cerr << "Not a valid WAV file (missing RIFF header)\n";
        return false;
    }
    
    uint32_t chunkSize;
    file.read(reinterpret_cast<char*>(&chunkSize), 4);
    
    char format[4];
    file.read(format, 4);
    if (std::strncmp(format, "WAVE", 4) != 0)
    {
        std::cerr << "Not a valid WAV file (missing WAVE format)\n";
        return false;
    }
    
    // Read fmt subchunk
    char subchunk1Id[4];
    file.read(subchunk1Id, 4);
    if (std::strncmp(subchunk1Id, "fmt ", 4) != 0)
    {
        std::cerr << "Invalid WAV file (missing fmt chunk)\n";
        return false;
    }
    
    uint32_t subchunk1Size;
    file.read(reinterpret_cast<char*>(&subchunk1Size), 4);
    
    uint16_t audioFormat;
    file.read(reinterpret_cast<char*>(&audioFormat), 2);
    if (audioFormat != 1)
    {
        std::cerr << "Only PCM WAV files are supported\n";
        return false;
    }
    
    uint16_t numChannels;
    file.read(reinterpret_cast<char*>(&numChannels), 2);
    m_NumChannels = numChannels;
    
    uint32_t sampleRate;
    file.read(reinterpret_cast<char*>(&sampleRate), 4);
    m_FileSampleRate = sampleRate;
    
    uint32_t byteRate;
    file.read(reinterpret_cast<char*>(&byteRate), 4);
    
    uint16_t blockAlign;
    file.read(reinterpret_cast<char*>(&blockAlign), 2);
    
    uint16_t bitsPerSample;
    file.read(reinterpret_cast<char*>(&bitsPerSample), 2);
    
    if (bitsPerSample != 16)
    {
        std::cerr << "Only 16-bit WAV files are supported\n";
        return false;
    }
    
    // Skip any extra format bytes
    if (subchunk1Size > 16)
    {
        file.seekg(subchunk1Size - 16, std::ios::cur);
    }
    
    // Find data subchunk
    char subchunk2Id[4];
    uint32_t subchunk2Size;
    
    while (file.read(subchunk2Id, 4))
    {
        file.read(reinterpret_cast<char*>(&subchunk2Size), 4);
        
        if (std::strncmp(subchunk2Id, "data", 4) == 0)
        {
            break;
        }
        
        // Skip this chunk
        file.seekg(subchunk2Size, std::ios::cur);
    }
    
    if (std::strncmp(subchunk2Id, "data", 4) != 0)
    {
        std::cerr << "No data chunk found in WAV file\n";
        return false;
    }
    
    // Read audio data
    size_t numSamples = subchunk2Size / (bitsPerSample / 8);
    m_Samples.resize(numSamples);
    
    for (size_t i = 0; i < numSamples; i++)
    {
        int16_t sample;
        file.read(reinterpret_cast<char*>(&sample), 2);
        m_Samples[i] = static_cast<float>(sample) / 32768.0f;
    }
    
    file.close();
    
    std::cout << "Loaded WAV file: " << filename << "\n"
              << "  Sample rate: " << m_FileSampleRate << " Hz\n"
              << "  Channels: " << m_NumChannels << "\n"
              << "  Samples: " << numSamples << "\n"
              << "  Duration: " << static_cast<float>(numSamples / m_NumChannels) / m_FileSampleRate << " seconds\n";
    
    m_CurrentSample = 0;
    return true;
}

float WavFileSource::generate(double time, int sampleRate)
{
    if (m_Samples.empty())
        return 0.0f;
    
    // Simple sample rate conversion (nearest neighbor)
    // TODO: convert to linear interpolation
    const float sampleRateRatio = static_cast<float>(m_FileSampleRate) / sampleRate;
    const size_t sourceSample = static_cast<size_t>(m_CurrentSample * sampleRateRatio);
    
    // Handle multi-channel (mix to mono)
    size_t frameIndex = (sourceSample / m_NumChannels) * m_NumChannels;
    
    if (frameIndex >= m_Samples.size())
    {
        if (m_Looping)
        {
            m_CurrentSample = 0;
            frameIndex = 0;
        }
        else
        {
            return 0.0f;
        }
    }
    
    // Mix channels to mono
    float sample = 0.0f;
    for (int ch = 0; ch < m_NumChannels && frameIndex + ch < m_Samples.size(); ch++)
    {
        sample += m_Samples[frameIndex + ch];
    }
    sample /= m_NumChannels;
    
    m_CurrentSample++;
    
    return sample;
}

bool WavFileSource::isActive() const
{
    if (m_Samples.empty())
        return false;
    
    if (m_Looping)
        return true;
    
    return m_CurrentSample < m_Samples.size();
}