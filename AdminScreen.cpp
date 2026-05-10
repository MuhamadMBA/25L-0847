#pragma warning(disable: 6262)
#include "AdminScreen.h"
#include "Validator.h"
#include <cstdio>
#include <ctime>

const char* AdminScreen::menuLabels[9] = {
    "1. Add Doctor",
    "2. Remove Doctor",
    "3. View All Patients",
    "4. View All Doctors",
    "5. View All Appointments",
    "6. View Unpaid Bills",
    "7. Discharge Patient",
    "8. View Security Log",
    "9. Generate Daily Report"
};

// ─── HELPERS ─────────────────────────────────────────────────
void AdminScreen::myCopy(char* dst, const char* src, int maxLen)
{
    int i = 0;
    while (src[i] && i < maxLen - 1) { dst[i] = src[i]; i++; }
    dst[i] = '\0';
}

bool AdminScreen::myEq(const char* a, const char* b)
{
    int i = 0;
    while (a[i] && b[i]) { if (a[i] != b[i]) return false; i++; }
    return a[i] == '\0' && b[i] == '\0';
}

int AdminScreen::myAtoi(const char* s, bool& ok)
{
    ok = true; int v = 0;
    for (int i = 0; s[i]; i++)
    {
        if (s[i] < '0' || s[i] > '9') { ok = false; return 0; }
        v = v * 10 + (s[i] - '0');
    }
    return v;
}

float AdminScreen::myAtof(const char* s, bool& ok)
{
    ok = true; float v = 0; int dec = -1;
    for (int i = 0; s[i]; i++)
    {
        if (s[i] == '.' && dec == -1) { dec = 0; continue; }
        if (s[i] < '0' || s[i] > '9') { ok = false; return 0; }
        if (dec >= 0) { v += (s[i] - '0') / (dec == 0 ? 10.f : (dec == 1 ? 100.f : 1000.f)); dec++; }
        else v = v * 10 + (s[i] - '0');
    }
    return v;
}

void AdminScreen::myFtoa(float f, char* buf, int decimals)
{
    int w = (int)f;
    char t[20];
    if (!w) { t[0] = '0'; t[1] = '\0'; }
    else
    {
        char r[20]; int i = 0, n = w;
        while (n > 0) { r[i++] = '0' + n % 10; n /= 10; }
        for (int j = 0; j < i; j++) t[j] = r[i - 1 - j];
        t[i] = '\0';
    }
    int l = 0;
    while (t[l]) { buf[l] = t[l]; l++; }
    buf[l++] = '.';
    float fr = f - w;
    for (int i = 0; i < decimals; i++)
    {
        fr *= 10;
        buf[l++] = '0' + (int)fr;
        fr -= (int)fr;
    }
    buf[l] = '\0';
}

void AdminScreen::loadToday(char* buf)
{
    time_t t = time(0);
    struct tm lt {};
    localtime_s(&lt, &t);
    snprintf(buf, 20, "%02d-%02d-%04d", lt.tm_mday, lt.tm_mon + 1, lt.tm_year + 1900);
}

