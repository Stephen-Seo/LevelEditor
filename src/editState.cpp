
#include "editState.hpp"

EditState::EditState(StateStack& stack, Context context)
: State(stack, context),
currentMode(Mode::layer0),
kmap(),
wm(),
map_layer0(),
selection(0,0),
drawing(false),
deleting(false)
{
    sheet.setTexture(getContext().textures->get(Textures::TileSheet));

    isize = getContext().textures->get(Textures::TileSheet).getSize();

    // Parse keyfile
    std::fstream ks;
    ks.open(getContext().oFile + K_SUFFIX);
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

    std::string line;
    std::getline(ks, line);

    for(int j=0; std::getline(ks, line); ++j)
    {
        for(int i=0; i < line.size(); ++i)
        {
            if(line[i] != ' ')
            {
                kmap.insert(std::pair<char, std::pair<int, int> >(line[i], std::pair<int, int>(i,j)));
            }
        }
    }

    std::cout << kmap.size() << " is number of symbols in key map.\n";

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

    // create window for selecting tiles
    getContext().twindow->create(sf::VideoMode(isize.x,isize.y), "TileSheet");

    // parse layer 0
    std::fstream of;
    of.open(getContext().oFile + L0_SUFFIX);
    line = "";

    if(of.is_open())
    {
        int y;
        for(y=0 ; std::getline(of,line); ++y);

        of.close();
        of.clear();
        of.open(getContext().oFile + L0_SUFFIX);

        for(int j=y-1; std::getline(of, line); --j)
        {
            for(int i=0; i < line.size(); ++i)
            {
                if(line[i] != ' ')
                {
                    map_layer0.add(line[i], i, j);
                }
            }
        }
        of.close();
    }

    // parse layer 1
    of.clear();
    of.open(getContext().oFile + L1_SUFFIX);
    line = "";

    if(of.is_open())
    {
        for(int j=0; std::getline(of, line); ++j)
        {
            for(int i=0; i < line.size(); ++i)
            {
                if(line[i] != ' ')
                {
                    map_layer1.add(line[i], i, j);
                }
            }
        }
        of.close();
    }

    // parse waypoints
    of.clear();
    of.open(getContext().oFile + W_SUFFIX);
    line = "";

    if(of.is_open())
    {
        // find delimeter, parse symbols
        do
        {
            std::getline(of, line);
            if(line[0] == '#')
            {
                break;
            }
            wm.addWaypoint(line[0]);
        }while(line != "");

        // get adjacent
        of.close();
        of.clear();
        of.open(getContext().oFile + W_SUFFIX);
        line = "";
        do
        {
            std::getline(of, line);
            if(line[0] == '#')
            {
                break;
            }
            char current = line[0];
            for(int i=1; i < line.size(); ++i)
            {
                if(line[i] == ' ')
                    continue;
                wm.makeAdjacent(current, line[i]);
            }
        }while(line != "");

        // location parsing
        for(int j=0; std::getline(of, line); ++j)
        {
            for(int i=0; i < line.size(); ++i)
            {
                if(line[i] != ' ')
                {
                    map_waypoint.add(line[i], i, j);
                }
            }
        }
        of.close();
    }

    // parse obstacles
    of.clear();
    of.open(getContext().oFile + O_SUFFIX);
    line = "";
    if(of.is_open())
    {
        for(int j=0; std::getline(of,line); ++j)
        {
            for(int i=0; i < line.size(); ++i)
            {
                if(line[i] != ' ')
                {
                    map_obstacles.add('o', i, j);
                }
            }
        }
        of.close();
    }

    // other initializations
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
    leftIndicator.setPosition(-2.f,0.f);

    topIndicator.setSize(sf::Vector2f(800.f,4.f));
    topIndicator.setFillColor(sf::Color::Red);

    grid.setFillColor(sf::Color(200,200,200));
    grid.setRadius(4.f);
    grid.setOrigin(sf::Vector2f(grid.getLocalBounds().width/2.f,grid.getLocalBounds().height/2.f));

    sf::View cView;
    cView.setSize(getContext().window->getView().getSize());
    cView.setCenter(getContext().window->getView().getCenter());
    cView.move(0,-600.f);
    getContext().window->setView(cView);

    std::cout << "\nkmap contents:\n";
    for(auto iter = kmap.begin(); iter != kmap.end(); ++iter)
    {
        std::cout << iter->first;
    }
    std::cout << "\n";
   }

