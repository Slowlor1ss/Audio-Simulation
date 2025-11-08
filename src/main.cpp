#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include "RtAudio.h"

constexpr double M_PI = 3.1415;

int main()
{
    // ----- Test SFML -----
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(400, 200)), "SFML Test");
    sf::CircleShape shape(50.f);
    shape.setFillColor(sf::Color::Green);

    std::cout << "SFML window created with a circle (hopefully). Close window to continue.\n";

    while (window.isOpen()) 
    {
        while (auto eventOpt = window.pollEvent()) 
        {
		    sf::Event event = *eventOpt;
		    if (event.is<sf::Event::Closed>())
		        window.close();
		}
        window.clear();
        window.draw(shape);
        window.display();
    }

    // ----- Test RtAudio -----
    std::cout << "Testing RtAudio...\n";

    RtAudio audio;
    if (audio.getDeviceCount() < 1) 
    {
        std::cerr << "No audio devices found!\n";
        return 1;
    }

    const unsigned int sampleRate = 44100;
    unsigned int bufferFrames = 256;

    // Simple callback that generates a sine wave
    auto callback = [](void* outputBuffer, void*, unsigned int nBufferFrames,
                       double, RtAudioStreamStatus, void* userData) -> int
	{
	    const auto buffer = static_cast<float*>(outputBuffer);
	    const auto phase = static_cast<double*>(userData);
		const double frequency = 440.0;
	    for (unsigned int i = 0; i < nBufferFrames; i++) 
        {
		    buffer[i] = static_cast<float>(0.3 * sin(*phase));
            *phase += 2.0 * M_PI * frequency / sampleRate;
            if (*phase > 2.0 * M_PI) *phase -= 2.0 * M_PI;
        }
        return 0;
    };

    double phase = 0.0;
    RtAudio::StreamParameters parameters;
    parameters.deviceId = audio.getDefaultOutputDevice();
    parameters.nChannels = 2; // Stereo

    try {
        audio.openStream(&parameters, nullptr, RTAUDIO_FLOAT32, sampleRate,
                         &bufferFrames, callback, &phase);
        audio.startStream();

        std::cout << "Playing a short sine wave. Press Enter to stop.\n";
        std::cin.get();

        audio.stopStream();
        audio.closeStream();
    } catch (std::exception& e) {
        std::cerr << "RtAudio error: " << e.what() << "\n";
        return 1;
    }

    std::cout << "Tests complete.\n";
    return 0;
}
