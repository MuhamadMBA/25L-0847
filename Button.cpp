#include "Button.h"

Button::Button()
{
    visible = true;
    normalColor = sf::Color(40, 120, 220);
    hoverColor = sf::Color(60, 160, 255);
}

void Button::setup(sf::Font& font,
    const char* buttonText,
    float x, float y,
    float width, float height,
    sf::Color normal,
    sf::Color hover)
{
    normalColor = normal;
    hoverColor = hover;

    shape.setPosition(x, y);
    shape.setSize(sf::Vector2f(width, height));
    shape.setFillColor(normalColor);
    shape.setOutlineThickness(2);
    shape.setOutlineColor(sf::Color(20, 80, 180));

    text.setFont(font);
    text.setString(buttonText);
    text.setCharacterSize(20);
    text.setFillColor(sf::Color::White);

    sf::FloatRect bounds = text.getLocalBounds();
    text.setPosition(
        x + (width - bounds.width) / 2.f - bounds.left,
        y + (height - bounds.height) / 2.f - bounds.top - 4.f
    );
}

bool Button::isHovered(sf::Vector2f mousePos) const
{
    return visible && shape.getGlobalBounds().contains(mousePos);
}

bool Button::isClicked(const sf::Event& event, sf::Vector2f mousePos) const
{
    return visible
        && event.type == sf::Event::MouseButtonReleased
        && event.mouseButton.button == sf::Mouse::Left
        && shape.getGlobalBounds().contains(mousePos);
}

void Button::updateHover(sf::Vector2f mousePos)
{
    if (!visible) return;
    shape.setFillColor(isHovered(mousePos) ? hoverColor : normalColor);
}

void Button::draw(sf::RenderWindow& window)
{
    if (!visible) return;
    window.draw(shape);
    window.draw(text);
}

void Button::setColor(sf::Color color)
{
    normalColor = color;
    shape.setFillColor(color);
}

void Button::setVisible(bool v)
{
    visible = v;
}

void Button::setText(const char* t)
{
    text.setString(t);
    sf::FloatRect b = text.getLocalBounds();
    sf::Vector2f  p = shape.getPosition();
    sf::Vector2f  s = shape.getSize();
    text.setPosition(
        p.x + (s.x - b.width) / 2.f - b.left,
        p.y + (s.y - b.height) / 2.f - b.top - 4.f
    );
}
