
#ifndef WAYPOINT_HPP
#define WAYPOINT_HPP

#include <cstdlib>

#include <vector>
#include <set>
#include <map>
#include <string>

class Waypoint
{
public:
    Waypoint(char symbol);

    char symbol;
    std::set<Waypoint*> adjacent;
};

class WaypointManager
{
public:
    WaypointManager();

    bool addWaypoint(const char& symbol);
    bool removeWaypoint(const char& symbol);
    Waypoint* getWaypoint(const char& symbol);
    bool makeAdjacent(const char& s0, const char& s1);
    bool unmakeAdjacent(const char& s0, const char& s1);
    bool isAdjacent(const char& s0, const char& s1);
    std::string getCurrentChars();
    std::set<std::pair<Waypoint,Waypoint> > getEdges();
private:
    std::map<char,Waypoint> waypoints;
};

#endif
