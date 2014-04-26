
#ifndef COORDINATE_MAP_HPP
#define COORDINATE_MAP_HPP

#include <list>
#include <map>

template <class T>
class RowEntry
{
public:
    RowEntry(T obj, int x, int y);

    T obj;
    int x;
    int y;
};

template <class T>
class ColumnEntry
{
public:
    ColumnEntry(T obj, int x, int y);

    T obj;
    int x;
    int y;
};

template <class T>
class CoordinateMap
{
public:
    CoordinateMap();

    bool add(T obj, int x, int y);
    bool remove(int x, int y);
    bool remove(T obj);
    T* get(int x, int y);
    std::pair<int,int> get(T obj);
    std::list<ColumnEntry<T> > getColumn(int x);
    std::list<RowEntry<T> > getRow(int y);
    std::pair<int,int> getMaxSize();
    std::pair<int,int> getMinSize();
    unsigned int getSize();
private:
    int maxX;
    int maxY;
    int minX;
    int minY;
    std::multimap<int, RowEntry<T> > rows;
    std::multimap<int, ColumnEntry<T> > columns;

    void decrementCheckMaxVals();
    void incrementCheckMinVals();
};

#include "coordinateMap.inl"

#endif
