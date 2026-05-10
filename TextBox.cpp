#include "TextBox.h"

TextBox::TextBox()
{
    length = 0;
    data[0] = '\0';
    selected = false;
    passwordMode = false;
}

void TextBox::setup(sf::Font& font,
    float x, float y,
    float width, float height,
    bool  isPassword)
{
    passwordMode = isPassword;

    box.setPosition(x, y);
    box.setSize(sf::Vector2f(width, height));
    box.setFillColor(sf::Color(245, 245, 250));
    box.setOutlineThickness(2);
    box.setOutlineColor(sf::Color(100, 100, 160));

    text.setFont(font);
    text.setCharacterSize(20);
    text.setFillColor(sf::Color(20, 20, 40));
    text.setPosition(x + 10.f, y + 10.f);
}

void TextBox::setSelected(bool state)
{
    selected = state;
    box.setOutlineColor(state ? sf::Color(40, 120, 220) : sf::Color(100, 100, 160));
}

bool TextBox::contains(sf::Vector2f pos) const
{
    return box.getGlobalBounds().contains(pos);
}

void TextBox::handleInput(const sf::Event& event)
{
    if (!selected) return;

    if (event.type == sf::Event::TextEntered)
    {
        char ch = static_cast<char>(event.text.unicode);

        if (ch == 8) // backspace
        {
            if (length > 0)
            {
                length--;
                data[length] = '\0';
            }
        }
        else if (ch >= 32 && ch <= 126)
        {
            if (length < 99)
            {
                data[length] = ch;
                length++;
                data[length] = '\0';
            }
        }

        // Build display string
        if (passwordMode)
        {
            char masked[100];
            for (int i = 0; i < length; i++) masked[i] = '*';
            masked[length] = '\0';
            text.setString(masked);
        }
        else
        {
            text.setString(data);
        }
    }
}

void TextBox::draw(sf::RenderWindow& window)
{
    window.draw(box);
    window.draw(text);
}

const char* TextBox::getText() const
{
    return data;
}

void TextBox::clear()
{
    length = 0;
    data[0] = '\0';
    text.setString("");
}
