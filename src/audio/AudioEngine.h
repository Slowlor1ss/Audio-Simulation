#pragma once

#include <RtAudio.h>
#include <mutex>
#include <vector>
#include "AudioAnalyzer.h"

class AudioSource
{
public:
    virtual ~AudioSource() = default;
    virtual float generate(double time, int sampleRate) = 0;
    virtual bool isActive() const = 0;
    
    void setVolume(float vol) { m_Volume = vol; }
    float getVolume() const { return m_Volume; }
    
    void setEnabled(bool enabled) { this->m_Enabled = enabled; }
    bool isEnabled() const { return m_Enabled; }
    
protected:
    float m_Volume = 1.0f;
    bool m_Enabled = true;
};

class AudioEngine
{
public:
    AudioEngine(int sampleRate = 44100, int bufferSize = 512);
    ~AudioEngine();
    
    bool initialize();
    void shutdown();
    
    void addSource(const std::shared_ptr<AudioSource>& source);
    void removeSource(const std::shared_ptr<AudioSource>& source);
    void clearSources();
    
    AudioAnalyzer& getAnalyzer() { return m_Analyzer; }
    
    int getSampleRate() const { return m_SampleRate; }
    double getCurrentTime() const { return m_CurrentTime; }
    
    void setMasterVolume(float volume) { m_MasterVolume = volume; }
    float getMasterVolume() const { return m_MasterVolume; }

private:
    static int audioCallback(void* outputBuffer, void* inputBuffer, 
                           unsigned int nFrames, double streamTime,
                           RtAudioStreamStatus status, void* userData);
    
    void processAudio(float* buffer, unsigned int nFrames);
    
    RtAudio m_Audio;
    AudioAnalyzer m_Analyzer;
    
    std::vector<std::shared_ptr<AudioSource>> m_Sources;
    std::mutex m_SourcesMutex;
    
    int m_SampleRate;
    int m_BufferSize;
    double m_CurrentTime;
    float m_MasterVolume;
    bool m_Initialized;
};
