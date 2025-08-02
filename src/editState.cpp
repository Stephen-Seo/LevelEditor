
#include "editState.hpp"

EditState::EditState(StateStack& stack, Context context)
: State(stack, context),
kmap(),
map(),
sheet(context.textures->get(Textures::TileSheet)),
selection(0,0),
drawing(false),
deleting(false)
{
    //sheet.setTexture(getContext().textures->get(Textures::TileSheet));

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

    while((s = ks.get()) != std::char_traits<char>::eof())
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

    getContext().twindow->create(sf::VideoMode({isize.x,isize.y}), "TileSheet");

    for(unsigned int i = 0; i < 600 / tsize; ++i)
        map.push_back(std::vector<char> ());

    std::fstream of;
    of.open(getContext().oFile);
    std::string line;

    if(of.is_open())
    {
        for(int i=0; std::getline(of, line); ++i)
        {
            while(map.size() <= i)
                map.push_back(std::vector<char> ());

        }
        of.close();
    }

    of.clear();
    of.open(getContext().oFile);
    if(of.is_open())
    {
        for(int i=0; std::getline(of, line); ++i)
        {
            for(int j=0; j < line.size(); ++j)
                map[map.size() - i - 1].push_back(line[j]);
        }
        of.close();
    }

    width = isize.x / tsize;
    std::cout << "width is " << width << "\n";

    sHighlight.setFillColor(sf::Color(255,255,255,50));
    sHighlight.setOutlineColor(sf::Color::Red);
    sHighlight.setOutlineThickness(1.f);
    sHighlight.setRadius(tsize/2.f);

    saveIndicator.setSize(sf::Vector2f(800.f,600.f));
    saveIndicator.setFillColor(sf::Color(255,255,255,0));

    leftIndicator.setSize(sf::Vector2f(4.f,600.f));
    leftIndicator.setFillColor(sf::Color::Green);
    leftIndicator.setPosition({-2.f,0.f});

    topIndicator.setSize(sf::Vector2f(800.f,4.f));
    topIndicator.setFillColor(sf::Color::Red);

    grid.setFillColor(sf::Color(200,200,200));
    grid.setRadius(4.f);
    grid.setOrigin(grid.getLocalBounds().size/2.0F);

    sf::View cView;
    cView.setSize(getContext().window->getView().getSize());
    cView.setCenter(getContext().window->getView().getCenter());
    cView.move({0.0F,-600.0F});
    getContext().window->setView(cView);

    printf("\nKMap contents:\n");
    for(int i = 0; i < kmap.size(); ++i)
        printf("%c",kmap[i]);
    printf("\n");

}

void EditState::draw()
{
    // Begin drawing for main window

    float l = getContext().window->getView().getCenter().x -
              getContext().window->getView().getSize().x / 2.f;
    float t = getContext().window->getView().getCenter().y -
              getContext().window->getView().getSize().y / 2.f;

    leftIndicator.setPosition({-2.f, t});
    getContext().window->draw(leftIndicator);

    topIndicator.setPosition({l, -2.f});
    getContext().window->draw(topIndicator);
    

    int left,top;
    for(int y=0; y < map.size(); ++y)
        for(int x=0; x < map[y].size(); ++x)
            for(int u=0; u < kmap.size(); ++u)
                if(map[y][x] != ' ' && kmap[u] == map[y][x])
                {
                    left = (u % width) * tsize;
                    top = (u / width) * tsize;
                    sheet.setTextureRect(sf::IntRect({left,top},{tsize,tsize}));
                    sheet.setPosition({x*tsize, -y*(int)tsize - (int)tsize});
                    getContext().window->draw(sheet);
                    break;
                }

    for(float y=t + 600.f; y >= t; y-=(float)tsize)
        for(float x=l; x <= l + 800.f; x+=(float)tsize)
        {
            grid.setPosition({x,y});
            getContext().window->draw(grid);
        }


/*
    printf("\nMapContents:\n");
    for(int y=0; y < map.size(); ++y)
    {
        printf("%d]",y);
        for(int x=0; x < map[y].size(); ++x)
        {
            printf("%c",map[y][x]);
        }
        printf("\n");
    }
*/

    getContext().window->draw(saveIndicator);

    // Begin drawing for twindow
    getContext().twindow->clear(sf::Color(127,127,127));
    sheet.setTextureRect(sf::IntRect({0,0},{isize.x,isize.y}));
    sheet.setPosition({0.f,0.f});
    getContext().twindow->draw(sheet);

    sHighlight.setPosition({(float)(selection.x * tsize),(float)(selection.y * tsize)});
    getContext().twindow->draw(sHighlight);

    getContext().twindow->display();
}

