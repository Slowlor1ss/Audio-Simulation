#include "Application.h"
#include <iostream>
#include "audio/sources/MicrophoneSource.h"
#include "audio/sources/WavFileSource.h"

//#define TEST_MIC
#define TEST_WAV

#if defined(TEST_MIC) && defined(TEST_WAV)
    #error "Cannot define both TEST_MIC and TEST_WAV at the same time, duh..."
#endif

int main()
{
    Application app(1280, 720);
    
    if (!app.initialize())
    {
        std::cerr << "Failed to initialize application\n";
        return 1;
    }

#if defined(TEST_MIC) 
    // ========== MICROPHONE TEST ==========
    const auto micSource = std::make_shared<MicrophoneSource>(44100, 512);
    if (micSource->initialize())
    {
        micSource->setVolume(1.0f);
        app.getAudioEngine().addSource(micSource);
        std::cout << "Microphone enabled - speak up!\n";
        std::cout << "Press ESC to quit\n";
    }
    else
    {
        std::cerr << "Failed to initialize microphone\n";
    }
#endif

#if defined(TEST_WAV)
    // ========== WAV FILE TEST ==========
    auto wavSource = std::make_shared<WavFileSource>();
    if (wavSource->loadFile("resources/400HzTestTone.wav"))
    {
        wavSource->setVolume(0.5f);  // Adjust volume
        wavSource->setLooping(true);  // Loop the audio
        app.getAudioEngine().addSource(wavSource);
        std::cout << "WAV file loaded and playing\n";
    }
    else
    {
        std::cerr << "Failed to load WAV file\n";
    }
#endif
    
    app.run();

#if definied(TEST_MIC)
    // Cleanup mic
    if (micSource)
    {
        micSource->shutdown();
    }
#endif
    
    return 0;
}