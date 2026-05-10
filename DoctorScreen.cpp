#include "DoctorScreen.h"
#include <cstdio>
#include <ctime>

const char* DoctorScreen::menuLabels[5] = {
    "1. View Today's Appointments",
    "2. Mark Appointment Complete",
    "3. Mark Appointment No-Show",
    "4. Write Prescription",
    "5. View Patient Medical History"
};

void DoctorScreen::myCopy(char* dst, const char* src, int maxLen)
{
    int i = 0; while (src[i] && i < maxLen - 1) { dst[i] = src[i];i++; } dst[i] = '\0';
}
bool DoctorScreen::myEq(const char* a, const char* b)
{
    int i = 0; while (a[i] && b[i]) { if (a[i] != b[i])return false;i++; } return a[i] == '\0' && b[i] == '\0';
}
int  DoctorScreen::myAtoi(const char* s, bool& ok)
{
    ok = true;int v = 0; for (int i = 0;s[i];i++) { if (s[i] < '0' || s[i]>'9') { ok = false;return 0; }v = v * 10 + (s[i] - '0'); } return v;
}
void DoctorScreen::myItoa(int n, char* buf)
{
    if (!n) { buf[0] = '0';buf[1] = '\0';return; } char t[20];int i = 0; while (n > 0) { t[i++] = '0' + n % 10;n /= 10; } for (int j = 0;j < i;j++)buf[j] = t[i - 1 - j];buf[i] = '\0';
}
int DoctorScreen:: myLen(const char* s)
{
    int i = 0;
    while (s[i]) i++;
    return i;
}


DoctorScreen::DoctorScreen(HospitalSystem& sys)
    : system(sys), doctorIndex(-1), switchFlag(false),
    currentView(DoctorView::MENU), rxAppointmentID(-1), outputCount(0)
{
    rxMedicines[0] = '\0';
    todayDate[0] = '\0';
    font.loadFromFile("Assets/Arial.ttf");

    headerBar.setSize(sf::Vector2f(800.f, 60.f));
    headerBar.setPosition(0.f, 0.f);
    headerBar.setFillColor(sf::Color(20, 80, 60));

    headerText.setFont(font);
    headerText.setCharacterSize(22);
    headerText.setFillColor(sf::Color::White);
    headerText.setPosition(10.f, 15.f);

    outputPanel.setSize(sf::Vector2f(760.f, 390.f));
    outputPanel.setPosition(20.f, 70.f);
    outputPanel.setFillColor(sf::Color(18, 18, 32));
    outputPanel.setOutlineThickness(1);
    outputPanel.setOutlineColor(sf::Color(40, 100, 80));

    for (int i = 0; i < 30; i++)
    {
        outputLines[i].setFont(font);
        outputLines[i].setCharacterSize(17);
        outputLines[i].setFillColor(sf::Color(200, 240, 210));
        outputLines[i].setPosition(30.f, 76.f + i * 24.f);
    }

    inputLabel.setFont(font);
    inputLabel.setCharacterSize(18);
    inputLabel.setFillColor(sf::Color(180, 220, 200));
    inputLabel.setPosition(20.f, 473.f);

    inputBox.setup(font, 20.f, 498.f, 480.f, 40.f);
    inputBox.setSelected(true);

    actionBtn.setup(font, "Submit", 510.f, 498.f, 120.f, 40.f,
        sf::Color(20, 140, 80), sf::Color(30, 180, 100));
    backBtn.setup(font, "Back", 640.f, 498.f, 70.f, 40.f,
        sf::Color(100, 80, 20), sf::Color(150, 120, 30));
    logoutBtn.setup(font, "Logout", 720.f, 498.f, 70.f, 40.f,
        sf::Color(160, 30, 30), sf::Color(200, 50, 50));

    for (int i = 0; i < 5; i++)
        menuBtns[i].setup(font, menuLabels[i],
            20.f, 75.f + i * 58.f, 750.f, 50.f,
            sf::Color(25, 65, 50), sf::Color(40, 100, 80));
}

