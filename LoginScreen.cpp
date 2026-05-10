#include "LoginScreen.h"
#include <cstdio>

// helper: copy char array (no std::string / strcpy)
static void myCopy(char* dst, const char* src, int maxLen)
{
    int i = 0;
    while (src[i] && i < maxLen - 1) { dst[i] = src[i]; i++; }
    dst[i] = '\0';
}

LoginScreen::LoginScreen(HospitalSystem& sys)
    : system(sys), selectedRole(0), failCount(0),
    locked(false), switchFlag(false), nextScreen(ScreenType::LOGIN),
    loggedIndex(-1)
{
    font.loadFromFile("Assets/Arial.ttf");

    // Background panel
    panel.setSize(sf::Vector2f(500.f, 480.f));
    panel.setPosition(150.f, 100.f);
    panel.setFillColor(sf::Color(30, 30, 50));
    panel.setOutlineThickness(2);
    panel.setOutlineColor(sf::Color(60, 100, 200));

    // Title
    title.setFont(font);
    title.setString("MediCore Hospital");
    title.setCharacterSize(36);
    title.setFillColor(sf::Color(100, 200, 255));
    title.setPosition(215.f, 115.f);

    subtitle.setFont(font);
    subtitle.setString("Management System");
    subtitle.setCharacterSize(20);
    subtitle.setFillColor(sf::Color(150, 180, 220));
    subtitle.setPosition(265.f, 158.f);

    // Labels
    roleLabel.setFont(font);
    roleLabel.setString("Select Role:");
    roleLabel.setCharacterSize(18);
    roleLabel.setFillColor(sf::Color(180, 200, 240));
    roleLabel.setPosition(170.f, 200.f);

    idLabel.setFont(font);
    idLabel.setString("ID:");
    idLabel.setCharacterSize(18);
    idLabel.setFillColor(sf::Color(180, 200, 240));
    idLabel.setPosition(170.f, 330.f);

    passwordLabel.setFont(font);
    passwordLabel.setString("Password:");
    passwordLabel.setCharacterSize(18);
    passwordLabel.setFillColor(sf::Color(180, 200, 240));
    passwordLabel.setPosition(170.f, 395.f);

    msgText.setFont(font);
    msgText.setCharacterSize(17);
    msgText.setPosition(170.f, 510.f);

    // Role buttons
    patientBtn.setup(font, "Patient", 170.f, 220.f, 145.f, 44.f,
        sf::Color(50, 90, 180), sf::Color(70, 130, 240));
    doctorBtn.setup(font, "Doctor", 325.f, 220.f, 145.f, 44.f,
        sf::Color(50, 90, 180), sf::Color(70, 130, 240));
    adminBtn.setup(font, "Admin", 480.f, 220.f, 145.f, 44.f,
        sf::Color(50, 90, 180), sf::Color(70, 130, 240));

    // Input boxes
    idBox.setup(font, 280.f, 320.f, 340.f, 44.f, false);
    passwordBox.setup(font, 280.f, 385.f, 340.f, 44.f, true);
    idBox.setSelected(true);

    // Login button
    loginBtn.setup(font, "Login", 290.f, 450.f, 220.f, 48.f,
        sf::Color(20, 160, 80), sf::Color(30, 200, 100));
}

void LoginScreen::setMessage(const char* msg, sf::Color color)
{
    msgText.setString(msg);
    msgText.setFillColor(color);
}

