
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
minX(0),
minY(0),
rows(),
columns()
{}

template <class T>
bool CoordinateMap<T>::add(T obj, int x, int y)
{
    if(get(x,y) != NULL)
        return false;

    if(rows.size() == 0)
    {
        maxX = x;
        minX = x;
        maxY = y;
        minY = y;
    }
    else
    {
        if(x > maxX)
            maxX = x;
        if(y > maxY)
            maxY = y;

        if(x < minX)
            minX = x;
        if(y < minY)
            minY = y;
    }

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
        incrementCheckMinVals();
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
    {
        decrementCheckMaxVals();
        incrementCheckMinVals();
    }

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
std::pair<int,int> CoordinateMap<T>::get(T obj)
{
    for(auto riter = rows.begin(); riter != rows.end(); ++riter)
    {
        if(riter->second.obj == obj)
        {
            return std::pair<int,int>(riter->second.x,riter->second.y);
        }
    }
    return std::pair<int,int>(-1,-1);
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
std::pair<int,int> CoordinateMap<T>::getMinSize()
{
    return std::pair<int,int>(minX,minY);
}

template <class T>
unsigned int CoordinateMap<T>::getSize()
{
    return rows.size();
}

template <class T>
void CoordinateMap<T>::decrementCheckMaxVals()
{
    auto citer = columns.rbegin();
    if(citer != columns.rend())
        maxX = citer->first;
    else
        maxX = 0;
    auto riter = rows.rbegin();
    if(riter != rows.rend())
        maxY = riter->first;
    else
        maxY = 0;
}

template <class T>
void CoordinateMap<T>::incrementCheckMinVals()
{
    auto citer = columns.begin();
    if(citer != columns.end())
        minX = citer->first;
    else
        minX = 0;
    auto riter = rows.begin();
    if(riter != rows.end())
        minY = riter->first;
    else
        minY = 0;
}
