
#include "leCommands.hpp"

TileCreateCommand::TileCreateCommand(char c, int x, int y, CoordinateMap<char>* map) :
c(c),
x(x),
y(y),
map(map),
prevExists(false)
{}

void TileCreateCommand::execute()
{
    char* p = map->get(x,y);
    if(p != NULL)
    {
        prev = *p;
        prevExists = true;
        map->remove(x,y);
    }
    map->add(c,x,y);
}

void TileCreateCommand::undo()
{
    map->remove(x,y);
    if(prevExists)
        map->add(prev,x,y);
}

TileDeleteCommand::TileDeleteCommand(int x, int y, CoordinateMap<char>* map) :
x(x),
y(y),
map(map)
{}

void TileDeleteCommand::execute()
{
    c = *(map->get(x,y));
    map->remove(x,y);
}

void TileDeleteCommand::undo()
{
    map->add(c,x,y);
}

WaypointCreateCommand::WaypointCreateCommand(char c, int x, int y, WaypointManager* wm, CoordinateMap<char>* map) :
c(c),
x(x),
y(y),
wm(wm),
map(map)
{}

void WaypointCreateCommand::execute()
{
    map->add(c,x,y);
    wm->addWaypoint(c);
}

void WaypointCreateCommand::undo()
{
    map->remove(x,y);
    wm->removeWaypoint(c);
}

WaypointDeleteCommand::WaypointDeleteCommand(char c, WaypointManager* wm, CoordinateMap<char>* map, std::map<char, std::list<char> >* ewmap) :
c(c),
wm(wm),
map(map),
ewmap(ewmap)
{
    auto pair = map->get(c);
    x = pair.x;
    y = pair.y;
}

WaypointDeleteCommand::WaypointDeleteCommand(int x, int y, WaypointManager* wm, CoordinateMap<char>* map, std::map<char,  std::list<char> >* ewmap) :
x(x),
y(y),
wm(wm),
map(map),
ewmap(ewmap)
{
    c = *(map->get(x,y));
}

void WaypointDeleteCommand::execute()
{
    map->remove(x,y);
    for(auto iter = wm->getWaypoint(c)->adjacent.begin(); iter != wm->getWaypoint(c)->adjacent.end(); ++iter)
    {
        adjacent.insert((*iter)->symbol);
    }
    wm->removeWaypoint(c);
    for(auto iter = ewmap->begin(); iter != ewmap->end(); ++iter)
    {
        for(auto liter = iter->second.begin(); liter != iter->second.end(); ++liter)
        {
            if(*liter == c)
            {
                entity.push_back(iter->first);
                liter = iter->second.erase(liter);
                if(liter != iter->second.end())
                    nextChar.push_back(*liter);
                else
                    nextChar.push_back(' ');
                break;
            }
        }
    }
}

void WaypointDeleteCommand::undo()
{
    map->add(c,x,y);
    wm->addWaypoint(c);
    for(auto iter = adjacent.begin(); iter != adjacent.end(); ++iter)
        wm->makeAdjacent(c, *iter);
    adjacent.clear();

    {
        auto eiter = entity.begin();
        auto niter = nextChar.begin();
        while(eiter != entity.end() && niter != nextChar.end())
        {
            auto iter = ewmap->find(*eiter);
            if(*niter != ' ')
            {
                for(auto liter = iter->second.begin(); liter != iter->second.end(); ++liter)
                {
                    if(*liter == *niter)
                    {
                        iter->second.insert(liter, c);
                        break;
                    }
                }
            }
            else
            {
                iter->second.push_back(c);
            }

            ++eiter;
            ++niter;
        }
    }
    entity.clear();
    nextChar.clear();
}

WaypointLinkCommand::WaypointLinkCommand(char waypointOne, char waypointTwo, WaypointManager* wm, bool unlink) :
waypointOne(waypointOne),
waypointTwo(waypointTwo),
wm(wm),
unlink(unlink)
{}

void WaypointLinkCommand::execute()
{
    if(!unlink)
        wm->makeAdjacent(waypointOne,waypointTwo);
    else
        wm->unmakeAdjacent(waypointOne,waypointTwo);
}

void WaypointLinkCommand::undo()
{
    if(!unlink)
        wm->unmakeAdjacent(waypointOne,waypointTwo);
    else
        wm->makeAdjacent(waypointOne,waypointTwo);
}

