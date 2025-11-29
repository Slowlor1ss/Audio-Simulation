#include "MicrophoneSource.h"
#include <iostream>
#include <algorithm>

MicrophoneSource::MicrophoneSource(int sampleRate, int bufferSize)
    : m_ReadPosition(0), m_WritePosition(0),
      m_SampleRate(sampleRate), m_BufferSize(bufferSize), m_Active(false), m_Initialized(false)
{
    // Simple ring buffer sized to about 1 second of audio
    m_InputBuffer.resize(sampleRate, 0.0f);
}

MicrophoneSource::~MicrophoneSource()
{
    shutdown();
}

bool MicrophoneSource::initialize()
{
    if (m_Initialized)
        return true;
    
    if (m_InputAudio.getDeviceCount() < 1)
    {
        std::cerr << "No audio input devices found\n";
        return false;
    }
    
    RtAudio::StreamParameters parameters;
    parameters.deviceId = m_InputAudio.getDefaultInputDevice();
    parameters.nChannels = 1;  // Mono input
    parameters.firstChannel = 0;
    
    unsigned int bufferFrames = m_BufferSize;
    
    try
    {
        m_InputAudio.openStream(nullptr, &parameters, RTAUDIO_FLOAT32,
                               m_SampleRate, &bufferFrames, &inputCallback, this);
        m_InputAudio.startStream();
        m_Initialized = true;
        m_Active = true;
        std::cout << "Microphone initialized successfully\n";
        return true;
    }
    catch (std::exception& e)
    {
        std::cerr << "Failed to initialize microphone: " << e.what() << "\n";
        return false;
    }
}

void MicrophoneSource::shutdown()
{
    if (m_Initialized && m_InputAudio.isStreamOpen())
    {
        m_InputAudio.stopStream();
        m_InputAudio.closeStream();
        m_Initialized = false;
        m_Active = false;
    }
}

int MicrophoneSource::inputCallback(void* outputBuffer, void* inputBuffer,
                                    unsigned int nFrames, double streamTime,
                                    RtAudioStreamStatus status, void* userData)
{
    MicrophoneSource* source = static_cast<MicrophoneSource*>(userData);
    float* input = static_cast<float*>(inputBuffer);
    
    std::lock_guard<std::mutex> lock(source->m_BufferMutex);
    
    // Write to ring buffer
    for (unsigned int i = 0; i < nFrames; i++)
    {
        source->m_InputBuffer[source->m_WritePosition] = input[i];
        source->m_WritePosition = (source->m_WritePosition + 1) % source->m_InputBuffer.size();
    }
    
    return 0;
}

float MicrophoneSource::generate(double time, int sampleRate)
{
    std::lock_guard<std::mutex> lock(m_BufferMutex);
    
    // Read from ring buffer
    const float sample = m_InputBuffer[m_ReadPosition];
    m_ReadPosition = (m_ReadPosition + 1) % m_InputBuffer.size();
    
    return sample;
}