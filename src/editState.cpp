
#include "editState.hpp"

EditState::EditState(StateStack& stack, Context context)
: State(stack, context),
currentMode(Mode::layer0),
kmap(),
map_layer0(),
map_layer1(),
validChars("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@$%^&*()-_=+[]{}|;:',./<>?"),
wm(),
map_waypoint(),
map_obstacles(),
map_entities(),
ewmap(),
selection(0,0),
drawing(false),
deleting(false),
adjLine(sf::LinesStrip, 2),
linkSelection(-1,-1),
ewLine(sf::LinesStrip, 2),
entitySelection(-1,-1),
eSymbolSelection(0),
entitySymbol("Hi",getContext().fonts->get(Fonts::ClearSans),10),
warpSymbol("Derp",getContext().fonts->get(Fonts::ClearSans),10),
warpLine(sf::LinesStrip, 2),
warpSelection(NULL)
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
    getContext().twindow->create(sf::VideoMode(isize.x,isize.y), "layer0");

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
        int y;
        for(y=0; std::getline(of,line); ++y);

        of.close();
        of.clear();
        of.open(getContext().oFile + L1_SUFFIX);

        for(int j=y-1; std::getline(of, line); --j)
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

        // get height of map
        int y;
        for(y=0; std::getline(of, line); ++y);

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
        for(int j=y-1; std::getline(of, line); --j)
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
        int y;
        for(y=0; std::getline(of,line); ++y);

        of.close();
        of.clear();
        of.open(getContext().oFile + O_SUFFIX);

        for(int j=y-1; std::getline(of,line); --j)
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

    // parse entities
    of.clear();
    of.open(getContext().oFile + E_SUFFIX);
    line = "";
    if(of.is_open())
    {
        // parse connections until delimeter is reached
        while(std::getline(of,line))
        {
            if(line[0] == '#')
                break;

            ewmap.insert(std::pair<char,std::list<char> >(line[0], std::list<char>()));
            for(int i=1; i < line.size(); ++i)
            {
                if(line[i] == ' ')
                    continue;
                ewmap[line[0]].push_back(line[i]);
            }
        }

        // get height
        int y;
        for(y=0; std::getline(of,line); ++y);

        of.close();
        of.clear();
        of.open(getContext().oFile + E_SUFFIX);

        // reset to just past delimeter
        while(std::getline(of,line) && line[0] != '#');

        // parse entities
        for(int j=y-1; std::getline(of,line); --j)
        {
            for(int i=0; i < line.size(); ++i)
            {
                if(line[i] != ' ')
                {
                    map_entities.add(line[i], i, j);
                }
            }
        }
        of.close();
    }

    // parse warps
    of.clear();
    of.open(getContext().oFile + WA_SUFFIX);
    line = "";
    if(of.is_open())
    {
        while(std::getline(of,line))
        {
            if(line[0] == '#')
                break;

            char warp = line[0];

            unsigned long space0 = line.find_first_of(" ");
            unsigned long space1 = line.find_first_of(" ",space0+1);
            if(space0 == std::string::npos || space1 == std::string::npos)
            {
                throw std::runtime_error("Invalid warps file detected.");
            }

            std::string substring = line.substr(space0+1,space1-space0-1);
            int coord0;// = std::stoi(substring);
            std::istringstream(substring) >> coord0;
            substring = line.substr(space1+1, std::string::npos);
            int coord1;// = std::stoi(substring);
            std::istringstream(substring) >> coord1;

            warp_destinations.add(warp, coord0, coord1);
        }

        int y;
        for(y=0; std::getline(of,line); ++y);

        of.close();
        of.clear();
        of.open(getContext().oFile + WA_SUFFIX);

        while(std::getline(of,line) && line[0] != '#');

        for(int j=y-1; std::getline(of,line); --j)
        {
            for(int i=0; i < line.size(); ++i)
            {
                if(line[i] != ' ')
                {
                    map_warps.add(line[i], i, j);
                }
            }
        }
        of.close();
    }

    // other initializations
    width = isize.x / tsize;
    std::cout << "width is " << width << "\n";

    waypointMarker.setFillColor(sf::Color(255,255,255,50));
    waypointMarker.setOutlineColor(sf::Color::Blue);
    waypointMarker.setOutlineThickness(1.f);
    waypointMarker.setRadius(tsize/2.f);

    sHighlight.setFillColor(sf::Color(255,255,255,50));
    sHighlight.setOutlineColor(sf::Color::Red);
    sHighlight.setOutlineThickness(1.f);
    sHighlight.setRadius(tsize/2.f);

    obstacleIndicator.setSize(sf::Vector2f((float)tsize,(float)tsize));
    obstacleIndicator.setFillColor(sf::Color(0,0,0,100));

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

    adjLine[0].color = sf::Color::Red;
    adjLine[1].color = sf::Color::Red;

    ewLine[0].color = sf::Color::Blue;
    ewLine[1].color = sf::Color::Blue;

    entitySymbol.setCharacterSize((unsigned int) tsize);
    entitySymbol.setStyle(sf::Text::Regular);

    warpSymbol.setCharacterSize((unsigned int) tsize);
    warpSymbol.setStyle(sf::Text::Regular);
    warpSymbol.setColor(sf::Color(30,255,30));

    warpLine[0].color = sf::Color::Green;
    warpLine[0].color = sf::Color::Green;

    std::cout << "\nkmap contents:\n";
    for(auto iter = kmap.begin(); iter != kmap.end(); ++iter)
    {
        std::cout << iter->first;
    }
    std::cout << "\n";

    getContext().window->setPosition(sf::Vector2i(0,50));
    getContext().twindow->setPosition(sf::Vector2i((int)getContext().window->getSize().x,50));
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

    // draw layer0
    if(currentMode != Mode::layer1)
    {
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
    }

    // draw layer1
    if(currentMode != Mode::layer0)
    {
        for(int y=0; y < map_layer1.getMaxSize().second; ++y)
        {
            auto row = map_layer1.getRow(y);
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
    }

    // draw waypoints
    if(currentMode == Mode::waypoint || currentMode == Mode::ewconnect)
    {
        for(int j=0; j < map_waypoint.getMaxSize().second; ++j)
        {
            for(int i=0; i < map_waypoint.getMaxSize().first; ++i)
            {
                if(map_waypoint.get(i,j) != NULL)
                {
                    waypointMarker.setPosition((float)(i * tsize),(float)(-j * tsize - tsize));
                    getContext().window->draw(waypointMarker);
                }
            }
        }

        // Drawing existing edges
        std::set<std::pair<Waypoint,Waypoint> > edges = wm.getEdges();
        for(auto iter = edges.begin(); iter != edges.end(); ++iter)
        {
            char a = iter->first.symbol;
            char b = iter->second.symbol;

            sf::Vector2i aCoords(-1,-1);
            sf::Vector2i bCoords(-1,-1);

            for(int j=0; j < map_waypoint.getMaxSize().second; ++j)
            {
                auto row = map_waypoint.getRow(j);
                for(auto riter = row.begin(); riter != row.end(); ++riter)
                {
                    if(riter->obj == a)
                    {
                        aCoords.x = riter->x;
                        aCoords.y = riter->y;
                    }
                    else if(riter->obj == b)
                    {
                        bCoords.x = riter->x;
                        bCoords.y = riter->y;
                    }
                    if(aCoords.x != -1 && bCoords.x != -1)
                        break;
                }
                if(aCoords.x != -1 && bCoords.x != -1)
                    break;
            }

            adjLine[0].position.x = (float) (aCoords.x * tsize) + ((float)tsize)/2.0f;
            adjLine[0].position.y = (float) (-aCoords.y * tsize) - ((float)tsize)/2.0f;
            adjLine[1].position.x = (float) (bCoords.x * tsize) + ((float)tsize)/2.0f;
            adjLine[1].position.y = (float) (-bCoords.y * tsize) - ((float)tsize)/2.0f;
            getContext().window->draw(adjLine);
        }

        // Drawing new connection to mouse
        if(linkSelection.x != -1)
        {
            sf::Vector2i mpos = sf::Mouse::getPosition(*(getContext().window));
            sf::Vector2f gpos = getContext().window->mapPixelToCoords(mpos);

            adjLine[0].position.x = (float) (linkSelection.x * tsize) + ((float)tsize)/2.0f;
            adjLine[0].position.y = (float) (-linkSelection.y * tsize) - ((float)tsize)/2.0f;
            adjLine[1].position = gpos;
            getContext().window->draw(adjLine);
        }
    }

    // draw obstacles
    if(currentMode == Mode::obstacles)
    {
        for(int j=0; j < map_obstacles.getMaxSize().second; ++j)
        {
            for(int i=0; i < map_obstacles.getMaxSize().first; ++i)
            {
                if(map_obstacles.get(i,j) != NULL)
                {
                    obstacleIndicator.setPosition((float)(i * tsize),(float)(-j * tsize - tsize));
                    getContext().window->draw(obstacleIndicator);
                }
            }
        }
    }

    // draw entities
    if(currentMode == Mode::entities || currentMode == Mode::ewconnect)
    {
        for(int j=0; j < map_entities.getMaxSize().second; ++j)
        {
            for(int i=0; i < map_entities.getMaxSize().first; ++i)
            {
                char* c = map_entities.get(i,j);
                if(c != NULL)
                {
                    entitySymbol.setString(*c);
                    entitySymbol.setPosition((float)(i * tsize), (float)(-j * tsize - tsize));
                    getContext().window->draw(entitySymbol);
                }
            }
        }
    }

    // draw entity waypoint connections
    if(currentMode == Mode::ewconnect)
    {
        for(auto iter = ewmap.begin(); iter != ewmap.end(); ++iter)
        {
            char entity = iter->first;
            for(auto liter = iter->second.begin(); liter != iter->second.end(); ++liter)
            {
                char waypoint = *liter;
                sf::Vector2i ecoords(-1,-1);
                sf::Vector2i wcoords(-1,-1);

                // TODO make following more efficient
                std::list<RowEntry<char> > row;
                for(int j=0; j < map_entities.getMaxSize().second; ++j)
                {
                    row = map_entities.getRow(j);
                    for(auto riter = row.begin(); riter != row.end(); ++riter)
                    {
                        if(riter->obj == entity)
                        {
                            ecoords.x = riter->x;
                            ecoords.y = riter->y;
                            break;
                        }
                    }
                    if(ecoords.x != -1)
                        break;
                }

                for(int j=0; j < map_waypoint.getMaxSize().second; ++j)
                {
                    row = map_waypoint.getRow(j);
                    for(auto riter = row.begin(); riter != row.end(); ++riter)
                    {
                        if(riter->obj == waypoint)
                        {
                            wcoords.x = riter->x;
                            wcoords.y = riter->y;
                            break;
                        }
                    }
                    if(wcoords.x != -1)
                        break;
                }

                ewLine[0].position.x = ((float) ecoords.x * tsize) + ((float)tsize)/2.0f;
                ewLine[0].position.y = ((float) -ecoords.y * tsize) - ((float)tsize)/2.0f;
                ewLine[1].position.x = ((float) wcoords.x * tsize) + ((float)tsize)/2.0f;
                ewLine[1].position.y = ((float) -wcoords.y * tsize) - ((float)tsize)/2.0f;
                
                getContext().window->draw(ewLine);
            }
        }

        if(entitySelection.x != -1)
        {
            sf::Vector2i mpos = sf::Mouse::getPosition(*(getContext().window));
            sf::Vector2f gpos = getContext().window->mapPixelToCoords(mpos);

            ewLine[0].position.x = (float) (entitySelection.x * tsize) + ((float)tsize)/2.0f;
            ewLine[0].position.y = (float) (-entitySelection.y * tsize) - ((float)tsize)/2.0f;
            ewLine[1].position = gpos;

            getContext().window->draw(ewLine);
        }
    }

    // draw warps
    if(currentMode == Mode::warps)
    {
        for(int j=0; j < map_warps.getMaxSize().second; ++j)
        {
            for(int i=0; i < map_warps.getMaxSize().first; ++i)
            {
                char* c = map_warps.get(i,j);
                if(c != NULL)
                {
                    warpSymbol.setString(*c);
                    warpSymbol.setPosition((float)(i * tsize), (float)(-j * tsize - tsize));
                    getContext().window->draw(warpSymbol);
                }
            }
        }

        if(warpSelection != NULL)
        {
            sf::Vector2i mpos = sf::Mouse::getPosition(*(getContext().window));
            sf::Vector2f gpos = getContext().window->mapPixelToCoords(mpos);

            auto coords = map_warps.get(*warpSelection);

            if(coords.first == -1)
                throw std::runtime_error("Unable to find existing warp coordinates.");

            warpLine[0].position.x = (float) (coords.first * tsize) + ((float)tsize)/2.0f;
            warpLine[0].position.y = (float) (-coords.second * tsize) - ((float)tsize)/2.0f;
            warpLine[1].position = gpos;

            getContext().window->draw(warpLine);
        }

        for(int j=0; j < warp_destinations.getMaxSize().second; ++j)
        {
            for(int i=0; i < warp_destinations.getMaxSize().first; ++i)
            {
                char* c = warp_destinations.get(i,j);
                if(c != NULL)
                {
                    auto coords = map_warps.get(*c);

                    warpLine[0].position.x = (float) (coords.first * tsize) + ((float)tsize)/2.0f;
                    warpLine[0].position.y = (float) (-coords.second * tsize) - ((float)tsize)/2.0f;
                    warpLine[1].position.x = (float) (i * tsize) + ((float)tsize)/2.0f;
                    warpLine[1].position.y = (float) (-j * tsize) - ((float)tsize)/2.0f;

                    getContext().window->draw(warpLine);
                }
            }
        }
    }

    // draw grid
    for(float y=t + getContext().window->getView().getSize().y; y >= t; y-=(float)tsize)
        for(float x=l; x <= l + getContext().window->getView().getSize().x; x+=(float)tsize)
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
    if(currentMode != Mode::entities && currentMode != Mode::warps)
    {
        sheet.setTextureRect(sf::IntRect(0,0,isize.x,isize.y));
        sheet.setPosition(0.f,0.f);
        getContext().twindow->draw(sheet);

        sHighlight.setPosition((float)(selection.x * tsize),(float)(selection.y * tsize));
        getContext().twindow->draw(sHighlight);
    }
    else if(currentMode == Mode::entities)
    {
        entitySymbol.setString(validChars[eSymbolSelection]);
        entitySymbol.setPosition(0.0f, 0.0f);
        getContext().twindow->draw(entitySymbol);
    }
    else if(currentMode == Mode::warps)
    {
        warpSymbol.setString(validChars[eSymbolSelection]);
        warpSymbol.setPosition(0.0f, 0.0f);
        getContext().twindow->draw(warpSymbol);
    }
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

        if(currentMode == Mode::layer0)
            map_layer0.remove(x,y);
        else if(currentMode == Mode::layer1)
            map_layer1.remove(x,y);
        else if(currentMode == Mode::waypoint)
        {
            char* waypointChar = map_waypoint.get(x,y);
            if(waypointChar != NULL)
            {
                wm.removeWaypoint(*waypointChar);
                map_waypoint.remove(x,y);
                for(auto iter = ewmap.begin(); iter != ewmap.end(); ++iter)
                {
                    for(auto liter = iter->second.begin(); liter != iter->second.end(); ++liter)
                    {
                        if(*liter == *waypointChar)
                        {
                            iter->second.erase(liter);
                            break;
                        }
                    }
                }
            }
        }
        else if(currentMode == Mode::obstacles)
            map_obstacles.remove(x,y);
        else if(currentMode == Mode::entities)
        {
            char* entity = map_entities.get(x,y);
            if(entity != NULL)
            {
                ewmap.erase(*entity);
                map_entities.remove(x,y);
            }
        }
        else if(currentMode == Mode::warps)
        {
            char* w = map_warps.get(x,y);
            if(w != NULL)
            {
                warp_destinations.remove(*w);
                map_warps.remove(x,y);
            }
        }
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

        sf::Vector2i mpos = sf::Mouse::getPosition(*(getContext().window));
        sf::Vector2f gpos = getContext().window->mapPixelToCoords(mpos);
        int x = (int)(gpos.x / tsize);
        int y = -(int)(gpos.y / tsize);
        if(y >= 0 && x >= 0)
        {
            if(currentMode == Mode::layer0 && selChar != ' ')
            {
                if(map_layer0.get(x,y) != NULL)
                    map_layer0.remove(x,y);
                map_layer0.add(selChar, x, y);
            }
            else if(currentMode == Mode::layer1 && selChar != ' ')
            {
                if(map_layer1.get(x,y) != NULL)
                    map_layer1.remove(x,y);
                map_layer1.add(selChar, x, y);
            }
            else if(currentMode == Mode::waypoint)
            {
                if(map_waypoint.get(x,y) == NULL)
                {
                    int i;
                    for(i=0; i < validChars.size(); ++i)
                    {
                        if(wm.getCurrentChars().find(validChars[i]) == std::string::npos)
                        {
                            selChar = validChars[i];
                            break;
                        }
                    }
                    if(i != validChars.size())
                    {
                        map_waypoint.add(selChar, x, y);
                        wm.addWaypoint(selChar);
                    }
                }
            }
            else if(currentMode == Mode::obstacles)
            {
                map_obstacles.add('o', x, y);
            }
            else if(currentMode == Mode::entities)
            {
                if(map_entities.get(x,y) != NULL)
                    map_entities.remove(x,y);
                map_entities.add(validChars[eSymbolSelection], x, y);
            }
            else if(currentMode == Mode::warps)
            {
                if(map_warps.get(x,y) != NULL)
                {
                    map_warps.remove(x,y);
                    warp_destinations.remove(x,y);
                }
                map_warps.add(validChars[eSymbolSelection], x, y);
            }
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
        // get max height
        int my=0;
        if(map_layer0.getMaxSize().second > my)
            my = map_layer0.getMaxSize().second;
        if(map_layer1.getMaxSize().second > my)
            my = map_layer1.getMaxSize().second;
        if(map_waypoint.getMaxSize().second > my)
            my = map_waypoint.getMaxSize().second;
        if(map_obstacles.getMaxSize().second > my)
            my = map_obstacles.getMaxSize().second;

        std::fstream of;

        // write layer0
        of.open(getContext().oFile + L0_SUFFIX, std::ios::trunc | std::ios::out);
        if(!of.is_open())
        {
            of.clear();
            of.open(getContext().oFile + L0_SUFFIX, std::ios::out);
            of.close();
            of.open(getContext().oFile + L0_SUFFIX, std::ios::trunc | std::ios::out);
        }

        for(int j = my; j > map_layer0.getMaxSize().second; --j)
            of << '\n';
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

        // write layer1
        of.clear();
        of.open(getContext().oFile + L1_SUFFIX, std::ios::trunc | std::ios::out);
        if(!of.is_open())
        {
            of.clear();
            of.open(getContext().oFile + L1_SUFFIX, std::ios::out);
            of.close();
            of.open(getContext().oFile + L1_SUFFIX, std::ios::trunc | std::ios::out);
        }

        for(int j = my; j > map_layer1.getMaxSize().second; --j)
            of << '\n';
        for(int y = map_layer1.getMaxSize().second - 1; y >= 0; --y)
        {
            for(int x=0; x < map_layer1.getMaxSize().first; ++x)
            {
                char* c = map_layer1.get(x,y);
                if(c == NULL)
                    of << ' ';
                else
                    of << (*c);
            }
            of << '\n';
        }

        of.flush();
        of.close();

        // write waypoints
        of.clear();
        of.open(getContext().oFile + W_SUFFIX, std::ios::trunc | std::ios::out);
        if(!of.is_open())
        {
            of.clear();
            of.open(getContext().oFile + W_SUFFIX, std::ios::out);
            of.close();
            of.open(getContext().oFile + W_SUFFIX, std::ios::trunc | std::ios::out);
        }

        std::string usedChars = wm.getCurrentChars();
        for(int i=0; i < usedChars.size(); ++i)
        {
            of << usedChars[i] << ' ';
            Waypoint* waypoint = wm.getWaypoint(usedChars[i]);
            for(auto iter = waypoint->adjacent.begin(); iter != waypoint->adjacent.end(); ++iter)
            {
                of << (*iter)->symbol << ' ';
            }
            of << '\n';
        }

        of << "#\n";

        for(int j = my; j > map_waypoint.getMaxSize().second; --j)
            of << '\n';
        for(int y = map_waypoint.getMaxSize().second - 1; y >= 0; --y)
        {
            for(int x=0; x < map_waypoint.getMaxSize().first; ++x)
            {
                char* c = map_waypoint.get(x,y);
                if(c == NULL)
                    of << ' ';
                else
                    of << (*c);
            }
            of << '\n';
        }

        of.flush();
        of.close();

        // write obstacles
        of.clear();
        of.open(getContext().oFile + O_SUFFIX, std::ios::trunc | std::ios::out);
        if(!of.is_open())
        {
            of.clear();
            of.open(getContext().oFile + O_SUFFIX, std::ios::out);
            of.close();
            of.open(getContext().oFile + O_SUFFIX, std::ios::trunc | std::ios::out);
        }

        for(int j = my; j > map_obstacles.getMaxSize().second; --j)
            of << '\n';
        for(int y = map_obstacles.getMaxSize().second - 1; y >= 0; --y)
        {
            for(int x=0; x < map_obstacles.getMaxSize().first; ++x)
            {
                char* c = map_obstacles.get(x,y);
                if(c == NULL)
                    of << ' ';
                else
                    of << 'o';
            }
            of << '\n';
        }

        of.flush();
        of.close();

        // write entities
        of.clear();
        of.open(getContext().oFile + E_SUFFIX, std::ios::trunc | std::ios::out);
        if(!of.is_open())
        {
            of.clear();
            of.open(getContext().oFile + E_SUFFIX, std::ios::out);
            of.close();
            of.open(getContext().oFile + E_SUFFIX, std::ios::trunc | std::ios::out);
        }

        for(auto iter = ewmap.begin(); iter != ewmap.end(); ++iter)
        {
            char c = iter->first;
            of << c << " ";
            for(auto liter = iter->second.begin(); liter != iter->second.end(); ++liter)
            {
                of << *liter << " ";
            }
            of << '\n';
        }

        of << "#\n";

        for(int j = my; j > map_entities.getMaxSize().second; --j)
            of << '\n';
        for(int y = map_entities.getMaxSize().second - 1; y >= 0; --y)
        {
            for(int x=0; x < map_entities.getMaxSize().first; ++x)
            {
                char* c = map_entities.get(x,y);
                if(c == NULL)
                    of << ' ';
                else
                    of << *c;
            }
            of << '\n';
        }

        of.flush();
        of.close();

        // write warps
        of.clear();
        of.open(getContext().oFile + WA_SUFFIX, std::ios::trunc | std::ios::out);
        if(!of.is_open())
        {
            of.clear();
            of.open(getContext().oFile + WA_SUFFIX, std::ios::out);
            of.close();
            of.open(getContext().oFile + WA_SUFFIX, std::ios::trunc | std::ios::out);
        }

        for(int j=0; j < warp_destinations.getMaxSize().second; ++j)
        {
            auto row = warp_destinations.getRow(j);
            for(auto riter = row.begin(); riter != row.end(); ++riter)
            {
                of << riter->obj << " " << riter->x << " " << riter->y << "\n";
            }
        }

        of << "#\n";

        for(int j = my; j > map_warps.getMaxSize().second; --j)
            of << '\n';
        for(int y = map_warps.getMaxSize().second - 1; y >= 0; --y)
        {
            for(int x=0; x < map_warps.getMaxSize().first; ++x)
            {
                char* c = map_warps.get(x,y);
                if(c == NULL)
                    of << ' ';
                else
                    of << *c;
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
    else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
    {
        linkSelection.x = -1;
        warpSelection = NULL;
        switch(currentMode)
        {
        case Mode::layer0:
            currentMode = Mode::layer1;
            getContext().twindow->setTitle("layer1");
            break;
        case Mode::layer1:
            currentMode = Mode::waypoint;
            getContext().twindow->setTitle("waypoint");
            break;
        case Mode::waypoint:
            currentMode = Mode::obstacles;
            getContext().twindow->setTitle("obstacles");
            break;
        case Mode::obstacles:
            currentMode = Mode::entities;
            getContext().twindow->setTitle("entities");
            break;
        case Mode::entities:
            currentMode = Mode::ewconnect;
            getContext().twindow->setTitle("entity_waypoint_connect");
            break;
        case Mode::ewconnect:
            currentMode = Mode::warps;
            getContext().twindow->setTitle("warps");
            break;
        case Mode::warps:
            currentMode = Mode::layer0;
            getContext().twindow->setTitle("layer0");
            break;
        default:
            currentMode = Mode::layer0;
            getContext().twindow->setTitle("layer0");
            break;
        }
    }
    else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::L)
    {
        sf::Vector2i mpos = sf::Mouse::getPosition(*(getContext().window));
        sf::Vector2f gpos = getContext().window->mapPixelToCoords(mpos);
        int x = (int)(gpos.x / tsize);
        int y = -(int)(gpos.y / tsize);

        if(currentMode == Mode::waypoint)
        {
            char* selChar = map_waypoint.get(x,y);
            if(selChar == NULL)
            {
                linkSelection.x = -1;
            }
            else
            {
                if(linkSelection.x == -1)
                {
                    linkSelection.x = x;
                    linkSelection.y = y;
                }
                else
                {
                    char* prevChar = map_waypoint.get(linkSelection.x, linkSelection.y);
                    if((*selChar) != (*prevChar))
                    {
                        if(!wm.isAdjacent(*prevChar, *selChar))
                            wm.makeAdjacent(*prevChar, *selChar);
                        else
                            wm.unmakeAdjacent(*prevChar, *selChar);
                    }
                    linkSelection.x = -1;
                }
            }
        }
        else if(currentMode == Mode::ewconnect)
        {
            if(entitySelection.x == -1)
            {
                char* selChar = map_entities.get(x,y);
                if(selChar != NULL)
                {
                    entitySelection.x = x;
                    entitySelection.y = y;
                }
            }
            else
            {
                char* selChar = map_waypoint.get(x,y);
                if(selChar == NULL)
                {
                    entitySelection.x = -1;
                }
                else
                {
                    char* prevChar = map_entities.get(entitySelection.x, entitySelection.y);
                    bool contains = false;
                    for(auto liter = ewmap[*prevChar].begin(); liter != ewmap[*prevChar].end(); ++liter)
                    {
                        if(*liter == *selChar)
                        {
                            contains = true;
                            ewmap[*prevChar].erase(liter);
                            break;
                        }
                    }
                    if(!contains)
                    {
                        ewmap[*prevChar].push_back(*selChar);
                    }

                    entitySelection.x = -1;
                }
            }
        }
        else if(currentMode == Mode::warps)
        {
            if(warpSelection != NULL)
            {
                if(warp_destinations.get(*warpSelection).first != -1)
                    warp_destinations.remove(*warpSelection);
                warp_destinations.add(*warpSelection, x, y);
                warpSelection = NULL;
            }
            else
            {
                char* selChar = map_warps.get(x,y);
                if(selChar != NULL)
                    warpSelection = selChar;
            }
        }
    }
    else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A)
    {
        if(--eSymbolSelection < 0)
            eSymbolSelection = validChars.size() - 1;
    }
    else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::D)
    {
        if(++eSymbolSelection >= validChars.size())
            eSymbolSelection = 0;
    }
    else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Equal)
    {
        sf::View cView;
        cView.setSize(getContext().window->getView().getSize());
        cView.setCenter(getContext().window->getView().getCenter());
        cView.zoom(0.5f);
        getContext().window->setView(cView);
    }
    else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Dash)
    {
        sf::View cView;
        cView.setSize(getContext().window->getView().getSize());
        cView.setCenter(getContext().window->getView().getCenter());
        cView.zoom(2.0f);
        getContext().window->setView(cView);
    }

    return true;
}

