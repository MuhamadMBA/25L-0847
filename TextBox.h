#pragma once

#include <SFML/Graphics.hpp>

class TextBox
{
private:

    sf::RectangleShape box;
    sf::Text           text;
    char               data[100];
    int                length;
    bool               selected;
    bool               passwordMode; // show * instead of chars

public:

    TextBox();

    void setup(sf::Font& font,
        float x, float y,
        float width, float height,
        bool  isPassword = false);

    void setSelected(bool state);

    bool contains(sf::Vector2f pos) const;

    void handleInput(const sf::Event& event);

    void draw(sf::RenderWindow& window);

    const char* getText() const;

    void clear();
};
