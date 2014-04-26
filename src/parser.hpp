
#ifndef PARSER_HPP
#define PARSER_HPP

#include <map>
#include <string>
#include <sstream>
#include <fstream>

#include "coordinateMap.hpp"
#include "waypoint.hpp"

class Parser
{
public:
    enum Mode {
        keyfile,
        level,
        waypoint,
        obstacle,
        entity,
        warp,
        door,
        key
    };

    struct Context {
        bool success;
        int tilesize;
        std::map<char, std::pair<int, int> > kmap;
        CoordinateMap<char> map;
        CoordinateMap<char> warpDestination;
        WaypointManager wm;
        std::map<char, std::list<char> > ewmap;
        std::map<char, char> dtok;
        std::map<char, char> ktod;
    };

    Context parse(std::string filename, Mode mode);
private:

};

#endif
