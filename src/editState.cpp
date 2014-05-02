
#include "editState.hpp"

EditState::EditState(StateStack& stack, Context context)
: State(stack, context),
currentMode(Mode::layer0),
kmap(),
map_layer0(),
map_layer1(),
map_layer2(),
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
doorSymbol("Derp",getContext().fonts->get(Fonts::ClearSans),10),
keySymbol("Derps",getContext().fonts->get(Fonts::ClearSans),10),
dkLine(sf::LinesStrip, 2),
keySelection(NULL),
warpLine(sf::LinesStrip, 2),
warpSelection(NULL)
{
    sheet.setTexture(getContext().textures->get(Textures::TileSheet));

    isize = getContext().textures->get(Textures::TileSheet).getSize();

    // Parse keyfile
    Parser p;
    {
        Parser::Context c = p.parse(*(getContext().oFile) + K_SUFFIX, Parser::keyfile);
        if(!c.success)
            throw std::runtime_error("Keyfile parsing error.");
        tsize = c.tilesize;
        kmap = c.kmap;
    }

    // create window for selecting tiles
    getContext().twindow->create(sf::VideoMode(isize.x,isize.y), "layer0");

    // parse layer 0
    {
        Parser::Context c = p.parse(*(getContext().oFile) + L0_SUFFIX, Parser::level);
        if(c.success)
            map_layer0 = c.map;
    }

    // parse layer 1

    {
        Parser::Context c = p.parse(*(getContext().oFile) + L1_SUFFIX, Parser::level);
        if(c.success)
            map_layer1 = c.map;
    }

    // parse layer 2

    {
        Parser::Context c = p.parse(*(getContext().oFile) + L2_SUFFIX, Parser::level);
        if(c.success)
            map_layer2 = c.map;
    }

    // parse waypoints
    {
        Parser::Context c = p.parse(*(getContext().oFile) + W_SUFFIX, Parser::waypoint);
        if(c.success)
        {
            auto list = c.wm.getCurrentChars();
            for(unsigned int x = 0; x < list.size(); ++x)
                wm.addWaypoint(list[x]);

            for(unsigned int x = 0; x < list.size(); ++x)
                for(unsigned int y = 0; y < list.size(); ++y)
                {
                    if(x == y)
                        continue;
                    if(c.wm.isAdjacent(list[x],list[y]))
                        wm.makeAdjacent(list[x],list[y]);
                }
            map_waypoint = c.map;
        }
    }

    // parse obstacles
    {
        Parser::Context c = p.parse(*(getContext().oFile) + O_SUFFIX, Parser::obstacle);
        if(c.success)
            map_obstacles = c.map;
    }

    // parse entities
    {
        Parser::Context c = p.parse(*(getContext().oFile) + E_SUFFIX, Parser::entity);
        if(c.success)
        {
            ewmap = c.ewmap;
            map_entities = c.map;
        }
    }

    // parse warps
    {
        Parser::Context c = p.parse(*(getContext().oFile) + WA_SUFFIX, Parser::warp);
        if(c.success)
        {
            warp_destinations = c.warpDestination;
            map_warps = c.map;
        }
    }

    // parse doors
    {
        Parser::Context c = p.parse(*(getContext().oFile) + D_SUFFIX, Parser::door);
        if(c.success)
            map_doors = c.map;
    }

    // parse keys to doors
    {
        Parser::Context c = p.parse(*(getContext().oFile) + DK_SUFFIX, Parser::key);
        if(c.success)
        {
            ktod = c.ktod;
            dtok = c.dtok;
            map_keys = c.map;
        }
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
//    cView.move(0,-600.f);
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
    
    doorSymbol.setCharacterSize((unsigned int) tsize);
    doorSymbol.setStyle(sf::Text::Regular);
    doorSymbol.setColor(sf::Color(85,18,0));
    
    keySymbol.setCharacterSize((unsigned int) tsize);
    keySymbol.setStyle(sf::Text::Regular);
    keySymbol.setColor(sf::Color::Yellow);

    dkLine[0].color = sf::Color::Yellow;
    dkLine[1].color = sf::Color(85,18,0);

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
    if(currentMode != Mode::layer1 && currentMode != Mode::layer2)
    {
        for(int y=map_layer0.getMinSize().second; y <= map_layer0.getMaxSize().second; ++y)
        {
            auto row = map_layer0.getRow(y);
            for(auto rowIter = row.begin(); rowIter != row.end(); ++rowIter)
            {
                auto pair = kmap.find(rowIter->obj)->second;
                left = pair.first * tsize;
                top = pair.second * tsize;
                sheet.setTextureRect(sf::IntRect(left, top, tsize, tsize));
                sheet.setPosition(rowIter->x * tsize, rowIter->y * tsize);
                getContext().window->draw(sheet);
            }
        }
    }

    // draw layer1
    if(currentMode != Mode::layer0 && currentMode != Mode::layer2)
    {
        for(int y=map_layer1.getMinSize().second; y <= map_layer1.getMaxSize().second; ++y)
        {
            auto row = map_layer1.getRow(y);
            for(auto rowIter = row.begin(); rowIter != row.end(); ++rowIter)
            {
                auto pair = kmap.find(rowIter->obj)->second;
                left = pair.first * tsize;
                top = pair.second * tsize;
                sheet.setTextureRect(sf::IntRect(left, top, tsize, tsize));
                sheet.setPosition(rowIter->x * tsize, rowIter->y * tsize);
                getContext().window->draw(sheet);
            }
        }
    }

    // draw layer2
    if(currentMode != Mode::layer0 && currentMode != Mode::layer1)
    {
        for(int y=map_layer2.getMinSize().second; y <= map_layer2.getMaxSize().second; ++y)
        {
            auto row = map_layer2.getRow(y);
            for(auto rowIter = row.begin(); rowIter != row.end(); ++rowIter)
            {
                auto pair = kmap.find(rowIter->obj)->second;
                left = pair.first * tsize;
                top = pair.second * tsize;
                sheet.setTextureRect(sf::IntRect(left, top, tsize, tsize));
                sheet.setPosition(rowIter->x * tsize, rowIter->y * tsize);
                getContext().window->draw(sheet);
            }
        }
    }

    // draw waypoints
    if(currentMode == Mode::waypoint || currentMode == Mode::ewconnect)
    {
        for(int j=map_waypoint.getMinSize().second; j <= map_waypoint.getMaxSize().second; ++j)
        {
            auto row = map_waypoint.getRow(j);
            for(auto riter = row.begin(); riter != row.end(); ++riter)
            {
                waypointMarker.setPosition((float)(riter->x * tsize),(float)(riter->y * tsize));
                getContext().window->draw(waypointMarker);
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

            auto pair = map_waypoint.get(a);
            aCoords.x = pair.first;
            aCoords.y = pair.second;

            pair = map_waypoint.get(b);
            bCoords.x = pair.first;
            bCoords.y = pair.second;

            adjLine[0].position.x = (float) (aCoords.x * tsize) + ((float)tsize)/2.0f;
            adjLine[0].position.y = (float) (aCoords.y * tsize) + ((float)tsize)/2.0f;
            adjLine[1].position.x = (float) (bCoords.x * tsize) + ((float)tsize)/2.0f;
            adjLine[1].position.y = (float) (bCoords.y * tsize) + ((float)tsize)/2.0f;
            getContext().window->draw(adjLine);
        }

        // Drawing new connection to mouse
        if(linkSelection.x != -1)
        {
            sf::Vector2i mpos = sf::Mouse::getPosition(*(getContext().window));
            sf::Vector2f gpos = getContext().window->mapPixelToCoords(mpos);

            adjLine[0].position.x = (float) (linkSelection.x * tsize) + ((float)tsize)/2.0f;
            adjLine[0].position.y = (float) (linkSelection.y * tsize) + ((float)tsize)/2.0f;
            adjLine[1].position = gpos;
            getContext().window->draw(adjLine);
        }
    }

    // draw obstacles
    if(currentMode == Mode::obstacles)
    {
        for(int j=map_obstacles.getMinSize().second; j <= map_obstacles.getMaxSize().second; ++j)
        {
            auto row = map_obstacles.getRow(j);
            for(auto riter = row.begin(); riter != row.end(); ++riter)
            {
                obstacleIndicator.setPosition((float)(riter->x * tsize),(float)(riter->y * tsize));
                getContext().window->draw(obstacleIndicator);
            }
        }
    }

    // draw entities
    if(currentMode == Mode::entities || currentMode == Mode::ewconnect)
    {
        for(int j=map_entities.getMinSize().second; j <= map_entities.getMaxSize().second; ++j)
        {
            auto row = map_entities.getRow(j);
            for(auto riter = row.begin(); riter != row.end(); ++riter)
            {
                entitySymbol.setString(riter->obj);
                entitySymbol.setPosition((float)(riter->x * tsize), (float)(riter->y * tsize));
                getContext().window->draw(entitySymbol);
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

                auto pair = map_entities.get(entity);
                ecoords.x = pair.first;
                ecoords.y = pair.second;

                pair = map_waypoint.get(waypoint);
                wcoords.x = pair.first;
                wcoords.y = pair.second;

                ewLine[0].position.x = ((float) ecoords.x * tsize) + ((float)tsize)/2.0f;
                ewLine[0].position.y = ((float) ecoords.y * tsize) + ((float)tsize)/2.0f;
                ewLine[1].position.x = ((float) wcoords.x * tsize) + ((float)tsize)/2.0f;
                ewLine[1].position.y = ((float) wcoords.y * tsize) + ((float)tsize)/2.0f;
                
                getContext().window->draw(ewLine);
            }
        }

        if(entitySelection.x != -1)
        {
            sf::Vector2i mpos = sf::Mouse::getPosition(*(getContext().window));
            sf::Vector2f gpos = getContext().window->mapPixelToCoords(mpos);

            ewLine[0].position.x = (float) (entitySelection.x * tsize) + ((float)tsize)/2.0f;
            ewLine[0].position.y = (float) (entitySelection.y * tsize) + ((float)tsize)/2.0f;
            ewLine[1].position = gpos;

            getContext().window->draw(ewLine);
        }
    }

    // draw warps
    if(currentMode == Mode::warps)
    {
        for(int j=map_warps.getMinSize().second; j <= map_warps.getMaxSize().second; ++j)
        {
            auto row = map_warps.getRow(j);
            for(auto riter = row.begin(); riter != row.end(); ++riter)
            {
                warpSymbol.setString(riter->obj);
                warpSymbol.setPosition((float)(riter->x * tsize), (float)(riter->y * tsize));
                getContext().window->draw(warpSymbol);
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
            warpLine[0].position.y = (float) (coords.second * tsize) + ((float)tsize)/2.0f;
            warpLine[1].position = gpos;

            getContext().window->draw(warpLine);
        }

        for(int j=warp_destinations.getMinSize().second; j <= warp_destinations.getMaxSize().second; ++j)
        {
            auto row = warp_destinations.getRow(j);
            for(auto riter = row.begin(); riter != row.end(); ++riter)
            {
                auto coords = map_warps.get(riter->obj);

                warpLine[0].position.x = (float) (coords.first * tsize) + ((float)tsize)/2.0f;
                warpLine[0].position.y = (float) (coords.second * tsize) + ((float)tsize)/2.0f;
                warpLine[1].position.x = (float) (riter->x * tsize) + ((float)tsize)/2.0f;
                warpLine[1].position.y = (float) (riter->y * tsize) + ((float)tsize)/2.0f;

                getContext().window->draw(warpLine);
            }
        }
    }

    // draw doors
    if(currentMode == Mode::door || currentMode == Mode::key)
    {
        for(int j=map_doors.getMinSize().second; j <= map_doors.getMaxSize().second; ++j)
        {
            auto row = map_doors.getRow(j);
            for(auto riter = row.begin(); riter != row.end(); ++riter)
            {
                doorSymbol.setString(riter->obj);
                doorSymbol.setPosition((float)(riter->x * tsize), (float)(riter->y * tsize));
                getContext().window->draw(doorSymbol);
            }
        }
    }

    // draw keys
    if(currentMode == Mode::key)
    {
        for(int j=map_keys.getMinSize().second; j <= map_keys.getMaxSize().second; ++j)
        {
            auto row = map_keys.getRow(j);
            for(auto riter = row.begin(); riter != row.end(); ++riter)
            {
                keySymbol.setString(riter->obj);
                keySymbol.setPosition((float)(riter->x * tsize), (float)(riter->y * tsize));
                getContext().window->draw(keySymbol);
            }
        }

        for(auto iter = ktod.begin(); iter != ktod.end(); ++iter)
        {
            auto kcoords = map_keys.get(iter->first);
            auto dcoords = map_doors.get(iter->second);

            dkLine[0].position.x = (float) (kcoords.first * tsize) + ((float)tsize)/2.0f;
            dkLine[0].position.y = (float) (kcoords.second * tsize) + ((float)tsize)/2.0f;
            dkLine[1].position.x = (float) (dcoords.first * tsize) + ((float)tsize)/2.0f;
            dkLine[1].position.y = (float) (dcoords.second * tsize) + ((float)tsize)/2.0f;

            getContext().window->draw(dkLine);
        }

        if(keySelection != NULL)
        {
            sf::Vector2i mpos = sf::Mouse::getPosition(*(getContext().window));
            sf::Vector2f gpos = getContext().window->mapPixelToCoords(mpos);

            auto kcoords = map_keys.get(*keySelection);

            dkLine[0].position.x = (float) (kcoords.first * tsize) + ((float)tsize)/2.0f;
            dkLine[0].position.y = (float) (kcoords.second * tsize) + ((float)tsize)/2.0f;
            dkLine[1].position = gpos;

            getContext().window->draw(dkLine);
        }
    }

    // draw grid
    for(float y=t; y <= t + getContext().window->getView().getSize().y; y += (float) tsize)
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
    if(currentMode != Mode::entities && currentMode != Mode::warps
        && currentMode != Mode::door && currentMode != Mode::key)
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
    else if(currentMode == Mode::door)
    {
        doorSymbol.setString(validChars[eSymbolSelection]);
        doorSymbol.setPosition(0.0f, 0.0f);
        getContext().twindow->draw(doorSymbol);
    }
    else if(currentMode == Mode::key)
    {
        keySymbol.setString(validChars[eSymbolSelection]);
        keySymbol.setPosition(0.0f, 0.0f);
        getContext().twindow->draw(keySymbol);
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
        int y = (int)(gpos.y / tsize);
        if(gpos.x < 0.0f)
            --x;
        if(gpos.y < 0.0f)
            --y;

        if(currentMode == Mode::layer0)
            map_layer0.remove(x,y);
        else if(currentMode == Mode::layer1)
            map_layer1.remove(x,y);
        else if(currentMode == Mode::layer2)
            map_layer2.remove(x,y);
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
        else if(currentMode == Mode::door)
        {
            char* d = map_doors.get(x,y);
            if(d != NULL)
            {
                auto iter = dtok.find(*d);
                if(iter != dtok.end())
                {
                    dtok.erase(*d);
                    ktod.erase(iter->second);
                }
                map_doors.remove(x,y);
            }
        }
        else if(currentMode == Mode::key)
        {
            char* k = map_keys.get(x,y);
            if(k != NULL)
            {
                auto iter = ktod.find(*k);
                if(iter != ktod.end())
                {
                    ktod.erase(*k);
                    dtok.erase(iter->second);
                }
                map_keys.remove(x,y);
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
        int y = (int)(gpos.y / tsize);
        if(gpos.x < 0.0f)
            --x;
        if(gpos.y < 0.0f)
            --y;
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
        else if(currentMode == Mode::layer2 && selChar != ' ')
        {
            if(map_layer2.get(x,y) != NULL)
                map_layer2.remove(x,y);
            map_layer2.add(selChar, x, y);
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
        else if(currentMode == Mode::door)
        {
            char* d = map_doors.get(x,y);
            if(d != NULL)
            {
                auto iter = dtok.find(*d);
                if(iter != dtok.end())
                {
                    dtok.erase(*d);
                    ktod.erase(iter->second);
                }
                map_doors.remove(x,y);
            }
            map_doors.add(validChars[eSymbolSelection], x, y);
        }
        else if(currentMode == Mode::key)
        {
            char* k = map_keys.get(x,y);
            if(k != NULL)
            {
                auto iter = ktod.find(*k);
                if(iter != ktod.end())
                {
                    ktod.erase(*k);
                    dtok.erase(iter->second);
                }
                map_keys.remove(x,y);
            }
            map_keys.add(validChars[eSymbolSelection], x, y);
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
        // get max/min
        int maxX;
        int maxY;
        int minX;
        int minY;

        maxX = map_layer0.getMaxSize().first;
        maxY = map_layer0.getMaxSize().second;
        minX = map_layer0.getMinSize().first;
        minY = map_layer0.getMinSize().second;

        if(map_layer1.getSize() != 0)
        {
            if(maxX < map_layer1.getMaxSize().first)
                maxX = map_layer1.getMaxSize().first;
            if(maxY < map_layer1.getMaxSize().second)
                maxY = map_layer1.getMaxSize().second;
            if(minX > map_layer1.getMinSize().first)
                minX = map_layer1.getMinSize().first;
            if(minY > map_layer1.getMinSize().second)
                minY = map_layer1.getMinSize().second;
        }

        if(map_layer2.getSize() != 0)
        {
            if(maxX < map_layer2.getMaxSize().first)
                maxX = map_layer2.getMaxSize().first;
            if(maxY < map_layer2.getMaxSize().second)
                maxY = map_layer2.getMaxSize().second;
            if(minX > map_layer2.getMinSize().first)
                minX = map_layer2.getMinSize().first;
            if(minY > map_layer2.getMinSize().second)
                minY = map_layer2.getMinSize().second;
        }

        if(map_waypoint.getSize() != 0)
        {
            if(maxX < map_waypoint.getMaxSize().first)
                maxX = map_waypoint.getMaxSize().first;
            if(maxY < map_waypoint.getMaxSize().second)
                maxY = map_waypoint.getMaxSize().second;
            if(minX > map_waypoint.getMinSize().first)
                minX = map_waypoint.getMinSize().first;
            if(minY > map_waypoint.getMinSize().second)
                minY = map_waypoint.getMinSize().second;
        }

        if(map_obstacles.getSize() != 0)
        {
            if(maxX < map_obstacles.getMaxSize().first)
                maxX = map_obstacles.getMaxSize().first;
            if(maxY < map_obstacles.getMaxSize().second)
                maxY = map_obstacles.getMaxSize().second;
            if(minX > map_obstacles.getMinSize().first)
                minX = map_obstacles.getMinSize().first;
            if(minY > map_obstacles.getMinSize().second)
                minY = map_obstacles.getMinSize().second;
        }

        if(map_entities.getSize() != 0)
        {
            if(maxX < map_entities.getMaxSize().first)
                maxX = map_entities.getMaxSize().first;
            if(maxY < map_entities.getMaxSize().second)
                maxY = map_entities.getMaxSize().second;
            if(minX > map_entities.getMinSize().first)
                minX = map_entities.getMinSize().first;
            if(minY > map_entities.getMinSize().second)
                minY = map_entities.getMinSize().second;
        }

        if(map_warps.getSize() != 0)
        {
            if(maxX < map_warps.getMaxSize().first)
                maxX = map_warps.getMaxSize().first;
            if(maxY < map_warps.getMaxSize().second)
                maxY = map_warps.getMaxSize().second;
            if(minX > map_warps.getMinSize().first)
                minX = map_warps.getMinSize().first;
            if(minY > map_warps.getMinSize().second)
                minY = map_warps.getMinSize().second;
        }

        if(map_doors.getSize() != 0)
        {
            if(maxX < map_doors.getMaxSize().first)
                maxX = map_doors.getMaxSize().first;
            if(maxY < map_doors.getMaxSize().second)
                maxY = map_doors.getMaxSize().second;
            if(minX > map_doors.getMinSize().first)
                minX = map_doors.getMinSize().first;
            if(minY > map_doors.getMinSize().second)
                minY = map_doors.getMinSize().second;
        }

        if(map_keys.getSize() != 0)
        {
            if(maxX < map_keys.getMaxSize().first)
                maxX = map_keys.getMaxSize().first;
            if(maxY < map_keys.getMaxSize().second)
                maxY = map_keys.getMaxSize().second;
            if(minX > map_keys.getMinSize().first)
                minX = map_keys.getMinSize().first;
            if(minY > map_keys.getMinSize().second)
                minY = map_keys.getMinSize().second;
        }

        std::cout << minX << " " << minY << " " << maxX << " " << maxY << "\n"; //TODO DEBUG

        std::fstream of;

        // write layer0
        of.open(*(getContext().oFile) + L0_SUFFIX, std::ios::trunc | std::ios::out);
        if(!of.is_open())
        {
            of.clear();
            of.open(*(getContext().oFile) + L0_SUFFIX, std::ios::out);
            of.close();
            of.open(*(getContext().oFile) + L0_SUFFIX, std::ios::trunc | std::ios::out);
        }

        for(int y=minY; y <= map_layer0.getMaxSize().second; ++y)
        {
            for(int x=minX; x <= map_layer0.getMaxSize().first; ++x)
            {
                char* c = map_layer0.get(x,y);
                if(c == NULL)
                    of << ' ';
                else
                    of << *c;
            }
            of << '\n';
        }

        of.flush();
        of.close();

        // write layer1
        of.clear();
        of.open(*(getContext().oFile) + L1_SUFFIX, std::ios::trunc | std::ios::out);
        if(!of.is_open())
        {
            of.clear();
            of.open(*(getContext().oFile) + L1_SUFFIX, std::ios::out);
            of.close();
            of.open(*(getContext().oFile) + L1_SUFFIX, std::ios::trunc | std::ios::out);
        }

        for(int y=minY; y <= map_layer1.getMaxSize().second; ++y)
        {
            for(int x=minX; x <= map_layer1.getMaxSize().first; ++x)
            {
                char* c = map_layer1.get(x,y);
                if(c == NULL)
                    of << ' ';
                else
                    of << *c;
            }
            of << '\n';
        }

        of.flush();
        of.close();

        // write layer2
        of.open(*(getContext().oFile) + L2_SUFFIX, std::ios::trunc | std::ios::out);
        if(!of.is_open())
        {
            of.clear();
            of.open(*(getContext().oFile) + L2_SUFFIX, std::ios::out);
            of.close();
            of.open(*(getContext().oFile) + L2_SUFFIX, std::ios::trunc | std::ios::out);
        }

        for(int y=minY; y <= map_layer2.getMaxSize().second; ++y)
        {
            for(int x=minX; x <= map_layer2.getMaxSize().first; ++x)
            {
                char* c = map_layer2.get(x,y);
                if(c == NULL)
                    of << ' ';
                else
                    of << *c;
            }
            of << '\n';
        }

        of.flush();
        of.close();

        // write waypoints
        of.clear();
        of.open(*(getContext().oFile) + W_SUFFIX, std::ios::trunc | std::ios::out);
        if(!of.is_open())
        {
            of.clear();
            of.open(*(getContext().oFile) + W_SUFFIX, std::ios::out);
            of.close();
            of.open(*(getContext().oFile) + W_SUFFIX, std::ios::trunc | std::ios::out);
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

        for(int y=minY; y <= map_waypoint.getMaxSize().second; ++y)
        {
            for(int x=minX; x <= map_waypoint.getMaxSize().first; ++x)
            {
                char* c = map_waypoint.get(x,y);
                if(c == NULL)
                    of << ' ';
                else
                    of << *c;
            }
            of << '\n';
        }

        of.flush();
        of.close();

        // write obstacles
        of.clear();
        of.open(*(getContext().oFile) + O_SUFFIX, std::ios::trunc | std::ios::out);
        if(!of.is_open())
        {
            of.clear();
            of.open(*(getContext().oFile) + O_SUFFIX, std::ios::out);
            of.close();
            of.open(*(getContext().oFile) + O_SUFFIX, std::ios::trunc | std::ios::out);
        }
        of.flush();

        for(int y=minY; y <= map_obstacles.getMaxSize().second; ++y)
        {
            for(int x=minX; x <= map_obstacles.getMaxSize().first; ++x)
            {
                if(map_obstacles.get(x,y) == NULL)
                    of << ' ';
                else
                    of << 'o';
            }
            of << '\n';
        }

        of.close();

        // write entities
        of.clear();
        of.open(*(getContext().oFile) + E_SUFFIX, std::ios::trunc | std::ios::out);
        if(!of.is_open())
        {
            of.clear();
            of.open(*(getContext().oFile) + E_SUFFIX, std::ios::out);
            of.close();
            of.open(*(getContext().oFile) + E_SUFFIX, std::ios::trunc | std::ios::out);
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

        for(int y=minY; y <= map_entities.getMaxSize().second; ++y)
        {
            for(int x=minX; x <= map_entities.getMaxSize().first; ++x)
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
        of.open(*(getContext().oFile) + WA_SUFFIX, std::ios::trunc | std::ios::out);
        if(!of.is_open())
        {
            of.clear();
            of.open(*(getContext().oFile) + WA_SUFFIX, std::ios::out);
            of.close();
            of.open(*(getContext().oFile) + WA_SUFFIX, std::ios::trunc | std::ios::out);
        }

        for(int j=warp_destinations.getMinSize().second; j <= warp_destinations.getMaxSize().second; ++j)
        {
            auto row = warp_destinations.getRow(j);
            for(auto riter = row.begin(); riter != row.end(); ++riter)
            {
                of << riter->obj << " " << riter->x - minX<< " " << riter->y - minY << "\n";
            }
        }

        of << "#\n";

        for(int y=minY; y <= map_warps.getMaxSize().second; ++y)
        {
            for(int x=minX; x <= map_warps.getMaxSize().first; ++x)
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

        // write doors
        of.clear();
        of.open(*(getContext().oFile) + D_SUFFIX, std::ios::trunc | std::ios::out);
        if(!of.is_open())
        {
            of.clear();
            of.open(*(getContext().oFile) + D_SUFFIX, std::ios::out);
            of.close();
            of.open(*(getContext().oFile) + D_SUFFIX, std::ios::trunc | std::ios::out);
        }

        for(int y=minY; y <= map_doors.getMaxSize().second; ++y)
        {
            for(int x=minX; x <= map_doors.getMaxSize().first; ++x)
            {
                char* c = map_doors.get(x,y);
                if(c == NULL)
                    of << ' ';
                else
                    of << *c;
            }
            of << '\n';
        }

        of.flush();
        of.close();

        // write keys
        of.clear();
        of.open(*(getContext().oFile) + DK_SUFFIX, std::ios::trunc | std::ios::out);
        if(!of.is_open())
        {
            of.clear();
            of.open(*(getContext().oFile) + DK_SUFFIX, std::ios::out);
            of.close();
            of.open(*(getContext().oFile) + DK_SUFFIX, std::ios::trunc | std::ios::out);
        }

        for(auto iter = ktod.begin(); iter != ktod.end(); ++iter)
        {
            of << iter->first << ' ' << iter->second << '\n';
        }
        of << "#\n";

        for(int y=minY; y <= map_keys.getMaxSize().second; ++y)
        {
            for(int x=minX; x <= map_keys.getMaxSize().first; ++x)
            {
                char* c = map_keys.get(x,y);
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
    else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Period)
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
            currentMode = Mode::layer2;
            getContext().twindow->setTitle("layer2");
            break;
        case Mode::layer2:
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
            currentMode = Mode::door;
            getContext().twindow->setTitle("door");
            break;
        case Mode::door:
            currentMode = Mode::key;
            getContext().twindow->setTitle("key");
            break;
        case Mode::key:
            currentMode = Mode::layer0;
            getContext().twindow->setTitle("layer0");
            break;
        default:
            currentMode = Mode::layer0;
            getContext().twindow->setTitle("layer0");
            break;
        }
    }
    else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Comma)
    {
        linkSelection.x = -1;
        warpSelection = NULL;
        switch(currentMode)
        {
        case Mode::layer0:
            currentMode = Mode::key;
            getContext().twindow->setTitle("key");
            break;
        case Mode::layer1:
            currentMode = Mode::layer0;
            getContext().twindow->setTitle("layer0");
            break;
        case Mode::layer2:
            currentMode = Mode::layer1;
            getContext().twindow->setTitle("layer1");
            break;
        case Mode::waypoint:
            currentMode = Mode::layer2;
            getContext().twindow->setTitle("layer2");
            break;
        case Mode::obstacles:
            currentMode = Mode::waypoint;
            getContext().twindow->setTitle("waypoint");
            break;
        case Mode::entities:
            currentMode = Mode::obstacles;
            getContext().twindow->setTitle("obstacles");
            break;
        case Mode::ewconnect:
            currentMode = Mode::entities;
            getContext().twindow->setTitle("entities");
            break;
        case Mode::warps:
            currentMode = Mode::ewconnect;
            getContext().twindow->setTitle("entity_waypoint_connect");
            break;
        case Mode::door:
            currentMode = Mode::warps;
            getContext().twindow->setTitle("warps");
            break;
        case Mode::key:
            currentMode = Mode::door;
            getContext().twindow->setTitle("door");
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
        int y = (int)(gpos.y / tsize);
        if(gpos.x < 0.0f)
            --x;
        if(gpos.y < 0.0f)
            --y;

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
        else if(currentMode == Mode::key)
        {
            if(keySelection == NULL)
            {
                char* selChar = map_keys.get(x,y);
                keySelection = selChar;
            }
            else
            {
                char* door = map_doors.get(x,y);
                if(door != NULL)
                {
                    {
                        auto iter = dtok.find(*door);
                        if(iter != dtok.end())
                        {
                            ktod.erase(iter->second);
                            dtok.erase(iter);
                        }
                    }

                    {
                        auto iter = ktod.find(*keySelection);
                        if(iter != ktod.end())
                        {
                            dtok.erase(iter->second);
                            ktod.erase(iter);
                        }
                    }
                    ktod.insert(std::pair<char,char>(*keySelection, *door));
                    dtok.insert(std::pair<char,char>(*door, *keySelection));
                }

                keySelection = NULL;
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

