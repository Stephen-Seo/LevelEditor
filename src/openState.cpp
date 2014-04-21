
#include "openState.hpp"

OpenState::OpenState(StateStack& stack, Context context)
: State(stack,context),
selection(Selection::None),
info("information", getContext().fonts->get(Fonts::ClearSans), 20),
line(sf::Lines, 4),
lshift(false),
rshift(false)
{

}

void OpenState::draw()
{
    getContext().window->clear(sf::Color(40,40,40));

    info.setPosition(0.0f, 20.0f);
    info.setString("Prefix:");
    info.setStyle(sf::Text::Bold);
    getContext().window->draw(info);

    if(selection == Selection::Prefix)
    {
        line[0].position = sf::Vector2f(0.0f, 44.0f);
        line[1].position = sf::Vector2f(800.0f, 44.0f);
        line[2].position = sf::Vector2f(0.0f, 64.0f);
        line[3].position = sf::Vector2f(800.0f, 64.0f);
        getContext().window->draw(line);
    }

    info.setPosition(10.0f, 40.0f);
    if(oFile == "")
        info.setString("Click here and start typing to set a prefix");
    else
        info.setString(oFile);
    info.setStyle(sf::Text::Regular);
    getContext().window->draw(info);

    info.setPosition(0.0f, 80.0f);
    info.setString("Tilesheet File:");
    info.setStyle(sf::Text::Bold);
    getContext().window->draw(info);

    if(selection == Selection::Img)
    {
        line[0].position = sf::Vector2f(0.0f, 104.0f);
        line[1].position = sf::Vector2f(800.0f, 104.0f);
        line[2].position = sf::Vector2f(0.0f, 124.0f);
        line[3].position = sf::Vector2f(800.0f, 124.0f);
        getContext().window->draw(line);
    }

    info.setPosition(10.0f, 100.0f);
    if(imgFile == "")
        info.setString("Click here and start typing to set the tilesheet");
    else
        info.setString(imgFile);
    info.setStyle(sf::Text::Regular);
    getContext().window->draw(info);

    info.setPosition(0.0f, 160.0f);
    info.setString("When finished, press the enter key.");
    info.setStyle(sf::Text::Bold);
    getContext().window->draw(info);
}

bool OpenState::update()
{

    return true;
}

bool OpenState::handleEvent(const sf::Event& event)
{
    if(event.type == sf::Event::MouseButtonPressed)
    {
        if(event.mouseButton.y >= 44 && event.mouseButton.y <= 64)
            selection = Selection::Prefix;
        else if(event.mouseButton.y >= 104 && event.mouseButton.y <= 124)
            selection = Selection::Img;
        else
            selection = Selection::None;
    }
    else if(event.type == sf::Event::TextEntered)
    {
        if(event.text.unicode >= 32 && event.text.unicode <= 126)
            write(sf::String(static_cast<char>(event.text.unicode)).toAnsiString());
    }
    else if(event.type == sf::Event::KeyPressed)
    {
        switch(event.key.code)
        {
        case sf::Keyboard::Return:
            startEditor();
            break;
        case sf::Keyboard::BackSpace:
            backspace();
            break;
        case sf::Keyboard::LShift:
            lshift = true;
            break;
        case sf::Keyboard::RShift:
            rshift = true;
            break;
        default:
            break;
        }
    }
    else if(event.type == sf::Event::KeyReleased)
    {
        switch(event.key.code)
        {
        case sf::Keyboard::LShift:
            lshift = false;
            break;
        case sf::Keyboard::RShift:
            rshift = false;
            break;
        default:
            break;
        }
    }

    return true;
}

void OpenState::write(std::string c)
{
    switch(selection)
    {
    case Selection::Prefix:
        oFile.append(c);
        break;
    case Selection::Img:
        imgFile.append(c);
        break;
    default:
        break;
    }
}

void OpenState::backspace()
{
    switch(selection)
    {
    case Selection::Prefix:
        if(oFile.size() > 0)
            oFile.pop_back();
        break;
    case Selection::Img:
        if(imgFile.size() > 0)
            imgFile.pop_back();
        break;
    default:
        break;
    }
}

void OpenState::startEditor()
{
    std::cout << "Using sheet \"" << imgFile << "\"\nUsing output \"" << oFile << "\"\n";
    getContext().oFile->insert(0,oFile);
    getContext().textures->load(Textures::TileSheet, imgFile);
    requestStackPop();
    requestStackPush(States::EditState);
}
