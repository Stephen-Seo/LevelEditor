
#ifndef LE_COMMANDS_HPP
#define LE_COMMANDS_HPP

#include <list>
#include <set>
#include <map>

#include "command.hpp"
#include "coordinateMap.hpp"
#include "waypoint.hpp"

class TileCreateCommand : public Command
{
public:
    TileCreateCommand(char c, int x, int y, CoordinateMap<char>* map);
    void execute();
    void undo();
private:
    char c;
    int x;
    int y;
    CoordinateMap<char>* map;
    char prev;
    bool prevExists;
};

class TileDeleteCommand : public Command
{
public:
    TileDeleteCommand(int x, int y, CoordinateMap<char>* map);
    void execute();
    void undo();
private:
    char c;
    int x;
    int y;
    CoordinateMap<char>* map;
};

class WaypointCreateCommand : public Command
{
public:
    WaypointCreateCommand(char c, int x, int y, WaypointManager* wm, CoordinateMap<char>* map);
    void execute();
    void undo();
private:
    char c;
    int x;
    int y;
    WaypointManager* wm;
    CoordinateMap<char>* map;
};

class WaypointDeleteCommand : public Command
{
public:
    WaypointDeleteCommand(char c, WaypointManager* wm, CoordinateMap<char>* map, std::map<char, std::list<char> >* ewmap);
    WaypointDeleteCommand(int x, int y, WaypointManager* wm, CoordinateMap<char>* map, std::map<char, std::list<char> >* ewmap);
    void execute();
    void undo();
private:
    char c;
    int x;
    int y;
    WaypointManager* wm;
    CoordinateMap<char>* map;
    std::map<char, std::list<char> >* ewmap;
    std::set<char> adjacent;
    std::list<char> entity;
    std::list<char> nextChar;
};

class WaypointLinkCommand : public Command
{
public:
    WaypointLinkCommand(char waypointOne, char waypointTwo, WaypointManager* wm, bool unlink);
    void execute();
    void undo();
private:
    char waypointOne;
    char waypointTwo;
    WaypointManager* wm;
    bool unlink;
};

class EntityCreateCommand : public Command
{
public:
    EntityCreateCommand(char c, int x, int y, CoordinateMap<char>* map, std::map<char, std::list<char> >* ewmap);
    void execute();
    void undo();
private:
    char c;
    int x;
    int y;
    CoordinateMap<char>* map;
    std::map<char, std::list<char> >* ewmap;
    char prev;
    std::list<char> prevList;
    bool prevExists;
};

class EntityDeleteCommand : public Command
{
public:
    EntityDeleteCommand(int x, int y, CoordinateMap<char>* map, std::map<char, std::list<char> >* ewmap);
    void execute();
    void undo();
private:
    char c;
    int x;
    int y;
    CoordinateMap<char>* map;
    std::map<char, std::list<char> >* ewmap;
    std::list<char> listContents;
};

class EWLinkCommand : public Command
{
public:
    EWLinkCommand(char entity, char waypoint, std::map<char, std::list<char> >* ewmap);
    void execute();
    void undo();
private:
    char entity;
    char waypoint;
    std::map<char, std::list<char> >* ewmap;
    bool isLinking;
    char pos;
};

class WarpCreateCommand : public Command
{
public:
    WarpCreateCommand(char c, int x, int y, CoordinateMap<char>* map, CoordinateMap<char>* dest);
    void execute();
    void undo();
private:
    char c;
    int x;
    int y;
    CoordinateMap<char>* map;
    CoordinateMap<char>* dest;
    Coords coords;
    bool prevExists;
    char prev;
};

class WarpDeleteCommand : public Command
{
public:
    WarpDeleteCommand(int x, int y, CoordinateMap<char>* map, CoordinateMap<char>* dest);
    void execute();
    void undo();
private:
    char c;
    int x;
    int y;
    CoordinateMap<char>* map;
    CoordinateMap<char>* dest;
    Coords coords;
};

class WarpDestCommand : public Command
{
public:
    WarpDestCommand(char c, int x, int y, CoordinateMap<char>* dest);
    void execute();
    void undo();
private:
    char c;
    int x;
    int y;
    CoordinateMap<char>* dest;
    Coords prevCoords;
};

class KDCreateCommand : public Command
{
public:
    KDCreateCommand(char c, int x, int y, CoordinateMap<char>* map, std::map<char,char>* dtok, std::map<char,char>* ktod, bool isKey);
    void execute();
    void undo();
private:
    char c;
    int x;
    int y;
    CoordinateMap<char>* map;
    std::map<char,char>* dtok;
    std::map<char,char>* ktod;
    bool isKey;
    bool hasLink;
    bool prevExists;
    char prev;
    char other;
};

class KDDeleteCommand : public Command
{
public:
    KDDeleteCommand(int x, int y, CoordinateMap<char>* map, std::map<char,char>* dtok, std::map<char,char>* ktod, bool isKey);
    void execute();
    void undo();
private:
    char c;
    int x;
    int y;
    CoordinateMap<char>* map;
    std::map<char,char>* dtok;
    std::map<char,char>* ktod;
    bool isKey;
    bool hasLink;
    char other;
};

class KDLinkCommand : public Command
{
public:
    KDLinkCommand(char key, char door, std::map<char,char>* dtok, std::map<char,char>* ktod);
    void execute();
    void undo();
private:
    char key;
    char door;
    std::map<char,char>* dtok;
    std::map<char,char>* ktod;
    bool klinkExists;
    char klink;
    bool dlinkExists;
    char dlink;
};

#endif