// ─── CONSTRUCTOR ─────────────────────────────────────────────
AdminScreen::AdminScreen(HospitalSystem& sys)
    : system(sys), adminIndex(-1), switchFlag(false),
    currentView(AdminView::MENU), outputCount(0)
{
    newDocName[0] = '\0';
    newDocSpec[0] = '\0';
    newDocContact[0] = '\0';
    newDocPass[0] = '\0';
    todayDate[0] = '\0';

    font.loadFromFile("Assets/Arial.ttf");

    headerBar.setSize(sf::Vector2f(800.f, 60.f));
    headerBar.setPosition(0.f, 0.f);
    headerBar.setFillColor(sf::Color(70, 20, 80));

    headerText.setFont(font);
    headerText.setCharacterSize(22);
    headerText.setFillColor(sf::Color::White);
    headerText.setPosition(10.f, 15.f);

    outputPanel.setSize(sf::Vector2f(760.f, 390.f));
    outputPanel.setPosition(20.f, 70.f);
    outputPanel.setFillColor(sf::Color(18, 18, 32));
    outputPanel.setOutlineThickness(1);
    outputPanel.setOutlineColor(sf::Color(80, 40, 100));

    for (int i = 0; i < 30; i++)
    {
        outputLines[i].setFont(font);
        outputLines[i].setCharacterSize(16);
        outputLines[i].setFillColor(sf::Color(230, 210, 240));
        outputLines[i].setPosition(30.f, 76.f + i * 24.f);
    }

    inputLabel.setFont(font);
    inputLabel.setCharacterSize(18);
    inputLabel.setFillColor(sf::Color(210, 180, 220));
    inputLabel.setPosition(20.f, 473.f);

    inputBox.setup(font, 20.f, 498.f, 480.f, 40.f);
    inputBox.setSelected(true);

    actionBtn.setup(font, "Submit", 510.f, 498.f, 120.f, 40.f,
        sf::Color(120, 40, 140), sf::Color(160, 60, 190));
    backBtn.setup(font, "Back", 640.f, 498.f, 70.f, 40.f,
        sf::Color(80, 50, 20), sf::Color(120, 80, 30));
    logoutBtn.setup(font, "Logout", 720.f, 498.f, 70.f, 40.f,
        sf::Color(160, 30, 30), sf::Color(200, 50, 50));

    for (int i = 0; i < 9; i++)
        menuBtns[i].setup(font, menuLabels[i],
            20.f, 68.f + i * 44.f, 750.f, 38.f,
            sf::Color(55, 20, 65), sf::Color(85, 40, 100));
}

// ─── INIT ─────────────────────────────────────────────────────
void AdminScreen::init(int aIndex)
{
    adminIndex = aIndex;
    switchFlag = false;
    currentView = AdminView::MENU;
    loadToday(todayDate);
    char buf[80];
    snprintf(buf, 80, "Admin Panel - MediCore  |  %s", system.getAdmins().get(aIndex).getName());
    headerText.setString(buf);
    showMenu();
}

// ─── OUTPUT HELPERS ───────────────────────────────────────────
void AdminScreen::clearOutput()
{
    for (int i = 0; i < 30; i++) outputLines[i].setString("");
    outputCount = 0;
}

void AdminScreen::addOutputLine(const char* line)
{
    if (outputCount < 30) { outputLines[outputCount].setString(line); outputCount++; }
}

void AdminScreen::showMenu()
{
    currentView = AdminView::MENU;
    clearOutput();
    inputLabel.setString("");
    inputBox.clear();
}

void AdminScreen::showMessage(const char* msg)
{
    clearOutput();
    addOutputLine(msg);
    inputLabel.setString("Press Back to return.");
}

// ─── VIEW HANDLERS ───────────────────────────────────────────
void AdminScreen::doViewPatients()
{
    currentView = AdminView::VIEW_PATIENTS;
    clearOutput();
    const Storage<Patient>& pats = system.getPatients();
    const Storage<Bill>& bills = system.getBills();

    addOutputLine("ID  | Name                 | Age| G| Contact     | Balance   | Unpaid");
    addOutputLine("----+----------------------+----+--+-------------+-----------+-------");

    for (int i = 0; i < pats.size(); i++)
    {
        const Patient& p = pats.get(i);
        int unpaid = 0;
        for (int j = 0; j < bills.size(); j++)
            if (bills.get(j).getPatientID() == p.getID() &&
                myEq(bills.get(j).getStatus(), "Unpaid"))
                unpaid++;
        char bal[20]; myFtoa(p.getBalance(), bal);
        char un[10];  snprintf(un, 10, "%d", unpaid);
        char line[200];
        snprintf(line, 200, "%-4d| %-20s | %-3d| %c| %-11s | %-9s | %s",
            p.getID(), p.getName(), p.getAge(), p.getGender(),
            p.getContact(), bal, un);
        addOutputLine(line);
    }
    if (pats.size() == 0) addOutputLine("No patients registered.");
    inputLabel.setString("Press Back to return.");
}

