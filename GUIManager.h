#pragma once

#include <SFML/Graphics.hpp>
#include "LoginScreen.h"
#include "PatientScreen.h"
#include "DoctorScreen.h"
#include "AdminScreen.h"
#include "HospitalSystem.h"

class GUIManager
{
private:

    sf::RenderWindow window;

    HospitalSystem system;

    LoginScreen* loginScreen;
    PatientScreen* patientScreen;
    DoctorScreen* doctorScreen;
    AdminScreen* adminScreen;

    ScreenType currentScreen;

    // Background gradient rects
    sf::RectangleShape bg;

    void switchTo(ScreenType next, int role, int index);

public:

    GUIManager();
    ~GUIManager();

    void run();
};
