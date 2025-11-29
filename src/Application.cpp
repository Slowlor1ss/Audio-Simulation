
#include "Application.h"

Application::Application(int width, int height)
    : m_WindowWidth(width), m_WindowHeight(height)
{
}

bool Application::initialize()
{
    m_Window.create(sf::VideoMode({static_cast<unsigned int>(m_WindowWidth), 
                                  static_cast<unsigned int>(m_WindowHeight)}), 
                  "Water Boiling Sound Simulation");
    m_Window.setFramerateLimit(60);
    
    if (!m_AudioEngine.initialize())
    {
        return false;
    }
    
    float sidebarWidth = static_cast<float>(m_WindowWidth) * SIDEBAR_RATIO;
    // TODO: for later & maybe adjust
    float mainWidth = static_cast<float>(m_WindowWidth) * (1.0f - SIDEBAR_RATIO);
    
    // Waveform takes top half of sidebar
    m_WaveformViz = std::make_unique<WaveformVisualizer>(
        0, 0, sidebarWidth, (float)m_WindowHeight * 0.5f
    );
    m_WaveformViz->setColor(sf::Color(100, 200, 255));
    
    // Spectrogram takes bottom half of sidebar
    m_SpectrogramViz = std::make_unique<SpectrogramVisualizer>(
        0, (float)m_WindowHeight * 0.5f, sidebarWidth, (float)m_WindowHeight * 0.5f, 150
    );
    m_SpectrogramViz->setSampleRate(m_AudioEngine.getSampleRate());
    
    return true;
}

void Application::run()
{
    sf::Clock clock;
    
    while (m_Window.isOpen())
    {
	    const float dt = clock.restart().asSeconds();
        processEvents();
        update(dt);
        render();
    }
}

void Application::processEvents()
{
    while (const std::optional event = m_Window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            m_Window.close();
        }
        
        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            if (keyPressed->code == sf::Keyboard::Key::Escape)
            {
                m_Window.close();
            }

            // Space bar to toggle master volume on/off
            if (keyPressed->code == sf::Keyboard::Key::Space)
            {
                float currentVolume = m_AudioEngine.getMasterVolume();
                m_AudioEngine.setMasterVolume(currentVolume > 0.0f ? 0.0f : m_LastVolume);
                m_LastVolume = currentVolume;
            }
            
            // Up/Down arrows to adjust master volume
            if (keyPressed->code == sf::Keyboard::Key::Up)
            {
                float vol = m_AudioEngine.getMasterVolume();
                m_AudioEngine.setMasterVolume(std::min(1.0f, vol + 0.1f));
            }
            if (keyPressed->code == sf::Keyboard::Key::Down)
            {
                float vol = m_AudioEngine.getMasterVolume();
                m_AudioEngine.setMasterVolume(std::max(0.0f, vol - 0.1f));
            }
        }
    }
}

void Application::update(const float dt)
{
    updateVisualizations();
}

void Application::updateVisualizations()
{
    auto& analyzer = m_AudioEngine.getAnalyzer();
    
    // Update waveform
    const auto& waveform = analyzer.getWaveform();
    m_WaveformViz->update(waveform);
    
    // Update spectrogram
    std::vector<float> spectrum;
    analyzer.computeSpectrum(spectrum);
    m_SpectrogramViz->update(spectrum);
}

void Application::render()
{
    m_Window.clear(sf::Color(30, 30, 40));
    
    // Visualizations on left sidebar
    m_WaveformViz->draw(m_Window);
    m_SpectrogramViz->draw(m_Window);
    
    // Divider line between sidebar and main area
    float sidebarWidth = static_cast<float>(m_WindowWidth) * SIDEBAR_RATIO;
    sf::RectangleShape divider(sf::Vector2f(2, static_cast<float>(m_WindowHeight)));
    divider.setPosition({sidebarWidth, 0});
    divider.setFillColor(sf::Color(60, 60, 70));
    m_Window.draw(divider);

    // TODO:
    // Draw bubble simulation in main area
    sf::View mainView = m_Window.getView();
    mainView.setViewport(sf::FloatRect({SIDEBAR_RATIO, 0.0f}, {1.0f - SIDEBAR_RATIO, 1.0f}));
    m_Window.setView(mainView);
    
    // Reset view
    m_Window.setView(m_Window.getDefaultView());
    
    m_Window.display();
}