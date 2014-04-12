
#include "game.hpp"

#if defined(_WIN32)
#define CS_PATH "..\\resources\\ClearSans-Regular.ttf"
#elif defined(__WIN32__)
#define CS_PATH "..\\resources\\ClearSans-Regular.ttf"
#else
#define CS_PATH "../resources/ClearSans-Regular.ttf"
#endif

Game::Game(std::string oFile, std::string imgFile)
: window(sf::VideoMode(800,600), "SFML App"),
twindow(),
textureHolder(),
fontHolder(),
stateStack(State::Context(window, twindow, textureHolder, fontHolder, oFile))
{
    window.setFramerateLimit(60);
    textureHolder.load(Textures::TileSheet, imgFile);
    try{
        fontHolder.load(Fonts::ClearSans, "ClearSans-Regular.ttf");
    } catch(std::runtime_error e)
    {
        std::cout << "Failed to load font in run location.\n";
        std::cout << "checking ../resources\n";
        fontHolder.load(Fonts::ClearSans, CS_PATH);
    }

    registerStates();

    stateStack.pushState(States::EditState);
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
}