void AdminScreen::doViewDoctors()
{
    currentView = AdminView::VIEW_DOCTORS;
    clearOutput();
    const Storage<Doctor>& docs = system.getDoctors();

    addOutputLine("ID  | Name                 | Specialization   | Contact     | Fee");
    addOutputLine("----+----------------------+------------------+-------------+------");

    for (int i = 0; i < docs.size(); i++)
    {
        const Doctor& d = docs.get(i);
        char fee[20]; myFtoa(d.getFee(), fee);
        char line[200];
        snprintf(line, 200, "%-4d| %-20s | %-16s | %-11s | %s",
            d.getID(), d.getName(), d.getSpecialization(),
            d.getContact(), fee);
        addOutputLine(line);
    }
    if (docs.size() == 0) addOutputLine("No doctors registered.");
    inputLabel.setString("Press Back to return.");
}

void AdminScreen::doViewAppts()
{
    currentView = AdminView::VIEW_APPTS;
    clearOutput();
    const Storage<Appointment>& appts = system.getAppointments();
    const Storage<Patient>& pats = system.getPatients();
    const Storage<Doctor>& docs = system.getDoctors();

    addOutputLine("ID  | Patient              | Doctor               | Date       | Time  | Status");
    addOutputLine("----+----------------------+----------------------+------------+-------+----------");

    // sort descending by date (bubble sort on indices)
    int indices[100]; int cnt = 0;
    for (int i = 0; i < appts.size() && cnt < 100; i++) indices[cnt++] = i;
    for (int i = 0; i < cnt - 1; i++)
        for (int j = 0; j < cnt - i - 1; j++)
        {
            const char* da = appts.get(indices[j]).getDate();
            const char* db = appts.get(indices[j + 1]).getDate();
            bool sw = false;
            for (int k = 6; k <= 9; k++)
            {
                if (da[k] > db[k]) { sw = false; break; }
                if (da[k] < db[k]) { sw = true;  break; }
            }
            if (sw) { int tmp = indices[j]; indices[j] = indices[j + 1]; indices[j + 1] = tmp; }
        }

    for (int i = 0; i < cnt; i++)
    {
        const Appointment& a = appts.get(indices[i]);
        int pi = pats.findByID(a.getPatientID());
        int di = docs.findByID(a.getDoctorID());
        char pn[50]; myCopy(pn, pi >= 0 ? pats.get(pi).getName() : "Unknown", 50);
        char dn[50]; myCopy(dn, di >= 0 ? docs.get(di).getName() : "Unknown", 50);
        char line[250];
        snprintf(line, 250, "%-4d| %-20s | %-20s | %s | %s | %s",
            a.getAppointmentID(), pn, dn,
            a.getDate(), a.getTime(), a.getStatus());
        addOutputLine(line);
    }
    if (cnt == 0) addOutputLine("No appointments.");
    inputLabel.setString("Press Back to return.");
}

void AdminScreen::doViewUnpaid()
{
    currentView = AdminView::VIEW_UNPAID;
    clearOutput();
    const Storage<Bill>& bills = system.getBills();
    const Storage<Patient>& pats = system.getPatients();

    addOutputLine("BillID | Patient Name         | Amount (PKR) | Date          ");
    addOutputLine("-------+----------------------+--------------+---------------");

    time_t now = time(0);
    bool any = false;
    for (int i = 0; i < bills.size(); i++)
    {
        const Bill& b = bills.get(i);
        if (!myEq(b.getStatus(), "Unpaid")) continue;
        any = true;
        int pi = pats.findByID(b.getPatientID());
        char pname[50]; myCopy(pname, pi >= 0 ? pats.get(pi).getName() : "Unknown", 50);
        char amt[20]; myFtoa(b.getAmount(), amt);

        // check overdue (> 7 days)
        const char* d = b.getDate(); // DD-MM-YYYY
        struct tm bt {};
        bt.tm_mday = (d[0] - '0') * 10 + (d[1] - '0');
        bt.tm_mon = (d[3] - '0') * 10 + (d[4] - '0') - 1;
        bt.tm_year = (d[6] - '0') * 1000 + (d[7] - '0') * 100 +
            (d[8] - '0') * 10 + (d[9] - '0') - 1900;
        time_t btime = mktime(&bt);
        bool overdue = difftime(now, btime) > 7.0 * 86400.0;

        char datecol[30];
        if (overdue) snprintf(datecol, 30, "%s [OVERDUE]", d);
        else myCopy(datecol, d, 30);

        char line[200];
        snprintf(line, 200, "%-7d| %-20s | %-12s | %s",
            b.getBillID(), pname, amt, datecol);
        addOutputLine(line);
    }
    if (!any) addOutputLine("No unpaid bills.");
    inputLabel.setString("Press Back to return.");
}