EntityCreateCommand::EntityCreateCommand(char c, int x, int y, CoordinateMap<char>* map, std::map<char, std::list<char> >* ewmap) :
c(c),
x(x),
y(y),
map(map),
ewmap(ewmap),
prevExists(false)
{}

void EntityCreateCommand::execute()
{
    {
        char* p = map->get(x,y);
        if(p != NULL)
        {
            prev = *p;
            prevExists = true;
            map->remove(x,y);
            prevList = ewmap->find(prev)->second;
        }
    }

    map->add(c,x,y);
    ewmap->insert(std::make_pair(c, std::list<char>()));
}

void EntityCreateCommand::undo()
{
    map->remove(x,y);
    if(prevExists)
    {
        map->add(prev,x,y);
        ewmap->insert(std::make_pair(prev, prevList));
    }

    ewmap->erase(c);
}

EntityDeleteCommand::EntityDeleteCommand(int x, int y, CoordinateMap<char>* map, std::map<char, std::list<char> >* ewmap) :
x(x),
y(y),
map(map),
ewmap(ewmap)
{
    c = *(map->get(x,y));
}

void EntityDeleteCommand::execute()
{
    map->remove(x,y);
    listContents = ewmap->find(c)->second;
    ewmap->erase(c);
}

void EntityDeleteCommand::undo()
{
    map->add(c,x,y);
    ewmap->insert(std::make_pair(c, listContents));
}

EWLinkCommand::EWLinkCommand(char entity, char waypoint, std::map<char, std::list<char> >* ewmap) :
entity(entity),
waypoint(waypoint),
ewmap(ewmap),
isLinking(true)
{}

void EWLinkCommand::execute()
{
    for(auto iter = ewmap->find(entity)->second.begin(); iter != ewmap->find(entity)->second.end(); ++iter)
    {
        if(*iter == waypoint)
        {
            isLinking = false;
            iter = ewmap->find(entity)->second.erase(iter);
            if(iter == ewmap->find(entity)->second.end())
                pos = ' ';
            else
                pos = *iter;
            break;
        }
    }
    if(isLinking)
    {
        ewmap->find(entity)->second.push_back(waypoint);
    }
}

void EWLinkCommand::undo()
{
    if(isLinking)
    {
        if(waypoint == ewmap->find(entity)->second.back())
            ewmap->find(entity)->second.pop_back();
        else
            throw std::runtime_error("EWLinkCommand::undo() - waypoint char not in expected location!");
    }
    else
    {
        for(auto iter = ewmap->find(entity)->second.begin(); iter != ewmap->find(entity)->second.end(); ++iter)
        {
            if(*iter == pos)
            {
                ewmap->find(entity)->second.insert(iter, waypoint);
                break;
            }
        }
    }
}

WarpCreateCommand::WarpCreateCommand(char c, int x, int y, CoordinateMap<char>* map, CoordinateMap<char>* dest) :
c(c),
x(x),
y(y),
map(map),
dest(dest),
coords(),
prevExists(false)
{}

void WarpCreateCommand::execute()
{
    char* p = map->get(x,y);
    if(p != NULL)
    {
        prevExists = true;
        prev = *p;
        coords = map->get(prev);
        map->remove(x,y);
        if(map->get(prev).valid)
            coords.valid = false;
        else
            dest->remove(prev);
    }
    map->add(c,x,y);
}

void WarpCreateCommand::undo()
{
    map->remove(x,y);
    if(prevExists)
    {
        map->add(prev,x,y);
        if(coords.valid)
            dest->add(prev,coords.x,coords.y);
    }
}

WarpDeleteCommand::WarpDeleteCommand(int x, int y, CoordinateMap<char>* map, CoordinateMap<char>* dest) :
x(x),
y(y),
map(map),
dest(dest),
coords()
{
    c = *(map->get(x,y));
}

void WarpDeleteCommand::execute()
{
    map->remove(x,y);
    if(!map->get(c).valid)
    {
        coords = dest->get(c);
        dest->remove(c);
    }
}

void WarpDeleteCommand::undo()
{
    map->add(c,x,y);
    if(coords.valid)
    {
        dest->add(c,coords.x,coords.y);
    }
}

WarpDestCommand::WarpDestCommand(char c, int x, int y, CoordinateMap<char>* dest) :
c(c),
x(x),
y(y),
dest(dest),
prevCoords()
{}