void DoctorScreen::loadTodayDate()
{
    time_t t = time(0);
    struct tm lt {};
    localtime_s(&lt, &t);    
    snprintf(todayDate, 20, "%02d-%02d-%04d", lt.tm_mday, lt.tm_mon + 1, lt.tm_year + 1900);
}

void DoctorScreen::init(int dIndex)
{
    doctorIndex = dIndex;
    switchFlag = false;
    currentView = DoctorView::MENU;
    loadTodayDate();
    char buf[120];
    snprintf(buf, 120, "Dr. %s  |  %s  |  Today: %s",
        system.getDoctors().get(dIndex).getName(),
        system.getDoctors().get(dIndex).getSpecialization(),
        todayDate);
    headerText.setString(buf);
    showMenu();
}

void DoctorScreen::clearOutput()
{
    for (int i = 0;i < 30;i++) outputLines[i].setString(""); outputCount = 0;
}
void DoctorScreen::addOutputLine(const char* line)
{
    if (outputCount < 30) { outputLines[outputCount].setString(line); outputCount++; }
}
void DoctorScreen::showMenu()
{
    currentView = DoctorView::MENU; clearOutput(); inputLabel.setString(""); inputBox.clear();
}
void DoctorScreen::showMessage(const char* msg)
{
    clearOutput(); addOutputLine(msg); inputLabel.setString("Press Back to return.");
}

void DoctorScreen::doTodayAppts()
{
    currentView = DoctorView::TODAY_APPTS;
    clearOutput();
    int did = system.getDoctors().get(doctorIndex).getID();
    const Storage<Appointment>& appts = system.getAppointments();
    const Storage<Patient>& pats = system.getPatients();

    int indices[50]; int cnt = 0;
    for (int i = 0; i < appts.size() && cnt < 50; i++)
    {
        const Appointment& a = appts.get(i);
        if (a.getDoctorID() == did && myEq(a.getDate(), todayDate))
            indices[cnt++] = i;
    }

    // sort by time slot ascending (simple bubble)
    for (int i = 0; i < cnt - 1; i++)
        for (int j = 0; j < cnt - i - 1; j++)
        {
            bool swap = false;
            const char* ta = appts.get(indices[j]).getTime();
            const char* tb = appts.get(indices[j + 1]).getTime();
            for (int k = 0; ta[k] || tb[k]; k++)
            {
                if (ta[k] < tb[k]) { swap = false;break; }
                if (ta[k] > tb[k]) { swap = true;break; }
            }
            if (swap) { int tmp = indices[j];indices[j] = indices[j + 1];indices[j + 1] = tmp; }
        }

    if (cnt == 0) { addOutputLine("No appointments scheduled for today."); }
    else
    {
        addOutputLine("ApptID | Patient Name         | Time  | Status");
        addOutputLine("-------+----------------------+-------+-----------");
        for (int i = 0; i < cnt; i++)
        {
            const Appointment& a = appts.get(indices[i]);
            int pi = pats.findByID(a.getPatientID());
            char pname[50]; myCopy(pname, pi >= 0 ? pats.get(pi).getName() : "Unknown", 50);
            char line[120];
            snprintf(line, 120, "%-7d| %-20s | %s | %s",
                a.getAppointmentID(), pname, a.getTime(), a.getStatus());
            addOutputLine(line);
        }
    }
    inputLabel.setString("Press Back to return.");
}

void DoctorScreen::handleMarkComplete(const char* input)
{
    bool ok; int id = myAtoi(input, ok);
    if (!ok || id <= 0) { showMessage("Invalid ID."); currentView = DoctorView::MENU; return; }
    int did = system.getDoctors().get(doctorIndex).getID();
    bool done = system.markAppointmentComplete(id, did);
    showMessage(done ? "Appointment marked as completed." : "Failed. Check ID, status, and that it is today.");
    currentView = DoctorView::MENU;
}

