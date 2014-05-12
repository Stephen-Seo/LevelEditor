
template <class T>
RowEntry<T>::RowEntry(T obj, int x, int y) :
obj(obj),
x(x),
y(y)
{}

template <class T>
bool operator< (const RowEntry<T>& lhs, const RowEntry<T>& rhs)
{
    return lhs.x < rhs.x;
}

template <class T>
ColumnEntry<T>::ColumnEntry(T obj, int x, int y) :
obj(obj),
x(x),
y(y)
{}

template <class T>
bool operator< (const ColumnEntry<T>& lhs, const ColumnEntry<T>& rhs)
{
    return lhs.y < rhs.y;
}

template <class T>
CoordinateMap<T>::CoordinateMap() :
maxX(0),
maxY(0),
rows(),
columns()
{}

template <class T>
bool CoordinateMap<T>::add(T obj, int x, int y)
{
    if(get(x,y) != NULL)
        return false;

    if(x+1 > maxX)
        maxX = x+1;
    if(y+1 > maxY)
        maxY = y+1;

    RowEntry<T> re(obj, x, y);
    ColumnEntry<T> ce(obj, x, y);

    rows.insert(std::pair<int,RowEntry<T> > (y, re));
    columns.insert(std::pair<int,ColumnEntry<T> > (x, ce));

    return true;
}

template <class T>
bool CoordinateMap<T>::remove(int x, int y)
{
    bool deletedFromRows = false;
    bool deletedFromColumns = false;

    for(auto riter = rows.find(y); riter != rows.end() && riter->first == y; ++riter)
    {
        if(riter->second.x == x)
        {
            rows.erase(riter);
            deletedFromRows = true;
            break;
        }
    }

    for(auto citer = columns.find(x); citer != columns.end() && citer->first == x; ++citer)
    {
        if(citer->second.y == y)
        {
            columns.erase(citer);
            deletedFromColumns = true;
            break;
        }
    }

    if(deletedFromRows || deletedFromColumns)
    {
        decrementCheckMaxVals();
    }
    return deletedFromRows && deletedFromColumns;
}

template <class T>
bool CoordinateMap<T>::remove(T obj)
{
    bool deletedFromRows = false;
    bool deletedFromColumns = false;

    for(auto riter = rows.begin(); riter != rows.end(); ++riter)
    {
        if(riter->second.obj == obj)
        {
            rows.erase(riter);
            deletedFromRows = true;
            break;
        }
    }

    for(auto citer = columns.begin(); citer != columns.end(); ++citer)
    {
        if(citer->second.obj == obj)
        {
            columns.erase(citer);
            deletedFromColumns = true;
            break;
        }
    }

    if(deletedFromRows || deletedFromColumns)
        decrementCheckMaxVals();

    return deletedFromRows && deletedFromColumns;
}

template <class T>
T* CoordinateMap<T>::get(int x, int y)
{
    for(auto riter = rows.find(y); riter != rows.end() && riter->first == y; ++riter)
    {
        if(riter->second.x == x)
        {
            return &(riter->second.obj);
        }
    }
    return NULL;
}

template <class T>
Coords CoordinateMap<T>::get(T obj)
{
    for(auto riter = rows.begin(); riter != rows.end(); ++riter)
    {
        if(riter->second.obj == obj)
        {
            return Coords(riter->second.x, riter->second.y);
        }
    }
    return Coords();
}

template <class T>
std::list<ColumnEntry<T> > CoordinateMap<T>::getColumn(int x)
{
    std::list<ColumnEntry<T> > list;

    for(auto citer = columns.find(x); citer != columns.end() && citer->first == x; ++citer)
    {
        list.push_back(citer->second);
    }

    return list;
}

template <class T>
std::list<RowEntry<T> > CoordinateMap<T>::getRow(int y)
{
    std::list<RowEntry<T> > list;

    for(auto riter = rows.find(y); riter != rows.end() && riter->first == y; ++riter)
    {
        list.push_back(riter->second);
    }

    return list;
}

template <class T>
std::pair<int,int> CoordinateMap<T>::getMaxSize()
{
    return std::pair<int,int>(maxX,maxY);
}

template <class T>
void CoordinateMap<T>::decrementCheckMaxVals()
{
    int x;
    for(x = maxX-1; x >= 0 && getColumn(x).size() == 0; --x)
    {}
    maxX = x+1;
    int y;
    for(y = maxY-1; y >= 0 && getRow(y).size() == 0; --y)
    {}
    maxY = y+1;
}
