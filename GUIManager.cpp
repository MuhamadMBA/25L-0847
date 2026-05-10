#include "GUIManager.h"

GUIManager::GUIManager()
    : window(sf::VideoMode(800, 560), "MediCore Hospital Management System",
        sf::Style::Titlebar | sf::Style::Close),
    currentScreen(ScreenType::LOGIN)
{
    window.setFramerateLimit(60);

    bg.setSize(sf::Vector2f(800.f, 560.f));
    bg.setFillColor(sf::Color(15, 15, 28));

    // Load backend data
    system.loadSystem();

    // Create screens (heap so we can re-init)
    loginScreen = new LoginScreen(system);
    patientScreen = new PatientScreen(system);
    doctorScreen = new DoctorScreen(system);
    adminScreen = new AdminScreen(system);
}

GUIManager::~GUIManager()
{
    delete loginScreen;
    delete patientScreen;
    delete doctorScreen;
    delete adminScreen;
}

void GUIManager::switchTo(ScreenType next, int role, int index)
{
    currentScreen = next;

    if (next == ScreenType::PATIENT)
        patientScreen->init(index);
    else if (next == ScreenType::DOCTOR)
        doctorScreen->init(index);
    else if (next == ScreenType::ADMIN)
        adminScreen->init(index);
    // LOGIN screen needs no re-init (it's stateless except fail count, reset handled)
}

void GUIManager::run()
{
    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                return;
            }

            // Dispatch event to current screen
            if (currentScreen == ScreenType::LOGIN)
                loginScreen->handleEvent(event, window);
            else if (currentScreen == ScreenType::PATIENT)
                patientScreen->handleEvent(event, window);
            else if (currentScreen == ScreenType::DOCTOR)
                doctorScreen->handleEvent(event, window);
            else if (currentScreen == ScreenType::ADMIN)
                adminScreen->handleEvent(event, window);
        }

        // Check for screen transitions
        if (currentScreen == ScreenType::LOGIN && loginScreen->shouldSwitch())
        {
            ScreenType next = loginScreen->getNextScreen();
            int role = loginScreen->getSelectedRole();
            int index = loginScreen->getLoggedIndex();
            loginScreen->resetSwitch();
            switchTo(next, role, index);
        }
        else if (currentScreen == ScreenType::PATIENT && patientScreen->shouldSwitch())
        {
            patientScreen->resetSwitch();
            currentScreen = ScreenType::LOGIN;
        }
        else if (currentScreen == ScreenType::DOCTOR && doctorScreen->shouldSwitch())
        {
            doctorScreen->resetSwitch();
            currentScreen = ScreenType::LOGIN;
        }
        else if (currentScreen == ScreenType::ADMIN && adminScreen->shouldSwitch())
        {
            adminScreen->resetSwitch();
            currentScreen = ScreenType::LOGIN;
        }

        // Update
        if (currentScreen == ScreenType::LOGIN)
            loginScreen->update();
        else if (currentScreen == ScreenType::PATIENT)
            patientScreen->update();
        else if (currentScreen == ScreenType::DOCTOR)
            doctorScreen->update();
        else if (currentScreen == ScreenType::ADMIN)
            adminScreen->update();

        // Render
        window.clear();
        window.draw(bg);

        if (currentScreen == ScreenType::LOGIN)
            loginScreen->render(window);
        else if (currentScreen == ScreenType::PATIENT)
            patientScreen->render(window);
        else if (currentScreen == ScreenType::DOCTOR)
            doctorScreen->render(window);
        else if (currentScreen == ScreenType::ADMIN)
            adminScreen->render(window);

        window.display();
    }
}