void AdminScreen::doViewSecLog()
{
    currentView = AdminView::VIEW_SECLOG;
    clearOutput();
    FILE* f = nullptr;
    fopen_s(&f, "DataFiles/security_log.txt", "r");
    if (!f) { addOutputLine("No security events logged."); inputLabel.setString("Press Back."); return; }
    char line[256]; bool any = false;
    while (fgets(line, 256, f) && outputCount < 28)
    {
        int l = 0;
        while (line[l] && line[l] != '\n' && line[l] != '\r') l++;
        line[l] = '\0';
        addOutputLine(line);
        any = true;
    }
    fclose(f);
    if (!any) addOutputLine("No security events logged.");
    inputLabel.setString("Press Back to return.");
}

void AdminScreen::doDailyReport()
{
    currentView = AdminView::DAILY_REPORT;
    clearOutput();

    char title[60]; snprintf(title, 60, "Daily Report - %s", todayDate);
    addOutputLine(title);
    addOutputLine("==========================================");

    const Storage<Appointment>& appts = system.getAppointments();
    const Storage<Bill>& bills = system.getBills();
    const Storage<Patient>& pats = system.getPatients();
    const Storage<Doctor>& docs = system.getDoctors();

    int total = 0, pending = 0, completed = 0, noshow = 0, cancelled = 0;
    float revenue = 0;

    for (int i = 0; i < appts.size(); i++)
    {
        if (!myEq(appts.get(i).getDate(), todayDate)) continue;
        total++;
        if (myEq(appts.get(i).getStatus(), "Scheduled"))  pending++;
        else if (myEq(appts.get(i).getStatus(), "Completed"))  completed++;
        else if (myEq(appts.get(i).getStatus(), "No-Show"))    noshow++;
        else if (myEq(appts.get(i).getStatus(), "Cancelled"))  cancelled++;
    }
    for (int i = 0; i < bills.size(); i++)
        if (myEq(bills.get(i).getDate(), todayDate) &&
            myEq(bills.get(i).getStatus(), "Paid"))
            revenue += bills.get(i).getAmount();

    char line[120];
    snprintf(line, 120, "Appointments: %d  (Scheduled:%d  Completed:%d  No-Show:%d  Cancelled:%d)",
        total, pending, completed, noshow, cancelled);
    addOutputLine(line);

    char rev[20]; myFtoa(revenue, rev);
    char rl[60];  snprintf(rl, 60, "Revenue collected: PKR %s", rev);
    addOutputLine(rl);
    addOutputLine("---");

    addOutputLine("Patients with unpaid bills:");
    for (int i = 0; i < pats.size(); i++)
    {
        float owed = 0;
        for (int j = 0; j < bills.size(); j++)
            if (bills.get(j).getPatientID() == pats.get(i).getID() &&
                myEq(bills.get(j).getStatus(), "Unpaid"))
                owed += bills.get(j).getAmount();
        if (owed > 0)
        {
            char ow[20]; myFtoa(owed, ow);
            char pl[100]; snprintf(pl, 100, "  %-20s  PKR %s", pats.get(i).getName(), ow);
            addOutputLine(pl);
        }
    }
    addOutputLine("---");

    addOutputLine("Doctor Summary:");
    for (int i = 0; i < docs.size(); i++)
    {
        int did = docs.get(i).getID();
        int dc = 0, dp = 0, dn = 0;
        for (int j = 0; j < appts.size(); j++)
        {
            if (appts.get(j).getDoctorID() != did ||
                !myEq(appts.get(j).getDate(), todayDate)) continue;
            if (myEq(appts.get(j).getStatus(), "Completed"))  dc++;
            else if (myEq(appts.get(j).getStatus(), "Scheduled"))  dp++;
            else if (myEq(appts.get(j).getStatus(), "No-Show"))    dn++;
        }
        if (dc + dp + dn > 0)
        {
            char dl[150];
            snprintf(dl, 150, "  %-20s  Done:%d  Pend:%d  NoShow:%d",
                docs.get(i).getName(), dc, dp, dn);
            addOutputLine(dl);
        }
    }
    inputLabel.setString("Press Back to return.");
}

