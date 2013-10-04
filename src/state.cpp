
#include "state.hpp"
#include "stateStack.hpp"

State::Context::Context(sf::RenderWindow& window, TextureHolder& textures, FontHolder& fonts, std::string oFile, std::string keyFile)
: window(&window), textures(&textures), fonts(&fonts), oFile(oFile), keyFile(keyFile)
{}

State::State(StateStack& stack, Context context)
: stack(&stack), context(context)
{}

State::~State()
{}

void State::requestStackPush(States::ID stateID)
{
    stack->pushState(stateID);
}

void State::requestStackPop()
{
    stack->popState();
}

void State::requestStateClear()
{
    stack->clearStates();
}

State::Context State::getContext() const
{
    return context;
}

