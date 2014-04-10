
#ifndef EDIT_STATE_HPP
#define EDIT_STATE_HPP

#include <fstream>
#include <stdexcept>
#include <iostream>
#include <sstream>

#include <SFML/Graphics.hpp>

#include "state.hpp"

#define K_SUFFIX "_key"
#define L0_SUFFIX "_level_layer0"
#define L1_SUFFIX "_level_layer1"
#define W_SUFFIX "_waypoint"
#define O_SUFFIX "_obstacles"

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
        layer0, layer1, waypoint, obstacles
    };

    Mode currentMode;

    std::vector<char> kmap;
    std::vector<std::vector<char> > map_layer0;
    std::vector<std::vector<char> > map_layer1;

    std::vector<char> wmap;
    std::vector<std::vector<char> > map_waypoint;

    std::vector<std::vector<char> > map_obstacles;

    sf::Sprite sheet;

    unsigned int tsize;
    sf::Vector2u isize;
    int width;

    sf::Vector2u selection;

    bool drawing;
    bool deleting;

    sf::CircleShape sHighlight;
    sf::CircleShape grid;

    sf::RectangleShape saveIndicator;
    sf::RectangleShape leftIndicator;
    sf::RectangleShape topIndicator;

    sf::View cView;
};

#endif
