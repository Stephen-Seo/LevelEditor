
#include "waypoint.hpp"

Waypoint::Waypoint(char symbol) :
symbol(symbol),
adjacent()
{}

bool operator< (const Waypoint& lhs, const Waypoint& rhs)
{
    return lhs.symbol < rhs.symbol;
}

bool operator== (const Waypoint& lhs, const Waypoint& rhs)
{
    return lhs.symbol == rhs.symbol;
}

WaypointManager::WaypointManager() :
waypoints()
{}

bool WaypointManager::addWaypoint(const char& symbol)
{
    if(getWaypoint(symbol) == NULL)
    {
        waypoints.insert(std::pair<char, Waypoint>(symbol, Waypoint(symbol)));
        return true;
    }
    else
        return false;
}

bool WaypointManager::removeWaypoint(const char& symbol)
{
    Waypoint* wp = getWaypoint(symbol);
    if(wp == NULL)
        return false;

    {
        for(auto iter = wp->adjacent.begin(); iter != wp->adjacent.end(); ++iter)
        {
            (*iter)->adjacent.erase(wp);
        }
    }

    waypoints.erase(symbol);
    return true;
}

Waypoint* WaypointManager::getWaypoint(const char& symbol)
{
    auto iter = waypoints.find(symbol);
    if(iter != waypoints.end())
        return &(iter->second);
    else
        return NULL;
}

bool WaypointManager::makeAdjacent(const char& s0, const char& s1)
{
    Waypoint* wp0 = getWaypoint(s0);
    Waypoint* wp1 = getWaypoint(s1);

    if(wp0 == NULL || wp1 == NULL)
    {
        return false;
    }

    wp0->adjacent.insert(wp1);
    wp1->adjacent.insert(wp0);
    return true;
}

bool WaypointManager::unmakeAdjacent(const char& s0, const char& s1)
{
    Waypoint* wp0 = getWaypoint(s0);
    Waypoint* wp1 = getWaypoint(s1);

    if(wp0 == NULL || wp1 == NULL)
    {
        return false;
    }

    wp0->adjacent.erase(wp1);
    wp1->adjacent.erase(wp0);
    return true;
}

bool WaypointManager::isAdjacent(const char& s0, const char& s1)
{
    Waypoint* wp0 = getWaypoint(s0);
    Waypoint* wp1 = getWaypoint(s1);

    bool adjacent = false;
    for(auto iter = wp0->adjacent.begin(); iter != wp0->adjacent.end(); ++iter)
    {
        if((**iter) == (*wp1))
        {
            adjacent = true;
            break;
        }
    }
    return adjacent;
}

std::string WaypointManager::getCurrentChars()
{
    std::string list;
    for(auto iter = waypoints.begin(); iter != waypoints.end(); ++iter)
    {
        list.append(1, iter->first);
    }
    return list;
}

bool operator< (const std::pair<Waypoint,Waypoint>& lhs, const std::pair<Waypoint,Waypoint>& rhs)
{
    if(lhs.first.symbol == rhs.first.symbol && lhs.second.symbol == rhs.second.symbol)
        return false;
    if(lhs.first.symbol == rhs.second.symbol && lhs.second.symbol == rhs.first.symbol)
        return false;
    return lhs.first.symbol < rhs.first.symbol || (lhs.first.symbol == rhs.first.symbol && lhs.second.symbol < rhs.second.symbol);
}

std::set<std::pair<Waypoint,Waypoint> > WaypointManager::getEdges()
{
    std::set<std::pair<Waypoint,Waypoint> > edges;
    for(auto witer = waypoints.begin(); witer != waypoints.end(); ++witer)
    {
        Waypoint a = Waypoint(witer->second.symbol);
        for(auto aiter = witer->second.adjacent.begin(); aiter != witer->second.adjacent.end(); ++aiter)
        {
            Waypoint b = Waypoint((*aiter)->symbol);
            edges.insert(std::pair<Waypoint,Waypoint>(a,b));
        }
    }
    return edges;
}
