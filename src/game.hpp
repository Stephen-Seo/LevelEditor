
#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "resourceHolder.hpp"
#include "resourceIdentifiers.hpp"
#include "state.hpp"
#include "stateStack.hpp"
#include "stateIdentifiers.hpp"
#include "editState.hpp"
#include "openState.hpp"

class Game
{
public:
    Game(std::string inPrefix, std::string imgFile);
    void run();
private:
    void processEvents();
    void update();
    void draw();
    void registerStates();

    sf::RenderWindow window;
    sf::RenderWindow twindow;

    TextureHolder textureHolder;
    FontHolder fontHolder;

    std::string oFile;

    StateStack stateStack;
};

#endif
