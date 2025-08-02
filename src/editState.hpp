
#include <fstream>
#include <deque>
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
    struct Action {
        size_t x;
        size_t y;
        char old_tile;
        char new_tile;

        void print() const;
    };

    std::deque<Action> history;

    std::vector<char> kmap;
    std::vector<std::vector<char> > map;

    sf::Sprite sheet;

    unsigned int tsize;
    sf::Vector2u isize;
    unsigned int width;
    size_t history_idx;

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
