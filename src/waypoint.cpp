
#include "waypoint.hpp"

Waypoint::Waypoint(char symbol) :
symbol(symbol),
adjacent()
{}

bool operator< (const Waypoint& lhs, const Waypoint& rhs)
{
    return lhs.symbol < rhs.symbol;
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
        auto iter = wp->adjacent.begin();
        while(iter != wp->adjacent.end())
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
