
#include "editState.hpp"

EditState::EditState(StateStack& stack, Context context)
: State(stack, context),
twindow(),
kmap()
{
    std::fstream ks;
    ks.open(context.keyFile);
    if(!ks.is_open())
    {
        ks.close();
        throw std::runtime_error("Unable to open Keyfile.");
    }

    ks >> tsize;
    if(ks.fail() || ks.bad())
    {
        ks.close();
        throw std::runtime_error("Unable to read Keyfile.");
    }

    ks >> width;
    if(ks.fail() || ks.bad())
    {
        ks.close();
        throw std::runtime_error("Unable to read Keyfile.");
    }

    ks >> height;
    if(ks.fail() || ks.bad())
    {
        ks.close();
        throw std::runtime_error("Unable to read Keyfile.");
    }

    char s;

    while(ks >> s)
    {
        if(s != '\n')
            kmap.push_back(s);
    }

    if(ks.fail())
    {
        ks.close();
        throw std::runtime_error("Unable to read Keyfile. (stream failbit true.)");
    }
    else if(ks.bad())
    {
        ks.close();
        throw std::runtime_error("Unable to read Keyfile. (stream badbit true.)");
    }

    ks.close();

    if(kmap.size() != width*height)
        std::cout << "Warning: Key contents do not match given width/height.";
}

void EditState::draw()
{
}

bool EditState::update()
{
    return true;
}

bool EditState::handleEvent(const sf::Event& event)
{
    return true;
}

