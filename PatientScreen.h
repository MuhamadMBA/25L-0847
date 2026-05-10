#pragma once

#include <SFML/Graphics.hpp>
#include "Screen.h"
#include "Button.h"
#include "TextBox.h"
#include "HospitalSystem.h"

// Sub-view the patient is currently in
enum class PatientView
{
    MENU,
    BOOK_SPEC,     // enter specialization
    BOOK_DOC,      // pick doctor from list
    BOOK_DATE,     // enter date
    BOOK_SLOT,     // pick slot
    CANCEL,        // cancel appointment
    VIEW_APPTS,
    VIEW_RECORDS,
    VIEW_BILLS,
    PAY_BILL,
    TOPUP
};

class PatientScreen : public Screen
{
private:

    sf::Font font;

    // Shared data
    HospitalSystem& system;
    int patientIndex;

    // Switch-back
    bool switchFlag;

    // UI: header
    sf::RectangleShape headerBar;
    sf::Text           headerText;
    sf::Text           balanceText;

    // Scrollable output area
    sf::RectangleShape outputPanel;
    sf::Text* outputLines;
    int                outputCount;

    // Input row (single textbox + action button)
    sf::Text    inputLabel;
    TextBox     inputBox;
    Button      actionBtn;
    Button      backBtn;
    Button      logoutBtn;

    // 7 menu buttons
    Button menuBtns[7];
    static const char* menuLabels[7];

    // State
    PatientView currentView;

    // Booking temporaries
    char  bookSpec[60];
    int   bookDocID;
    char  bookDate[20];
    int   bookDocResults[20];
    int   bookDocCount;
    char  availSlots[8][10];
    int   availSlotCount;

    // Used for cancel/pay selection
    int   pendingIDs[100];
    int   pendingCount;

    // helpers
    void clearOutput();
    void addOutputLine(const char* line);
    void updateBalanceText();
    void showMenu();
    void showMessage(const char* msg);

    // action handlers
    void handleBookSpec(const char* input);
    void handleBookDoc(const char* input);
    void handleBookDate(const char* input);
    void handleBookSlot(const char* input);
    void handleCancel(const char* input);
    void handlePayBill(const char* input);
    void handleTopUp(const char* input);

    void doViewAppointments();
    void doViewRecords();
    void doViewBills();
    void startCancel();
    void startPayBill();

    // char helpers (no std::string)
    static void myCopy(char* dst, const char* src, int maxLen);
    static int  myStrLen(const char* s);
    static bool myEq(const char* a, const char* b);
    static int  myAtoi(const char* s, bool& ok);
    static float myAtof(const char* s, bool& ok);
    static void myItoa(int n, char* buf);
    static void myFtoa(float f, char* buf, int decimals = 2);
    static void myConcat(char* dst, const char* a, const char* b, int maxLen);

public:

    PatientScreen(HospitalSystem& sys);

    void init(int pIndex);

    void handleEvent(sf::Event& event, sf::RenderWindow& window) override;
    void update() override;
    void render(sf::RenderWindow& window) override;

    ScreenType getNextScreen() const override;
    bool       shouldSwitch()  const override;
    void       resetSwitch()         override;
    ~PatientScreen();
};