void LoginScreen::tryLogin()
{
    if (locked)
    {
        setMessage("Account locked. Contact admin.");
        return;
    }

    if (selectedRole == 0)
    {
        setMessage("Please select a role first.");
        return;
    }

    const char* idStr = idBox.getText();
    const char* pass = passwordBox.getText();

    // Parse ID manually (no std::stoi, no atoi from string — use plain loop)
    int id = 0;
    bool bad = false;
    for (int i = 0; idStr[i] != '\0'; i++)
    {
        if (idStr[i] < '0' || idStr[i] > '9') { bad = true; break; }
        id = id * 10 + (idStr[i] - '0');
    }

    if (bad || id <= 0)
    {
        setMessage("Invalid ID. Enter a positive number.");
        return;
    }

    int idx = -1;

    if (selectedRole == 1)
        idx = system.loginPatient(id, pass, failCount);
    else if (selectedRole == 2)
        idx = system.loginDoctor(id, pass, failCount);
    else
        idx = system.loginAdmin(id, pass, failCount);

    if (failCount >= 3)
    {
        locked = true;
        setMessage("Account locked. Contact admin.");
        return;
    }

    if (idx == -1)
    {
        char buf[80];
        // build message without sprintf (actually sprintf is fine — not a banned function)
        snprintf(buf, 80, "Invalid ID or password. Attempt %d/3.", failCount);
        setMessage(buf);
        return;
    }

    // Success
    loggedIndex = idx;
    idBox.clear();
    passwordBox.clear();
    setMessage("Login successful!", sf::Color(50, 220, 100));

    if (selectedRole == 1) nextScreen = ScreenType::PATIENT;
    else if (selectedRole == 2) nextScreen = ScreenType::DOCTOR;
    else nextScreen = ScreenType::ADMIN;

    switchFlag = true;
}

void LoginScreen::handleEvent(sf::Event& event, sf::RenderWindow& window)
{
    sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    // Click-to-focus textboxes
    if (event.type == sf::Event::MouseButtonPressed
        && event.mouseButton.button == sf::Mouse::Left)
    {
        idBox.setSelected(idBox.contains(mouse));
        passwordBox.setSelected(passwordBox.contains(mouse));
    }

    // Role selection
    if (patientBtn.isClicked(event, mouse))
    {
        selectedRole = 1;
        patientBtn.setColor(sf::Color(20, 140, 80));
        doctorBtn.setColor(sf::Color(50, 90, 180));
        adminBtn.setColor(sf::Color(50, 90, 180));
        setMessage("Role: Patient", sf::Color(100, 220, 150));
    }
    if (doctorBtn.isClicked(event, mouse))
    {
        selectedRole = 2;
        doctorBtn.setColor(sf::Color(20, 140, 80));
        patientBtn.setColor(sf::Color(50, 90, 180));
        adminBtn.setColor(sf::Color(50, 90, 180));
        setMessage("Role: Doctor", sf::Color(100, 220, 150));
    }
    if (adminBtn.isClicked(event, mouse))
    {
        selectedRole = 3;
        adminBtn.setColor(sf::Color(20, 140, 80));
        patientBtn.setColor(sf::Color(50, 90, 180));
        doctorBtn.setColor(sf::Color(50, 90, 180));
        setMessage("Role: Admin", sf::Color(100, 220, 150));
    }

    if (loginBtn.isClicked(event, mouse))
        tryLogin();

    // Enter key triggers login
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Return)
        tryLogin();

    // Tab switches focus between textboxes
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Tab)
    {
        const char* selID = idBox.getText();
        bool idSelected = false;
        // crude check: if idBox is selected, switch to password
        // We track via contains — just toggle
        idBox.setSelected(!idBox.contains(mouse));
        passwordBox.setSelected(idBox.contains(mouse));
    }

    idBox.handleInput(event);
    passwordBox.handleInput(event);
}

void LoginScreen::update() {}

void LoginScreen::render(sf::RenderWindow& window)
{
    window.draw(panel);
    window.draw(title);
    window.draw(subtitle);
    window.draw(roleLabel);
    window.draw(idLabel);
    window.draw(passwordLabel);

    patientBtn.draw(window);
    doctorBtn.draw(window);
    adminBtn.draw(window);
    loginBtn.draw(window);

    idBox.draw(window);
    passwordBox.draw(window);

    window.draw(msgText);
}

ScreenType LoginScreen::getNextScreen() const { return nextScreen; }
bool       LoginScreen::shouldSwitch()  const { return switchFlag; }
void       LoginScreen::resetSwitch() { switchFlag = false; failCount = 0; locked = false; }
int        LoginScreen::getLoggedIndex() const { return loggedIndex; }
int        LoginScreen::getSelectedRole() const { return selectedRole; }
