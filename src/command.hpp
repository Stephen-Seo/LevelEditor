#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <list>

class Command
{
public:
    virtual ~Command() {}
    virtual void execute() = 0;
    virtual void undo() = 0;
};

class CommandStack
{
public:
    CommandStack();
    ~CommandStack();
    void execute(Command* c);
    bool undo();
    bool redo();
private:
    std::list<Command*> executed;
    std::list<Command*> undone;

    void clearExecuted();
    void clearUndone();
};

#endif
