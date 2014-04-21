
#ifndef OPEN_STATE_HPP
#define OPEN_STATE_HPP

#include <iostream>
#include <string>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "state.hpp"

class OpenState : public State
{
public:
    OpenState(StateStack& stack, Context context);

    virtual void draw();
    virtual bool update();
    virtual bool handleEvent(const sf::Event& event);
private:
    void write(std::string c);
    void backspace();
    void startEditor();

    enum Selection
    {
        None,
        Prefix,
        Img
    };

    Selection selection;

    std::string oFile;
    std::string imgFile;

    sf::Text info;
    sf::VertexArray line;

    bool lshift;
    bool rshift;
};

#endif