void DoctorScreen::handleMarkNoShow(const char* input)
{
    bool ok; int id = myAtoi(input, ok);
    if (!ok || id <= 0) { showMessage("Invalid ID."); currentView = DoctorView::MENU; return; }
    int did = system.getDoctors().get(doctorIndex).getID();
    bool done = system.markNoShow(id, did);
    showMessage(done ? "Appointment marked as no-show." : "Failed. Check ID, status, and that it is today.");
    currentView = DoctorView::MENU;
}

void DoctorScreen::handleRxID(const char* input)
{
    bool ok; int id = myAtoi(input, ok);
    if (!ok || id <= 0) { showMessage("Invalid appointment ID."); currentView = DoctorView::MENU; return; }

    int did = system.getDoctors().get(doctorIndex).getID();
    const Storage<Appointment>& appts = system.getAppointments();
    int ai = appts.findByID(id);
    if (ai < 0 || appts.get(ai).getDoctorID() != did || !myEq(appts.get(ai).getStatus(), "completed"))
    {
        showMessage("Appointment not found, not yours, or not completed.");
        currentView = DoctorView::MENU; return;
    }

    // Check no existing prescription
    const Storage<Prescription>& rxs = system.getPrescriptions();
    for (int i = 0; i < rxs.size(); i++)
        if (rxs.get(i).getAppointmentID() == id)
        {
            showMessage("Prescription already written for this appointment.");
            currentView = DoctorView::MENU; return;
        }

    rxAppointmentID = id;
    currentView = DoctorView::WRITE_RX_MED;
    clearOutput();
    addOutputLine("Format: MedicineName Dosage;MedicineName Dosage");
    addOutputLine("Example: Paracetamol 500mg;Amoxicillin 250mg");
    inputLabel.setString("Enter medicines:");
    inputBox.clear(); inputBox.setSelected(true);
}

void DoctorScreen::handleRxMed(const char* input)
{
    if (myLen(input) == 0) 
    {
        showMessage("Medicines cannot be empty."); currentView = DoctorView::MENU; 
        return; 
    }
    myCopy(rxMedicines, input, 500);
    currentView = DoctorView::WRITE_RX_NOTES;
    clearOutput();
    inputLabel.setString("Enter notes (max 300 chars):");
    inputBox.clear(); inputBox.setSelected(true);
}

void DoctorScreen::handleRxNotes(const char* input)
{
    int did = system.getDoctors().get(doctorIndex).getID();
    bool done = system.writePrescription(rxAppointmentID, did, rxMedicines, input, todayDate);
    showMessage(done ? "Prescription saved." : "Failed to save prescription.");
    currentView = DoctorView::MENU;
}

void DoctorScreen::handleHistoryID(const char* input)
{
    bool ok; int pid = myAtoi(input, ok);
    if (!ok || pid <= 0) { showMessage("Invalid Patient ID."); currentView = DoctorView::MENU; return; }

    int did = system.getDoctors().get(doctorIndex).getID();
    // Verify doctor has at least one completed appointment with this patient
    const Storage<Appointment>& appts = system.getAppointments();
    bool allowed = false;
    for (int i = 0; i < appts.size(); i++)
        if (appts.get(i).getPatientID() == pid && appts.get(i).getDoctorID() == did
            && myEq(appts.get(i).getStatus(), "completed")) {
            allowed = true; break;
        }

    if (!allowed) { showMessage("Access denied. You can only view records of your own patients."); currentView = DoctorView::MENU; return; }

    clearOutput();
    const Storage<Prescription>& rxs = system.getPrescriptions();
    int indices[50]; int cnt = 0;
    for (int i = 0;i < rxs.size() && cnt < 50;i++)
        if (rxs.get(i).getPatientID() == pid && rxs.get(i).getDoctorID() == did) indices[cnt++] = i;

    if (cnt == 0) { addOutputLine("No records found."); inputLabel.setString("Press Back."); return; }

    // sort descending by date
    for (int i = 0;i < cnt - 1;i++) for (int j = 0;j < cnt - i - 1;j++)
    {
        const char* da = rxs.get(indices[j]).getDate(); const char* db = rxs.get(indices[j + 1]).getDate();
        bool sw = false;
        for (int k = 6;k <= 9;k++) { if (da[k] > db[k]) { sw = false;break; }if (da[k] < db[k]) { sw = true;break; } }
        if (sw) { int t = indices[j];indices[j] = indices[j + 1];indices[j + 1] = t; }
    }

    addOutputLine("Date       | Medicines                         | Notes");
    addOutputLine("-----------+-----------------------------------+------------------");
    for (int i = 0;i < cnt;i++)
    {
        const Prescription& p = rxs.get(indices[i]);
        char line[250];
        snprintf(line, 250, "%s | %-33s | %s", p.getDate(), p.getMedicines(), p.getNotes());
        addOutputLine(line);
    }
    inputLabel.setString("Press Back to return.");
    currentView = DoctorView::TODAY_APPTS; // reuse non-menu state for display
}