bool EditState::update()
{

    // Begin main update
    int l = (int)( getContext().window->getView().getCenter().x -
                   getContext().window->getView().getSize().x / 2.f);
    int t = (int)( getContext().window->getView().getCenter().y -
                   getContext().window->getView().getSize().y / 2.f);

    t = -t - 600;

    l /= tsize;
    t /= tsize;

    std::stringstream ss;
    ss << "LevelEditor [" << l << "," << t << "]";
    getContext().window->setTitle(ss.str());


    if(deleting)
    {
        char selChar = ' ';
        sf::Vector2i mpos = sf::Mouse::getPosition(*(getContext().window));
        sf::Vector2f gpos = getContext().window->mapPixelToCoords(mpos);
        int x = (int)(gpos.x / tsize);
        int y = -(int)(gpos.y / tsize);
        if(y >= 0 && y < map.size() && x >= 0 && x < map[y].size())
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
        int y = -(int)(gpos.y / tsize);
        //printf("%d,%d\n",x,y);
        if(y >= 0 && x >= 0)
        {
            while(map.size() <= y)
            {
                map.push_back(std::vector<char>());
            }
            while(map[y].size() <= x)
            {
                map[y].push_back(' ');
            }
            map[y][x] = selChar;
        }
    }

    saveIndicator.setPosition(getContext().window->mapPixelToCoords(sf::Vector2i(0,0)));
    if(saveIndicator.getFillColor().a > 0)
    {
        unsigned int a = saveIndicator.getFillColor().a;
        if(a >= 5)
            a-=5;
        else
            a=0;
        saveIndicator.setFillColor(sf::Color(255,255,255,a));
    }

    // Begin twindow event handling
    while(const auto event = getContext().twindow->pollEvent())
    {
        if(event->is<sf::Event::MouseButtonPressed>())
        {
            sf::Vector2i pos = event->getIf<sf::Event::MouseButtonPressed>()->position;
            selection.x = (unsigned int) (pos.x / (float)tsize);
            selection.y = (unsigned int) (pos.y / (float)tsize);
        }
    }
    return true;
}

bool EditState::handleEvent(const sf::Event& event)
{
    if(event.is<sf::Event::MouseButtonPressed>() && event.getIf<sf::Event::MouseButtonPressed>()->button == sf::Mouse::Button::Left)
        drawing = true;
    else if(event.is<sf::Event::MouseButtonReleased>() && event.getIf<sf::Event::MouseButtonReleased>()->button == sf::Mouse::Button::Left)
        drawing = false;
    else if(event.is<sf::Event::MouseButtonPressed>() && event.getIf<sf::Event::MouseButtonPressed>()->button == sf::Mouse::Button::Right)
        deleting = true;
    else if(event.is<sf::Event::MouseButtonReleased>() && event.getIf<sf::Event::MouseButtonReleased>()->button == sf::Mouse::Button::Right)
        deleting = false;
    else if(event.is<sf::Event::KeyPressed>() && event.getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Enter)
    {
        std::fstream of;
        of.open(getContext().oFile, std::ios::trunc | std::ios::out);
        if(!of.is_open())
        {
            of.clear();
            of.open(getContext().oFile, std::ios::out);
            of.close();
            of.open(getContext().oFile, std::ios::trunc | std::ios::out);
        }

/*
        for(int y=0; y<map.size(); ++y)
        {
            for(int x=0; x<map[y].size(); ++x)
            {
                of << map[y][x];
            }
            if(y != map.size()-1)
                of << '\n';
        }
*/
        for(auto y = map.rbegin(); y != map.rend(); ++y)
        {
            for(auto x = (*y).begin(); x != (*y).end(); ++x)
            {
                of << *x;
            }
            of << '\n';
        }

        of.flush();
        of.close();

        saveIndicator.setFillColor(sf::Color::White);
    }

    else if(event.is<sf::Event::KeyPressed>() && event.getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Right)
    {
        sf::View cView;
        cView.setSize(getContext().window->getView().getSize());
        cView.setCenter(getContext().window->getView().getCenter());
        cView.move({(float)tsize,0.0F});
        getContext().window->setView(cView);
    }
    else if(event.is<sf::Event::KeyPressed>() && event.getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Left)
    {
        sf::View cView;
        cView.setSize(getContext().window->getView().getSize());
        cView.setCenter(getContext().window->getView().getCenter());
        cView.move({-(float)tsize,0.0F});
        getContext().window->setView(cView);
    }
    else if(event.is<sf::Event::KeyPressed>() && event.getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Up)
    {
        sf::View cView;
        cView.setSize(getContext().window->getView().getSize());
        cView.setCenter(getContext().window->getView().getCenter());
        cView.move({0.0F,-(float)tsize});
        getContext().window->setView(cView);
    }
    else if(event.is<sf::Event::KeyPressed>() && event.getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Down)
    {
        sf::View cView;
        cView.setSize(getContext().window->getView().getSize());
        cView.setCenter(getContext().window->getView().getCenter());
        cView.move({0.0F,(float)tsize});
        getContext().window->setView(cView);
    }

    return true;
}

