
#include "game.hpp"

Game::Game(std::string oFile, std::string imgFile, std::string keyFile)
: window(sf::VideoMode(sf::Vector2u{800,600}), "SFML App"),
twindow(),
textureHolder(),
fontHolder(),
stateStack(State::Context(window, twindow, textureHolder, fontHolder, oFile, keyFile))
{
    window.setFramerateLimit(60);
    textureHolder.load(Textures::TileSheet, imgFile);

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
    while (const std::optional event = window.pollEvent())
    {
        if (event) {
            stateStack.handleEvent(event.value());
            if(event->is<sf::Event::Closed>()) {
                window.close();
            }
        }
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
