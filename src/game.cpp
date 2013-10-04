
#include "game.hpp"

Game::Game()
: window(sf::VideoMode(720,480), "SFML App"),
textureHolder(),
fontHolder(),
stateStack(State::Context(window, textureHolder, fontHolder))
{
    window.setFramerateLimit(60);
}

void Game::run()
{
    sf::Clock clock;
    while (window.isOpen())
    {
        update();
        draw();
    }
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
    window.clear();
    stateStack.draw();
    window.display();
}

void Game::registerStates()
{
}