// ─── ADD / REMOVE DOCTOR ─────────────────────────────────────
void AdminScreen::handleAddDocName(const char* s)
{
    if (!s[0]) { showMessage("Name cannot be empty."); return; }
    myCopy(newDocName, s, 60);
    currentView = AdminView::ADD_DOC_SPEC;
    clearOutput();
    addOutputLine("Step 2/5: Enter specialization.");
    inputLabel.setString("Specialization:");
    inputBox.clear(); inputBox.setSelected(true);
}

void AdminScreen::handleAddDocSpec(const char* s)
{
    if (!s[0]) { showMessage("Specialization cannot be empty."); return; }
    myCopy(newDocSpec, s, 60);
    currentView = AdminView::ADD_DOC_CONTACT;
    clearOutput();
    addOutputLine("Step 3/5: Contact must be exactly 11 digits.");
    inputLabel.setString("Contact (11 digits):");
    inputBox.clear(); inputBox.setSelected(true);
}

void AdminScreen::handleAddDocContact(const char* s)
{
    if (!Validator::isValidContact(s))
    {
        showMessage("Invalid contact. Must be 11 numeric digits.");
        return;
    }
    myCopy(newDocContact, s, 20);
    currentView = AdminView::ADD_DOC_PASS;
    clearOutput();
    addOutputLine("Step 4/5: Password (min 6 chars, no spaces).");
    inputLabel.setString("Password:");
    inputBox.clear(); inputBox.setSelected(true);
}

void AdminScreen::handleAddDocPass(const char* s)
{
    if (!Validator::isValidPassword(s))
    {
        showMessage("Password must be at least 6 characters with no spaces.");
        return;
    }
    myCopy(newDocPass, s, 60);
    currentView = AdminView::ADD_DOC_FEE;
    clearOutput();
    addOutputLine("Step 5/5: Consultation fee (positive number, e.g. 1500).");
    inputLabel.setString("Fee (PKR):");
    inputBox.clear(); inputBox.setSelected(true);
}

void AdminScreen::handleAddDocFee(const char* s)
{
    // strip leading whitespace / carriage returns
    int start = 0;
    while (s[start] == ' ' || s[start] == '\r' || s[start] == '\n') start++;
    bool ok;
    float fee = myAtof(s + start, ok);
    if (!ok || fee <= 0)
    {
        showMessage("Invalid fee. Must be a positive number (e.g. 1500).");
        return;
    }
    bool added = system.addDoctor(newDocName, newDocSpec, newDocContact, newDocPass, fee);
    showMessage(added ? "Doctor added successfully!" : "Failed to add doctor. Storage may be full.");
    currentView = AdminView::MENU;
}

void AdminScreen::handleRemoveDoc(const char* s)
{
    bool ok; int id = myAtoi(s, ok);
    if (!ok || id <= 0) { showMessage("Invalid Doctor ID."); return; }

    const Storage<Appointment>& appts = system.getAppointments();
    for (int i = 0; i < appts.size(); i++)
        if (appts.get(i).getDoctorID() == id &&
            myEq(appts.get(i).getStatus(), "Scheduled"))
        {
            showMessage("Cannot remove doctor with pending appointments.");
            return;
        }

    bool done = system.removeDoctor(id);
    showMessage(done ? "Doctor removed successfully." : "Doctor not found.");
    currentView = AdminView::MENU;
}

