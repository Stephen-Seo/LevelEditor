#include "game.hpp"

#if defined(_WIN32)
#define CS_PATH "..\\resources\\ClearSans-Regular.ttf"
#elif defined(__WIN32__)
#define CS_PATH "..\\resources\\ClearSans-Regular.ttf"
#elif defined(__APPLE__)
#include "utility.hpp"
#else
#define CS_PATH "../resources/ClearSans-Regular.ttf"
#endif

Game::Game(std::string inPrefix, std::string imgFile)
: window(sf::VideoMode(800,600), "Level Editor"),
twindow(),
textureHolder(),
fontHolder(),
stateStack(State::Context(window, twindow, textureHolder, fontHolder, oFile))
{
    registerStates();
    window.setFramerateLimit(60);
    oFile = inPrefix;

#if defined(__APPLE__)
    std::string executablePath = getExecutableDirectory();
    
    std::cout << "checking " << executablePath + "ClearSans-Regular.ttf\n";
    try{
        fontHolder.load(Fonts::ClearSans, executablePath + "ClearSans-Regular.ttf");
    } catch(std::runtime_error e)
    {
        std::cout << "checking " << executablePath + "../resources/ClearSans-Regular.ttf\n";
        fontHolder.load(Fonts::ClearSans, executablePath + "../resources/ClearSans-Regular.ttf");
    }
#else
    try{
        fontHolder.load(Fonts::ClearSans, "ClearSans-Regular.ttf");
    } catch(std::runtime_error e)
    {
        std::cout << "Failed to load font in run location.\n";
        std::cout << "checking " << CS_PATH << "\n";
        fontHolder.load(Fonts::ClearSans, CS_PATH);
    }
#endif

    if(oFile == "" && imgFile == "")
    {
        stateStack.pushState(States::OpenState);
    }
    else
    {
#if defined(__APPLE__)
        oFile = executablePath + oFile;
        imgFile = executablePath + imgFile;
#endif
        textureHolder.load(Textures::TileSheet, imgFile);
        stateStack.pushState(States::EditState);
    }
}

void Game::run()
{
    sf::Clock clock;
    while (window.isOpen())
    {
        update();
        processEvents();
        draw();
    }
    twindow.close();
}

void Game::processEvents()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        stateStack.handleEvent(event);
        if(event.type == sf::Event::Closed)
            window.close();
    }
}

void Game::update()
{
    stateStack.update();
}

void Game::draw()
{
    window.clear(sf::Color(127,127,127));
    stateStack.draw();
    window.display();
}

void Game::registerStates()
{
    stateStack.registerState<EditState>(States::EditState);
    stateStack.registerState<OpenState>(States::OpenState);
}
