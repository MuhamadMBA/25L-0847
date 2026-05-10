#pragma once

#include <SFML/Graphics.hpp>
#include "Screen.h"
#include "Button.h"
#include "TextBox.h"
#include "HospitalSystem.h"

enum class AdminView
{
    MENU,
    ADD_DOC_NAME,
    ADD_DOC_SPEC,
    ADD_DOC_CONTACT,
    ADD_DOC_PASS,
    ADD_DOC_FEE,
    REMOVE_DOC,
    VIEW_PATIENTS,
    VIEW_DOCTORS,
    VIEW_APPTS,
    VIEW_UNPAID,
    DISCHARGE_ID,
    VIEW_SECLOG,
    DAILY_REPORT
};

class AdminScreen : public Screen
{
private:

    sf::Font font;
    HospitalSystem& system;
    int adminIndex;
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
    Button    menuBtns[9];

    AdminView currentView;

    // Temp storage for add-doctor multi-step
    char newDocName[60];
    char newDocSpec[60];
    char newDocContact[20];
    char newDocPass[60];
    char todayDate[20];

    static const char* menuLabels[9];

    static void myCopy(char* dst, const char* src, int maxLen);
    static bool myEq(const char* a, const char* b);
    static int  myAtoi(const char* s, bool& ok);
    static float myAtof(const char* s, bool& ok);
    static void myFtoa(float f, char* buf, int decimals = 2);
    static void loadToday(char* buf);

    void clearOutput();
    void addOutputLine(const char* line);
    void showMenu();
    void showMessage(const char* msg);

    void doViewPatients();
    void doViewDoctors();
    void doViewAppts();
    void doViewUnpaid();
    void doViewSecLog();
    void doDailyReport();

    void handleAddDocName(const char* s);
    void handleAddDocSpec(const char* s);
    void handleAddDocContact(const char* s);
    void handleAddDocPass(const char* s);
    void handleAddDocFee(const char* s);
    void handleRemoveDoc(const char* s);
    void handleDischarge(const char* s);

public:

    AdminScreen(HospitalSystem& sys);
    void init(int aIndex);

    void handleEvent(sf::Event& event, sf::RenderWindow& window) override;
    void update() override;
    void render(sf::RenderWindow& window) override;

    ScreenType getNextScreen() const override;
    bool       shouldSwitch()  const override;
    void       resetSwitch()         override;
    ~AdminScreen();
};