void WarpDestCommand::execute()
{
    prevCoords = dest->get(c);
    if(prevCoords.valid)
    {
        dest->remove(c);
    }
    dest->add(c,x,y);
}

void WarpDestCommand::undo()
{
    dest->remove(c);
    if(prevCoords.valid)
    {
        dest->add(c,prevCoords.x,prevCoords.y);
    }
}

KDCreateCommand::KDCreateCommand(char c, int x, int y, CoordinateMap<char>* map, std::map<char,char>* dtok, std::map<char,char>* ktod, bool isKey) :
c(c),
x(x),
y(y),
map(map),
dtok(dtok),
ktod(ktod),
isKey(isKey),
hasLink(false),
prevExists(false)
{}

void KDCreateCommand::execute()
{
    char* p = map->get(x,y);
    if(p != NULL)
    {
        prevExists = true;
        prev = *p;
        map->remove(x,y);
        if(isKey && ktod->find(prev) != ktod->end())
        {
            hasLink = true;
            other = ktod->find(prev)->second;
            ktod->erase(prev);
            dtok->erase(other);
        }
        else if(!isKey && dtok->find(prev) != dtok->end())
        {
            hasLink = true;
            other = dtok->find(prev)->second;
            dtok->erase(prev);
            ktod->erase(other);
        }
    }
    map->add(c,x,y);
}

void KDCreateCommand::undo()
{
    map->remove(x,y);
    if(prevExists)
    {
        map->add(prev,x,y);
        if(hasLink && isKey)
        {
            ktod->insert(std::make_pair(prev,other));
            dtok->insert(std::make_pair(other,prev));
        }
        else if(hasLink && !isKey)
        {
            dtok->insert(std::make_pair(prev,other));
            ktod->insert(std::make_pair(other,prev));
        }
    }
}

KDDeleteCommand::KDDeleteCommand(int x, int y, CoordinateMap<char>* map, std::map<char,char>* dtok, std::map<char,char>* ktod, bool isKey) :
x(x),
y(y),
map(map),
dtok(dtok),
ktod(ktod),
isKey(isKey),
hasLink(false)
{
    c = *(map->get(x,y));
}

void KDDeleteCommand::execute()
{
    map->remove(x,y);
    if(isKey)
    {
        auto iter = ktod->find(c);
        if(iter != ktod->end())
        {
            hasLink = true;
            other = iter->second;
            ktod->erase(c);
            dtok->erase(other);
        }
    }
    else
    {
        auto iter = dtok->find(c);
        if(iter != dtok->end())
        {
            hasLink = true;
            other = iter->second;
            dtok->erase(c);
            ktod->erase(other);
        }
    }
}

void KDDeleteCommand::undo()
{
    map->add(c,x,y);
    if(isKey && hasLink)
    {
        ktod->insert(std::make_pair(c,other));
        dtok->insert(std::make_pair(other,c));
    }
    else if(!isKey && hasLink)
    {
        dtok->insert(std::make_pair(c,other));
        ktod->insert(std::make_pair(other,c));
    }
}

KDLinkCommand::KDLinkCommand(char key, char door, std::map<char,char>* dtok, std::map<char,char>* ktod) :
key(key),
door(door),
dtok(dtok),
ktod(ktod),
klinkExists(false),
dlinkExists(false)
{}

void KDLinkCommand::execute()
{
    {
        auto iter = dtok->find(door);
        if(iter != dtok->end())
        {
            dlinkExists = true;
            dlink = iter->second;
            ktod->erase(iter->second);
            dtok->erase(iter);
        }
    }
    {
        auto iter = ktod->find(key);
        if(iter != ktod->end())
        {
            klinkExists = true;
            klink = iter->second;
            dtok->erase(iter->second);
            ktod->erase(iter);
        }
    }

    dtok->insert(std::make_pair(door,key));
    ktod->insert(std::make_pair(key,door));
}

void KDLinkCommand::undo()
{
    dtok->erase(door);
    ktod->erase(key);

    if(dlinkExists)
    {
        dtok->insert(std::make_pair(door, dlink));
        ktod->insert(std::make_pair(dlink, door));
    }

    if(klinkExists)
    {
        ktod->insert(std::make_pair(key, klink));
        dtok->insert(std::make_pair(klink, key));
    }
}
