
#include "AudioEngine.h"
#include <memory>

AudioEngine::AudioEngine(int sampleRate, int bufferSize)
    : m_Analyzer(), m_SampleRate(sampleRate), 
      m_BufferSize(bufferSize), m_CurrentTime(0.0), m_MasterVolume(1.0f), m_Initialized(false)
{
    m_Analyzer.setSampleRate(sampleRate);
}

AudioEngine::~AudioEngine()
{
    shutdown();
}

bool AudioEngine::initialize()
{
    if (m_Initialized)
        return true;
    
    if (m_Audio.getDeviceCount() < 1)
    {
        return false;
    }
    
    RtAudio::StreamParameters parameters;
    parameters.deviceId = m_Audio.getDefaultOutputDevice();
    parameters.nChannels = 2;
    parameters.firstChannel = 0;
    
    unsigned int bufferFrames = m_BufferSize;
    
    try
    {
        m_Audio.openStream(&parameters, nullptr, RTAUDIO_FLOAT32,
                        m_SampleRate, &bufferFrames, &audioCallback, this);
        m_Audio.startStream();
        m_Initialized = true;
        return true;
    }
    catch (std::exception& e)
    {
        return false;
    }
}

void AudioEngine::shutdown()
{
    if (m_Initialized && m_Audio.isStreamOpen())
    {
        m_Audio.stopStream();
        m_Audio.closeStream();
        m_Initialized = false;
    }
}

void AudioEngine::addSource(const std::shared_ptr<AudioSource>& source)
{
    std::lock_guard lock(m_SourcesMutex);
    m_Sources.push_back(source);
}

void AudioEngine::removeSource(const std::shared_ptr<AudioSource>& source)
{
    std::lock_guard lock(m_SourcesMutex);
    std::erase(m_Sources, source);
}

void AudioEngine::clearSources()
{
    std::lock_guard<std::mutex> lock(m_SourcesMutex);
    m_Sources.clear();
}

int AudioEngine::audioCallback(void* outputBuffer, void* inputBuffer,
                                     unsigned int nFrames, double streamTime,
                                     RtAudioStreamStatus status, void* userData)
{
    AudioEngine* engine = static_cast<AudioEngine*>(userData);
    float* buffer = static_cast<float*>(outputBuffer);
    engine->processAudio(buffer, nFrames);
    return 0;
}

void AudioEngine::processAudio(float* buffer, unsigned int nFrames)
{
    std::lock_guard<std::mutex> lock(m_SourcesMutex);

    // Clear buffer
    for (unsigned int i = 0; i < nFrames * 2; i++)
        buffer[i] = 0.0f;

    // Mix all sources (update audio visualization by extend)
    for (size_t s = 0; s < m_Sources.size(); s++)
    {
	    const auto& source = m_Sources[s];
        if (!source->isEnabled() || !source->isActive())
            continue;

        double sourceTime = m_CurrentTime;

        for (unsigned int i = 0; i < nFrames; i++)
        {
            float sample = source->generate(sourceTime, m_SampleRate);

            // Apply volume
            sample *= source->getVolume() * m_MasterVolume;

            // Stereo output
            buffer[i * 2] += sample;
            buffer[i * 2 + 1] += sample;

            sourceTime += 1.0 / m_SampleRate;
        }
    }

    m_CurrentTime += static_cast<double>(nFrames) / m_SampleRate;

    // Soft clipping
    for (unsigned int i = 0; i < nFrames * 2; i++)
    {
        if (buffer[i] > 1.0f) buffer[i] = 1.0f;
        else if (buffer[i] < -1.0f) buffer[i] = -1.0f;
    }

    // Send to analyzer (mono mix)
    std::vector<float> monoSamples(nFrames);
    for (unsigned int i = 0; i < nFrames; i++)
        monoSamples[i] = 0.5f * (buffer[i * 2] + buffer[i * 2 + 1]);

    m_Analyzer.addSamples(monoSamples.data(), nFrames);
}
