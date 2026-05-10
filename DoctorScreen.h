#pragma once

#include <SFML/Graphics.hpp>
#include "Screen.h"
#include "Button.h"
#include "TextBox.h"
#include "HospitalSystem.h"

enum class DoctorView
{
    MENU,
    TODAY_APPTS,
    MARK_COMPLETE_INPUT,
    MARK_NOSHOW_INPUT,
    WRITE_RX_ID,
    WRITE_RX_MED,
    WRITE_RX_NOTES,
    VIEW_HISTORY_ID
};

class DoctorScreen : public Screen
{
private:

    sf::Font font;
    HospitalSystem& system;
    int doctorIndex;
    bool switchFlag;

    sf::RectangleShape headerBar;
    sf::Text           headerText;
    sf::RectangleShape outputPanel;
    sf::Text           outputLines[30];
    int                outputCount;

    sf::Text  inputLabel;
    TextBox   inputBox;
    Button    actionBtn;
    Button    backBtn;
    Button    logoutBtn;
    Button    menuBtns[5];

    DoctorView currentView;
    char todayDate[20];
    int  rxAppointmentID;
    char rxMedicines[500];

    static const char* menuLabels[5];

    // helpers
    static void myCopy(char* dst, const char* src, int maxLen);
    static bool myEq(const char* a, const char* b);
    static int  myAtoi(const char* s, bool& ok);
    static void myItoa(int n, char* buf);

    void clearOutput();
    void addOutputLine(const char* line);
    void showMenu();
    void showMessage(const char* msg);
    void loadTodayDate();

    void doTodayAppts();
    void handleMarkComplete(const char* input);
    void handleMarkNoShow(const char* input);
    void handleRxID(const char* input);
    void handleRxMed(const char* input);
    void handleRxNotes(const char* input);
    void handleHistoryID(const char* input);

public:

    DoctorScreen(HospitalSystem& sys);
    void init(int dIndex);
    //helper
    static int  myLen(const char* s);

    void handleEvent(sf::Event& event, sf::RenderWindow& window) override;
    void update() override;
    void render(sf::RenderWindow& window) override;

    ScreenType getNextScreen() const override;
    bool       shouldSwitch()  const override;
    void       resetSwitch()         override;
};
