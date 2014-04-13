
#ifndef EDIT_STATE_HPP
#define EDIT_STATE_HPP

#include <fstream>
#include <stdexcept>
#include <iostream>
#include <sstream>

#include <SFML/Graphics.hpp>

#include "state.hpp"
#include "waypoint.hpp"

#include "coordinateMap.hpp"

#define K_SUFFIX "_key.txt"
#define L0_SUFFIX "_level_layer0.txt"
#define L1_SUFFIX "_level_layer1.txt"
#define W_SUFFIX "_waypoint.txt"
#define O_SUFFIX "_obstacles.txt"
#define E_SUFFIX "_entities.txt"

class EditState : public State
{
public:
    EditState(StateStack& stack, Context context);

    virtual void draw();
    virtual bool update();
    virtual bool handleEvent(const sf::Event& event);
private:
    enum Mode
    {
        layer0, layer1, waypoint, obstacles, entities
    };

    Mode currentMode;

    std::map<char, std::pair<int,int> > kmap;
    CoordinateMap<char> map_layer0;
    CoordinateMap<char> map_layer1;

    std::string validChars;
    WaypointManager wm;
    CoordinateMap<char> map_waypoint;

    CoordinateMap<char> map_obstacles;

    CoordinateMap<char> map_entities;

    sf::Sprite sheet;

    int tsize;
    sf::Vector2u isize;
    int width;

    sf::Vector2i selection;

    bool drawing;
    bool deleting;

    sf::CircleShape waypointMarker;
    sf::CircleShape sHighlight;
    sf::CircleShape grid;

    sf::RectangleShape obstacleIndicator;
    sf::RectangleShape saveIndicator;
    sf::RectangleShape leftIndicator;
    sf::RectangleShape topIndicator;

    sf::VertexArray adjLine;
    sf::Vector2i linkSelection;

    int eSymbolSelection;
    sf::Text entitySymbol;

    sf::View cView;
};

#endif
