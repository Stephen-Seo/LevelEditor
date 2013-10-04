
#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "resourceHolder.hpp"
#include "resourceIdentifiers.hpp"
#include "state.hpp"
#include "stateStack.hpp"

class Game
{
public:
    Game(std::string oFile, std::string imgFile);
    void run();
private:
    void processEvents();
    void update();
    void draw();
    void registerStates();

    sf::RenderWindow window;

    TextureHolder textureHolder;
    FontHolder fontHolder;

    StateStack stateStack;
};

#endif
