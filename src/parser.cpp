
#include "parser.hpp"

Parser::Context Parser::parse(std::string filename, Parser::Mode mode)
{
    Parser::Context c;
    std::fstream fs;
    std::string line;

    c.success = false;
    fs.open(filename);

    if(!fs.is_open())
        return c;

    switch(mode)
    {
    case Parser::keyfile:
        fs >> c.tilesize;
        std::getline(fs, line); // advance to next line
        for(int y=0; std::getline(fs, line); ++y)
            for(int x=0; x < line.size(); ++x)
            {
                if(line[x] != ' ')
                    c.kmap.insert(std::pair<char, std::pair<int, int> >(line[x], std::pair<int, int>(x,y)));
            }
        break;
    case Parser::level:
    case Parser::door:
        for(int y=0; std::getline(fs, line); ++y)
            for(int x=0; x < line.size(); ++x)
            {
                if(line[x] != ' ')
                    c.map.add(line[x], x, y);
            }
        break;
    case Parser::waypoint:
        // populate waypointManager
        while(std::getline(fs, line))
        {
            if(line[0] == '#')
                break;
            c.wm.addWaypoint(line[0]);
        }
        fs.close();
        fs.clear();
        fs.open(filename);
        // connect adjacent
        while(std::getline(fs, line))
        {
            if(line[0] == '#')
                break;
            char current = line[0];
            for(int x=1; x < line.size(); ++x)
            {
                if(line[x] == ' ')
                    continue;
                c.wm.makeAdjacent(current, line[x]);
            }
        }
        // locations
        for(int y=0; std::getline(fs, line); ++y)
            for(int x=0; x < line.size(); ++x)
            {
                if(line[x] != ' ')
                    c.map.add(line[x], x, y);
            }
        break;
    case Parser::obstacle:
        for(int y=0; std::getline(fs, line); ++y)
            for(int x=0; x < line.size(); ++x)
            {
                if(line[x] != ' ')
                    c.map.add('o', x, y);
            }
        break;
    case Parser::entity:
        // get entity waypoint connections
        while(std::getline(fs,line))
        {
            if(line[0] == '#')
                break;
            c.ewmap.insert(std::pair<char, std::list<char> >(line[0], std::list<char>()));
            for(int x=1; x < line.size(); ++x)
            {
                if(line[x] == ' ')
                    continue;
                c.ewmap[line[0]].push_back(line[x]);
            }
        }

        for(int y=0; std::getline(fs, line); ++y)
            for(int x=0; x < line.size(); ++x)
            {
                if(line[x] != ' ')
                    c.map.add(line[x], x, y);
            }
        break;
    case Parser::warp:
        while(std::getline(fs, line))
        {
            if(line[0] == '#')
                break;
            char warp = line[0];
            unsigned long space0 = line.find_first_of(" ");
            unsigned long space1 = line.find_first_of(" ", space0+1);
            if(space0 == std::string::npos || space1 == std::string::npos)
            {
                fs.close();
                c.success = false;
                return c;
            }

            std::string substring = line.substr(space0+1, space1-space0-1);
            int coord0;
            std::istringstream(substring) >> coord0;

            substring = line.substr(space1+1, std::string::npos);
            int coord1;
            std::istringstream(substring) >> coord1;

            c.warpDestination.add(warp, coord0, coord1);
        }

        for(int y=0; std::getline(fs, line); ++y)
            for(int x=0; x < line.size(); ++x)
            {
                if(line[x] != ' ')
                    c.map.add(line[x], x, y);
            }
        break;
    case Parser::key:
        while(std::getline(fs, line))
        {
            if(line[0] == '#')
                break;
            for(int x=1; x < line.size(); ++x)
            {
                if(line[x] != ' ')
                {
                    c.ktod.insert(std::pair<char,char>(line[0],line[x]));
                    c.dtok.insert(std::pair<char,char>(line[x],line[0]));
                }
            }
        }
        for(int y=0; std::getline(fs, line); ++y)
            for(int x=0; x < line.size(); ++x)
            {
                if(line[x] != ' ')
                    c.map.add(line[x], x, y);
            }
        break;
    default:
        fs.close();
        c.success = false;
        return c;
    }

    fs.close();

    c.success = true;
    return c;
}
