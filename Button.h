#pragma once

#include <SFML/Graphics.hpp>

class Button
{
private:

    sf::RectangleShape shape;
    sf::Text text;
    sf::Color normalColor;
    sf::Color hoverColor;
    bool visible;

public:

    Button();

    void setup(sf::Font& font,
        const char* buttonText,
        float x, float y,
        float width, float height,
        sf::Color normal = sf::Color(40, 120, 220),
        sf::Color hover = sf::Color(60, 160, 255));

    bool isHovered(sf::Vector2f mousePos) const;

    // Returns true only on left-click release inside button
    bool isClicked(const sf::Event& event, sf::Vector2f mousePos) const;

    void updateHover(sf::Vector2f mousePos);

    void draw(sf::RenderWindow& window);

    void setColor(sf::Color color);

    void setVisible(bool v);

    void setText(const char* t);
};
