
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <sstream>

#include <SFML/Graphics.hpp>

#include "state.hpp"

class EditState : public State
{
public:
    EditState(StateStack& stack, Context context);

    virtual void draw();
    virtual bool update();
    virtual bool handleEvent(const sf::Event& event);
private:
    std::vector<char> kmap;
    std::vector<std::vector<char> > map;

    sf::Sprite sheet;

    unsigned int tsize;
    sf::Vector2u isize;
    unsigned int width;

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
