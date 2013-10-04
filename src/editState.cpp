
#include "editState.hpp"

EditState::EditState(StateStack& stack, Context context)
: State(stack, context),
twindow(),
kmap()
{
    sheet.setTexture(context.textures->get(Textures::TileSheet));

    sf::Vector2u isize = context.textures->get(Textures::TileSheet).getSize();

    std::fstream ks;
    ks.open(context.keyFile);
    if(!ks.is_open())
    {
        ks.close();
        throw std::runtime_error("Unable to open Keyfile.");
    }

    ks >> tsize;
    if(ks.fail())
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

    std::cout << kmap.size() << " is size of map.\n";

    if(ks.bad())
    {
        ks.close();
        throw std::runtime_error("Unable to read Keyfile. (stream badbit true.)");
    }
    else if(ks.fail() && !ks.eof())
    {
        ks.close();
        throw std::runtime_error("Unable to read Keyfile. (stream failbit true.)");
    }

    ks.close();

    if(kmap.size() != isize.x*isize.y/tsize/tsize)
        std::cout << "Warning: Key contents do not match given width/height.\n";
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

