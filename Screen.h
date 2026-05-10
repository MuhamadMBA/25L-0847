#pragma once

#include <SFML/Graphics.hpp>

enum class ScreenType
{
    LOGIN,
    PATIENT,
    DOCTOR,
    ADMIN,
    EXIT
};

class Screen
{
public:

    virtual void handleEvent(sf::Event& event, sf::RenderWindow& window) = 0;

    virtual void update() = 0;

    virtual void render(sf::RenderWindow& window) = 0;

    virtual ScreenType getNextScreen() const = 0;

    virtual bool shouldSwitch() const = 0;

    virtual void resetSwitch() = 0;

    virtual ~Screen() {}
};
