#pragma once

#include "../AudioEngine.h"
#include <RtAudio.h>
#include <vector>
#include <mutex>

class MicrophoneSource final : public AudioSource
{
public:
    MicrophoneSource(int sampleRate = 44100, int bufferSize = 512);
    ~MicrophoneSource() override;
    
    bool initialize();
    void shutdown();
    
    float generate(double time, int sampleRate) override;
    bool isActive() const override { return m_Active; }
    
private:
    static int inputCallback(void* outputBuffer, void* inputBuffer,
                            unsigned int nFrames, double streamTime,
                            RtAudioStreamStatus status, void* userData);
    
    RtAudio m_InputAudio;
    // Used as a simple ring buffer
    std::vector<float> m_InputBuffer;
    std::mutex m_BufferMutex;
    int m_ReadPosition;
    int m_WritePosition;
    int m_SampleRate;
    int m_BufferSize;
    bool m_Active;
    bool m_Initialized;
};