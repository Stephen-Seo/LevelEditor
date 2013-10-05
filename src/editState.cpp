
#include "editState.hpp"

EditState::EditState(StateStack& stack, Context context)
: State(stack, context),
kmap(),
map(),
selection(0,0),
drawing(false),
deleting(false)
{

    sheet.setTexture(getContext().textures->get(Textures::TileSheet));

    isize = getContext().textures->get(Textures::TileSheet).getSize();

    std::fstream ks;
    ks.open(getContext().keyFile);
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

    getContext().twindow->create(sf::VideoMode(isize.x,isize.y), "TileSheet");

    for(unsigned int i = 0; i < 600 / tsize; ++i)
        map.push_back(std::vector<char> ());

    width = isize.x / tsize;
    std::cout << "width is " << width << "\n";

    sHighlight.setFillColor(sf::Color(255,255,255,50));
    sHighlight.setOutlineColor(sf::Color::Red);
    sHighlight.setOutlineThickness(1.f);
    sHighlight.setRadius(tsize/2.f);
}

void EditState::draw()
{
    // Begin drawing for main window
    int left,top;
    for(int y=0; y < map.size(); ++y)
        for(int x=0; x < map[y].size(); ++x)
            for(int u=0; u < kmap.size(); ++u)
                if(kmap[u] == map[y][x])
                {
                    left = (u % width) * tsize;
                    top = (u / width) * tsize;
                    sheet.setTextureRect(sf::IntRect(left,top,tsize,tsize));
                    sheet.setPosition(x*tsize, y*tsize);
                    getContext().window->draw(sheet);
                    break;
                }

    // Begin drawing for twindow
    getContext().twindow->clear(sf::Color(127,127,127));
    sheet.setTextureRect(sf::IntRect(0,0,isize.x,isize.y));
    sheet.setPosition(0.f,0.f);
    getContext().twindow->draw(sheet);

    sHighlight.setPosition((float)(selection.x * tsize),(float)(selection.y * tsize));
    getContext().twindow->draw(sHighlight);

    getContext().twindow->display();
}

bool EditState::update()
{
    // Begin main update

    if(deleting)
    {
        char selChar = ' ';
        sf::Vector2i mpos = sf::Mouse::getPosition(*(getContext().window));
        sf::Vector2f gpos = getContext().window->mapPixelToCoords(mpos);
        int x = (int)(gpos.x / tsize);
        int y = (int)(gpos.y / tsize);
        if(y < map.size())
        {
            while(map[y].size() <= x)
            {
                map[y].push_back(' ');
            }
            map[y][x] = selChar;
        }
    }
    else if(drawing)
    {
        char selChar = kmap[selection.y * width + selection.x];
        sf::Vector2i mpos = sf::Mouse::getPosition(*(getContext().window));
        sf::Vector2f gpos = getContext().window->mapPixelToCoords(mpos);
        int x = (int)(gpos.x / tsize);
        int y = (int)(gpos.y / tsize);
        if(y < map.size())
        {
            while(map[y].size() <= x)
            {
                map[y].push_back(' ');
            }
            map[y][x] = selChar;
        }
    }

    // Begin twindow event handling
    sf::Event event;
    while(getContext().twindow->pollEvent(event))
    {
        if(event.type == sf::Event::MouseButtonPressed)
        {
            selection.x = (unsigned int) (event.mouseButton.x / (float)tsize);
            selection.y = (unsigned int) (event.mouseButton.y / (float)tsize);
        }
    }
    return true;
}

bool EditState::handleEvent(const sf::Event& event)
{
    if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
        drawing = true;
    else if(event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
        drawing = false;

    if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right)
        deleting = true;
    else if(event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right)
        deleting = false;

    return true;
}

