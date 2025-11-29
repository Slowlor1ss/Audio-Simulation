#include "AudioAnalyzer.h"

AudioAnalyzer::AudioAnalyzer(int fftSize) 
    : m_FftSize(fftSize), m_SampleRate(44100), m_BufferPosition(0)
{
    m_AudioBuffer.resize(fftSize, 0.0f);
    m_WaveformBuffer.resize(fftSize, 0.0f);
    m_WindowFunction.resize(fftSize);

    // Hann window to reduce spectral leakage in the FFT
    for (int i = 0; i < fftSize; i++)
    {
        m_WindowFunction[i] = 0.5f * (1.0f - std::cos(2.0f * M_PI * i / (fftSize - 1)));
    }
}

void AudioAnalyzer::addSamples(const float* samples, int count)
{
    for (int i = 0; i < count; i++)
    {
        m_AudioBuffer[m_BufferPosition] = samples[i];
        m_WaveformBuffer[m_BufferPosition] = samples[i];
        m_BufferPosition = (m_BufferPosition + 1) % m_FftSize;
    }
}

int AudioAnalyzer::reverseBits(int num, int bits)
{
    int result = 0;
    for (int i = 0; i < bits; i++)
    {
        result = (result << 1) | (num & 1);
        num >>= 1;
    }
    return result;
}

void AudioAnalyzer::fft(std::vector<std::complex<float>>& data)
{
    const int n = data.size();
    const int bits = std::log2(n);
    
    // Bit reversal
    for (int i = 0; i < n; i++)
    {
        int j = reverseBits(i, bits);
        if (j > i)
        {
            std::swap(data[i], data[j]);
        }
    }
    
    // Cooley-Tukey FFT
    for (int len = 2; len <= n; len *= 2)
    {
        float angle = -2.0f * M_PI / len;
        std::complex<float> wlen(std::cos(angle), std::sin(angle));
        
        for (int i = 0; i < n; i += len)
        {
            std::complex<float> w(1.0f, 0.0f);
            for (int j = 0; j < len / 2; j++)
            {
                std::complex<float> u = data[i + j];
                std::complex<float> v = data[i + j + len / 2] * w;
                data[i + j] = u + v;
                data[i + j + len / 2] = u - v;
                w *= wlen;
            }
        }
    }
}

void AudioAnalyzer::computeSpectrum(std::vector<float>& magnitudes)
{
    std::vector<std::complex<float>> fftData(m_FftSize);

    for (int i = 0; i < m_FftSize; i++)
    {
	    int bufferIdx = (m_BufferPosition + i) % m_FftSize;
        // Apply window function
        fftData[i] = m_AudioBuffer[bufferIdx] * m_WindowFunction[i];
    }
    
    fft(fftData);
    
    magnitudes.resize(m_FftSize / 2);
    
    // Normalize magnitudes by FFT size
    const float normalization = 2.0f / m_FftSize;
    
    for (int i = 0; i < m_FftSize / 2; i++)
    {
        const float real = fftData[i].real();
        const float imag = fftData[i].imag();
        magnitudes[i] = std::sqrt(real * real + imag * imag) * normalization;
    }
}