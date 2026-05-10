#pragma once
#include <SFML/Graphics.hpp>
#include "Screen.h"
#include "Button.h"
#include "TextBox.h"
#include "HospitalSystem.h"

class LoginScreen : public Screen
{
private:
    HospitalSystem& system;

    sf::Font font;
    sf::RectangleShape panel;
    sf::Text title, subtitle, roleLabel;
    sf::Text idLabel, passwordLabel, msgText;

    TextBox idBox, passwordBox;
    Button patientBtn, doctorBtn, adminBtn, loginBtn;

    int  selectedRole;   // 1=Patient, 2=Doctor, 3=Admin
    int  failCount;
    bool locked;
    bool switchFlag;
    ScreenType nextScreen;
    int  loggedIndex;

    void setMessage(const char* msg,
        sf::Color color = sf::Color(220, 80, 80));
    void tryLogin();

public:
    explicit LoginScreen(HospitalSystem& sys);

    // --- Screen interface (all 6 pure virtuals) ---
    void handleEvent(sf::Event& event, sf::RenderWindow& window) override;
    void update() override;
    void render(sf::RenderWindow& window) override;
    ScreenType getNextScreen() const override;
    bool shouldSwitch() const override;
    void resetSwitch() override;

    // --- Extra accessors used by GUIManager ---
    int getSelectedRole() const;
    int getLoggedIndex()  const;
};