
#include <fstream>
#include <stdexcept>
#include <iostream>

#include "state.hpp"

class EditState : public State
{
public:
    EditState(StateStack& stack, Context context);

    virtual void draw();
    virtual bool update();
    virtual bool handleEvent(const sf::Event& event);
private:
    sf::RenderWindow twindow;
    std::vector<char> kmap;
//    std::map<char, sf::Vector2i> kmap;

    sf::Sprite sheet;

    unsigned int tsize;
    sf::Vector2u isize;
};
