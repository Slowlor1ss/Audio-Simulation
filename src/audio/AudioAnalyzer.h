#pragma once

#include <vector>
#include <complex>
#include <cmath>
#include <algorithm>
#include <numbers>

// TODO: put this in a math header or something
#ifndef M_PI
constexpr double M_PI = std::numbers::pi;
#endif

class AudioAnalyzer
{
public:
    AudioAnalyzer(int fftSize = 2048);
    
    // Add audio samples to the buffer
    void addSamples(const float* samples, int count);
    
    // Compute FFT and return magnitude spectrum
    void computeSpectrum(std::vector<float>& magnitudes);
    
    // Get the current waveform buffer
    const std::vector<float>& getWaveform() const { return m_WaveformBuffer; }
    
    int getFFTSize() const { return m_FftSize; }
    int getSampleRate() const { return m_SampleRate; }
    void setSampleRate(int rate) { m_SampleRate = rate; }

private:
    void applyWindow();
    void fft(std::vector<std::complex<float>>& data);
    int reverseBits(int num, int bits);
    
    int m_FftSize;
    int m_SampleRate;
    std::vector<float> m_AudioBuffer;
    std::vector<float> m_WaveformBuffer;
    std::vector<float> m_WindowFunction;
    int m_BufferPosition;
};

