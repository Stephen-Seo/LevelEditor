
#include "state.hpp"
#include "stateStack.hpp"

State::Context::Context(sf::RenderWindow& window, sf::RenderWindow& twindow, TextureHolder& textures, FontHolder& fonts, std::string oFile)
: window(&window), twindow(&twindow), textures(&textures), fonts(&fonts), oFile(oFile)
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