void EditState::draw()
{
    // Begin drawing for main window

    float l = getContext().window->getView().getCenter().x -
              getContext().window->getView().getSize().x / 2.f;
    float t = getContext().window->getView().getCenter().y -
              getContext().window->getView().getSize().y / 2.f;

    leftIndicator.setPosition(-2.f, t);
    getContext().window->draw(leftIndicator);

    topIndicator.setPosition(l, -2.f);
    getContext().window->draw(topIndicator);
    

    int left,top;
/*
    for(int y=0; y < map_layer0.size(); ++y)
        for(int x=0; x < map_layer0[y].size(); ++x)
            for(int u=0; u < kmap.size(); ++u)
                if(map_layer0[y][x] != ' ' && kmap[u] == map_layer0[y][x])
                {
                    left = (u % width) * tsize;
                    top = (u / width) * tsize;
                    sheet.setTextureRect(sf::IntRect(left,top,tsize,tsize));
                    sheet.setPosition(x*tsize, -y*(int)tsize - (int)tsize);
                    getContext().window->draw(sheet);
                    break;
                }
*/

    for(int y=0; y < map_layer0.getMaxSize().second; ++y)
    {
        auto row = map_layer0.getRow(y);
        for(auto rowIter = row.begin(); rowIter != row.end(); ++rowIter)
        {
            auto pair = kmap.find(rowIter->obj)->second;
            left = pair.first * tsize;
            top = pair.second * tsize;
            sheet.setTextureRect(sf::IntRect(left, top, tsize, tsize));
            sheet.setPosition(rowIter->x * tsize, -rowIter->y * tsize - tsize);
            getContext().window->draw(sheet);
        }
    }

    for(float y=t + 600.f; y >= t; y-=(float)tsize)
        for(float x=l; x <= l + 800.f; x+=(float)tsize)
        {
            grid.setPosition(x,y);
            getContext().window->draw(grid);
        }


/*
    printf("\nMapContents:\n");
    for(int y=0; y < map_layer0.size(); ++y)
    {
        printf("%d]",y);
        for(int x=0; x < map_layer0[y].size(); ++x)
        {
            printf("%c",map_layer0[y][x]);
        }
        printf("\n");
    }
*/

    getContext().window->draw(saveIndicator);

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
        sf::Vector2i mpos = sf::Mouse::getPosition(*(getContext().window));
        sf::Vector2f gpos = getContext().window->mapPixelToCoords(mpos);
        int x = (int)(gpos.x / tsize);
        int y = -(int)(gpos.y / tsize);

        std::cout << "[" << x << "," << y << "]\n";
        map_layer0.remove(x,y);
        /*
        if(y >= 0 && y < map_layer0.size() && x >= 0 && x < map_layer0[y].size())
        {
            while(map_layer0[y].size() <= x)
            {
                map_layer0[y].push_back(' ');
            }
            map_layer0[y][x] = selChar;
        }
        */
    }
    else if(drawing)
    {
        char selChar = ' ';
        for(auto kiter = kmap.begin(); kiter != kmap.end(); ++kiter)
        {
            if(kiter->second.first == selection.x && kiter->second.second == selection.y)
            {
                selChar = kiter->first;
                break;
            }
        }
        if(selChar == ' ') // TODO: DEBUG
            std::cout << "WARNING: selChar IS ' ', selection is [" << selection.x << "," << selection.y << "]\n";

        sf::Vector2i mpos = sf::Mouse::getPosition(*(getContext().window));
        sf::Vector2f gpos = getContext().window->mapPixelToCoords(mpos);
        int x = (int)(gpos.x / tsize);
        int y = -(int)(gpos.y / tsize);
        if(y >= 0 && x >= 0 && selChar != ' ')
        {
            if(map_layer0.get(x,y) != NULL)
                map_layer0.remove(x,y);
            map_layer0.add(selChar, x, y);
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
    sf::Event event;
    while(getContext().twindow->pollEvent(event))
    {
        if(event.type == sf::Event::MouseButtonPressed)
        {
            selection.x = (int) (event.mouseButton.x / (float)tsize);
            selection.y = (int) (event.mouseButton.y / (float)tsize);
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
    else if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right)
        deleting = true;
    else if(event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right)
        deleting = false;
    else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return)
    {
        std::fstream of;
        of.open(getContext().oFile + L0_SUFFIX, std::ios::trunc | std::ios::out);
        if(!of.is_open())
        {
            of.clear();
            of.open(getContext().oFile + L0_SUFFIX, std::ios::out);
            of.close();
            of.open(getContext().oFile + L0_SUFFIX, std::ios::trunc | std::ios::out);
        }

/*
        for(int y=0; y<map_layer0.size(); ++y)
        {
            for(int x=0; x<map_layer0[y].size(); ++x)
            {
                of << map_layer0[y][x];
            }
            if(y != map_layer0.size()-1)
                of << '\n';
        }
*/
/*
        for(auto y = map_layer0.rbegin(); y != map_layer0.rend(); ++y)
        {
            for(auto x = (*y).begin(); x != (*y).end(); ++x)
            {
                of << *x;
            }
            of << '\n';
        }
*/

        for(int y = map_layer0.getMaxSize().second - 1; y >= 0; --y)
        {
            for(int x=0; x < map_layer0.getMaxSize().first; ++x)
            {
                char* c = map_layer0.get(x,y);
                if(c == NULL)
                    of << ' ';
                else
                    of << (*c);
            }
            of << '\n';
        }

        of.flush();
        of.close();

        saveIndicator.setFillColor(sf::Color::White);
    }

    else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Right)
    {
        sf::View cView;
        cView.setSize(getContext().window->getView().getSize());
        cView.setCenter(getContext().window->getView().getCenter());
        cView.move((float)tsize,0);
        getContext().window->setView(cView);
    }
    else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Left)
    {
        sf::View cView;
        cView.setSize(getContext().window->getView().getSize());
        cView.setCenter(getContext().window->getView().getCenter());
        cView.move(-(float)tsize,0);
        getContext().window->setView(cView);
    }
    else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Up)
    {
        sf::View cView;
        cView.setSize(getContext().window->getView().getSize());
        cView.setCenter(getContext().window->getView().getCenter());
        cView.move(0,-(float)tsize);
        getContext().window->setView(cView);
    }
    else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Down)
    {
        sf::View cView;
        cView.setSize(getContext().window->getView().getSize());
        cView.setCenter(getContext().window->getView().getCenter());
        cView.move(0,(float)tsize);
        getContext().window->setView(cView);
    }

    return true;
}

