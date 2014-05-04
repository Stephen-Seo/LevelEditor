
#include "command.hpp"

CommandStack::CommandStack() :
executed(),
undone(),
undoLimit(COMMAND_UNDO_LIMIT)
{}

CommandStack::~CommandStack()
{
    clearExecuted();
    clearUndone();
}

void CommandStack::execute(Command* c)
{
    c->execute();
    executed.push_front(c);
    clearUndone();
    if(executed.size() > undoLimit)
        executed.pop_back();
}

bool CommandStack::undo()
{
    if(executed.size() == 0)
        return false;
    Command* c = executed.front();
    executed.pop_front();
    c->undo();
    undone.push_front(c);
    return true;
}

bool CommandStack::redo()
{
    if(undone.size() == 0)
        return false;
    Command* c = undone.front();
    undone.pop_front();
    c->execute();
    executed.push_front(c);
    return true;
}

void CommandStack::clearExecuted()
{
    for(auto iter = executed.begin(); iter != executed.end(); ++iter)
        delete *iter;

    executed.clear();
}

void CommandStack::clearUndone()
{
    for(auto iter = undone.begin(); iter != undone.end(); ++iter)
        delete *iter;

    undone.clear();
}