void DoctorScreen::handleEvent(sf::Event& event, sf::RenderWindow& window)
{
    sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
        inputBox.setSelected(inputBox.contains(mouse));

    if (currentView == DoctorView::MENU)
    {
        for (int i = 0; i < 5; i++)
        {
            if (menuBtns[i].isClicked(event, mouse))
            {
                if (i == 0) doTodayAppts();
                else if (i == 1) {
                    currentView = DoctorView::MARK_COMPLETE_INPUT; clearOutput();
                    addOutputLine("Enter ID of a pending appointment to mark complete.");
                    inputLabel.setString("Appointment ID:"); inputBox.clear(); inputBox.setSelected(true);
                }
                else if (i == 2) {
                    currentView = DoctorView::MARK_NOSHOW_INPUT; clearOutput();
                    addOutputLine("Enter ID of a pending appointment to mark no-show.");
                    inputLabel.setString("Appointment ID:"); inputBox.clear(); inputBox.setSelected(true);
                }
                else if (i == 3) {
                    currentView = DoctorView::WRITE_RX_ID; clearOutput();
                    addOutputLine("Enter the completed appointment ID to write a prescription for.");
                    inputLabel.setString("Appointment ID:"); inputBox.clear(); inputBox.setSelected(true);
                }
                else if (i == 4) {
                    currentView = DoctorView::VIEW_HISTORY_ID; clearOutput();
                    inputLabel.setString("Enter Patient ID:"); inputBox.clear(); inputBox.setSelected(true);
                }
                return;
            }
        }
    }

    bool submitted = actionBtn.isClicked(event, mouse)
        || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return);

    if (submitted)
    {
        const char* inp = inputBox.getText();
        if (currentView == DoctorView::MARK_COMPLETE_INPUT) handleMarkComplete(inp);
        else if (currentView == DoctorView::MARK_NOSHOW_INPUT)   handleMarkNoShow(inp);
        else if (currentView == DoctorView::WRITE_RX_ID)         handleRxID(inp);
        else if (currentView == DoctorView::WRITE_RX_MED)        handleRxMed(inp);
        else if (currentView == DoctorView::WRITE_RX_NOTES)      handleRxNotes(inp);
        else if (currentView == DoctorView::VIEW_HISTORY_ID)     handleHistoryID(inp);
    }

    if (backBtn.isClicked(event, mouse)) showMenu();
    if (logoutBtn.isClicked(event, mouse)) switchFlag = true;

    inputBox.handleInput(event);
}

void DoctorScreen::update() {}

void DoctorScreen::render(sf::RenderWindow& window)
{
    window.draw(headerBar);
    window.draw(headerText);

    if (currentView == DoctorView::MENU)
        for (int i = 0; i < 5; i++) menuBtns[i].draw(window);
    else
    {
        window.draw(outputPanel);
        for (int i = 0; i < outputCount; i++) window.draw(outputLines[i]);
        window.draw(inputLabel);
        inputBox.draw(window);
        actionBtn.draw(window);
    }

    backBtn.draw(window);
    logoutBtn.draw(window);
}

ScreenType DoctorScreen::getNextScreen() const { return ScreenType::LOGIN; }
bool       DoctorScreen::shouldSwitch()  const { return switchFlag; }
void       DoctorScreen::resetSwitch() { switchFlag = false; }