void AdminScreen::handleDischarge(const char* s)
{
    bool ok; int pid = myAtoi(s, ok);
    if (!ok || pid <= 0) { showMessage("Invalid Patient ID."); return; }
    bool done = system.dischargePatient(pid);
    showMessage(done
        ? "Patient discharged and archived successfully."
        : "Discharge failed. Patient has unpaid bills or pending appointments.");
    currentView = AdminView::MENU;
}

// ─── EVENT HANDLER ───────────────────────────────────────────
void AdminScreen::handleEvent(sf::Event& event, sf::RenderWindow& window)
{
    // Handle input FIRST so text is registered before submit check
    inputBox.handleInput(event);

    sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left)
        inputBox.setSelected(inputBox.contains(mouse));

    if (currentView == AdminView::MENU)
    {
        for (int i = 0; i < 9; i++)
        {
            if (menuBtns[i].isClicked(event, mouse))
            {
                if (i == 0)
                {
                    currentView = AdminView::ADD_DOC_NAME;
                    clearOutput();
                    addOutputLine("Step 1/5: Enter doctor name (max 50 chars).");
                    inputLabel.setString("Doctor Name:");
                    inputBox.clear(); inputBox.setSelected(true);
                }
                else if (i == 1)
                {
                    currentView = AdminView::REMOVE_DOC;
                    doViewDoctors();
                    inputLabel.setString("Enter Doctor ID to remove:");
                    inputBox.clear(); inputBox.setSelected(true);
                }
                else if (i == 2) doViewPatients();
                else if (i == 3) doViewDoctors();
                else if (i == 4) doViewAppts();
                else if (i == 5) doViewUnpaid();
                else if (i == 6)
                {
                    currentView = AdminView::DISCHARGE_ID;
                    clearOutput();
                    addOutputLine("Enter the Patient ID to discharge.");
                    inputLabel.setString("Patient ID:");
                    inputBox.clear(); inputBox.setSelected(true);
                }
                else if (i == 7) doViewSecLog();
                else if (i == 8) doDailyReport();
                return;
            }
        }
        return; // nothing else to do in MENU view
    }

    // Submit via button click OR Enter key
    bool submitted = actionBtn.isClicked(event, mouse)
        || (event.type == sf::Event::KeyPressed &&
            event.key.code == sf::Keyboard::Return);

    if (submitted)
    {
        const char* inp = inputBox.getText();
        if (currentView == AdminView::ADD_DOC_NAME)    handleAddDocName(inp);
        else if (currentView == AdminView::ADD_DOC_SPEC)    handleAddDocSpec(inp);
        else if (currentView == AdminView::ADD_DOC_CONTACT) handleAddDocContact(inp);
        else if (currentView == AdminView::ADD_DOC_PASS)    handleAddDocPass(inp);
        else if (currentView == AdminView::ADD_DOC_FEE)     handleAddDocFee(inp);
        else if (currentView == AdminView::REMOVE_DOC)      handleRemoveDoc(inp);
        else if (currentView == AdminView::DISCHARGE_ID)    handleDischarge(inp);
    }

    if (backBtn.isClicked(event, mouse))    showMenu();
    if (logoutBtn.isClicked(event, mouse))  switchFlag = true;
}

// ─── UPDATE / RENDER ─────────────────────────────────────────
void AdminScreen::update() {}

void AdminScreen::render(sf::RenderWindow& window)
{
    window.draw(headerBar);
    window.draw(headerText);

    if (currentView == AdminView::MENU)
    {
        for (int i = 0; i < 9; i++) menuBtns[i].draw(window);
    }
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

// ─── SCREEN INTERFACE ────────────────────────────────────────
ScreenType AdminScreen::getNextScreen() const { return ScreenType::LOGIN; }
bool       AdminScreen::shouldSwitch()  const { return switchFlag; }
void       AdminScreen::resetSwitch() { switchFlag = false; }

AdminScreen::~AdminScreen() {}