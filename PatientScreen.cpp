#pragma warning(disable: 6262)
#include "PatientScreen.h"
#include "Validator.h"
#include "SlotUnavailableException.h"
#include "InsufficientFundsException.h"
#include <cstdio>
#include <ctime>


const char* PatientScreen::menuLabels[7] = {
    "1. Book Appointment",
    "2. Cancel Appointment",
    "3. View Appointments",
    "4. View Medical Records",
    "5. View Bills",
    "6. Pay Bill",
    "7. Top Up Balance"
};

// ─── char helpers ────────────────────────────────────────────
void PatientScreen::myCopy(char* dst, const char* src, int maxLen)
{
    int i = 0;
    while (src[i] && i < maxLen - 1) { dst[i] = src[i]; i++; }
    dst[i] = '\0';
}

int PatientScreen::myStrLen(const char* s)
{
    int i = 0;
    while (s[i]) i++;
    return i;
}
bool PatientScreen::myEq(const char* a, const char* b)
{
    int i = 0;
    while (a[i] && b[i]) { if (a[i] != b[i]) return false; i++; }
    return a[i] == '\0' && b[i] == '\0';
}

int PatientScreen::myAtoi(const char* s, bool& ok)
{
    ok = true;
    int v = 0;
    for (int i = 0; s[i]; i++)
    {
        if (s[i] < '0' || s[i] > '9') { ok = false; return 0; }
        v = v * 10 + (s[i] - '0');
    }
    return v;
}

float PatientScreen::myAtof(const char* s, bool& ok)
{
    ok = true;
    float v = 0; int dec = -1; int i = 0;
    for (; s[i]; i++)
    {
        if (s[i] == '.' && dec == -1) { dec = 0; continue; }
        if (s[i] < '0' || s[i] > '9') { ok = false; return 0; }
        if (dec >= 0) { v += (s[i] - '0') / (dec == 0 ? 10.f : (dec == 1 ? 100.f : 1000.f)); dec++; }
        else v = v * 10 + (s[i] - '0');
    }
    return v;
}

void PatientScreen::myItoa(int n, char* buf)
{
    if (n == 0) { buf[0] = '0'; buf[1] = '\0'; return; }
    char tmp[20]; int i = 0;
    while (n > 0) { tmp[i++] = '0' + n % 10; n /= 10; }
    for (int j = 0; j < i; j++) buf[j] = tmp[i - 1 - j];
    buf[i] = '\0';
}

void PatientScreen::myFtoa(float f, char* buf, int decimals)
{
    int whole = (int)f;
    myItoa(whole, buf);
    int len = myStrLen(buf);
    buf[len] = '.';
    float frac = f - whole;
    for (int i = 0; i < decimals; i++)
    {
        frac *= 10;
        buf[len + 1 + i] = '0' + (int)frac;
        frac -= (int)frac;
    }
    buf[len + 1 + decimals] = '\0';
}

void PatientScreen::myConcat(char* dst, const char* a, const char* b, int maxLen)
{
    int i = 0, j = 0;
    while (a[i] && i < maxLen - 1) { dst[i] = a[i]; i++; }
    while (b[j] && i < maxLen - 1) { dst[i] = b[j]; i++; j++; }
    dst[i] = '\0';
}
// ─── Constructor ─────────────────────────────────────────────
PatientScreen::PatientScreen(HospitalSystem& sys)
    : system(sys), patientIndex(-1), switchFlag(false), outputLines(nullptr),
    currentView(PatientView::MENU), bookDocID(-1),
    bookDocCount(0), availSlotCount(0), pendingCount(0), outputCount(0)
{
    outputLines = new sf::Text[30];
    bookSpec[0] = '\0';
    bookDate[0] = '\0';
    bookDocResults[0] = 0;
    availSlots[0][0] = '\0';
    pendingIDs[0] = 0;

    font.loadFromFile("Assets/Arial.ttf");

    // Header
    headerBar.setSize(sf::Vector2f(800.f, 60.f));
    headerBar.setPosition(0.f, 0.f);
    headerBar.setFillColor(sf::Color(20, 60, 120));

    headerText.setFont(font);
    headerText.setCharacterSize(22);
    headerText.setFillColor(sf::Color::White);
    headerText.setPosition(10.f, 15.f);

    balanceText.setFont(font);
    balanceText.setCharacterSize(20);
    balanceText.setFillColor(sf::Color(100, 255, 160));
    balanceText.setPosition(560.f, 18.f);

    // Output panel
    outputPanel.setSize(sf::Vector2f(760.f, 390.f));
    outputPanel.setPosition(20.f, 70.f);
    outputPanel.setFillColor(sf::Color(18, 18, 32));
    outputPanel.setOutlineThickness(1);
    outputPanel.setOutlineColor(sf::Color(60, 80, 140));

    for (int i = 0; i < 30; i++)
    {
        outputLines[i].setFont(font);
        outputLines[i].setCharacterSize(17);
        outputLines[i].setFillColor(sf::Color(200, 210, 240));
        outputLines[i].setPosition(30.f, 76.f + i * 24.f);
    }

    // Input area
    inputLabel.setFont(font);
    inputLabel.setCharacterSize(18);
    inputLabel.setFillColor(sf::Color(180, 200, 240));
    inputLabel.setPosition(20.f, 473.f);

    inputBox.setup(font, 20.f, 498.f, 480.f, 40.f);
    inputBox.setSelected(true);

    actionBtn.setup(font, "Submit", 510.f, 498.f, 120.f, 40.f,
        sf::Color(20, 140, 80), sf::Color(30, 180, 100));

    backBtn.setup(font, "Back", 640.f, 498.f, 70.f, 40.f,
        sf::Color(100, 60, 20), sf::Color(150, 90, 30));

    logoutBtn.setup(font, "Logout", 720.f, 498.f, 70.f, 40.f,
        sf::Color(160, 30, 30), sf::Color(200, 50, 50));

    // 7 menu buttons
    for (int i = 0; i < 7; i++)
    {
        menuBtns[i].setup(font, menuLabels[i],
            20.f, 75.f + i * 54.f, 750.f, 46.f,
            sf::Color(35, 55, 110), sf::Color(50, 80, 160));
    }
}

void PatientScreen::init(int pIndex)
{
    patientIndex = pIndex;
    currentView = PatientView::MENU;
    switchFlag = false;
    updateBalanceText();
    showMenu();

    char buf[100];
    myCopy(buf, "Welcome, ", 100);
    // append name
    int l = myStrLen(buf);
    const char* nm = system.getPatients().get(pIndex).getName();
    for (int i = 0; nm[i] && l < 98; i++) buf[l++] = nm[i];
    buf[l] = '\0';
    headerText.setString(buf);
}

void PatientScreen::updateBalanceText()
{
    char bal[60];
    float b = system.getPatients().get(patientIndex).getBalance();
    char fstr[20]; myFtoa(b, fstr);
    myCopy(bal, "Balance: PKR ", 60);
    int l = myStrLen(bal);
    for (int i = 0; fstr[i]; i++) bal[l++] = fstr[i];
    bal[l] = '\0';
    balanceText.setString(bal);
}

void PatientScreen::clearOutput()
{
    for (int i = 0; i < 30; i++) outputLines[i].setString("");
    outputCount = 0;
}

void PatientScreen::addOutputLine(const char* line)
{
    if (outputCount < 30)
    {
        outputLines[outputCount].setString(line);
        outputCount++;
    }
}

void PatientScreen::showMenu()
{
    currentView = PatientView::MENU;
    clearOutput();
    inputLabel.setString("");
    inputBox.clear();
    // Menu buttons visible — nothing else to do
}

void PatientScreen::showMessage(const char* msg)
{
    clearOutput();
    addOutputLine(msg);
    inputLabel.setString("Press Back to return to menu.");
}

// ─── VIEW HANDLERS ───────────────────────────────────────────
void PatientScreen::doViewAppointments()
{
    currentView = PatientView::VIEW_APPTS;
    clearOutput();
    int pid = system.getPatients().get(patientIndex).getID();
    const Storage<Appointment>& appts = system.getAppointments();
    const Storage<Doctor>& docs = system.getDoctors();

    // Collect & sort by date (simple bubble sort on strings — DD-MM-YYYY sorts by year then month then day)
    int indices[100]; int cnt = 0;
    for (int i = 0; i < appts.size() && cnt < 100; i++)
        if (appts.get(i).getPatientID() == pid) indices[cnt++] = i;

    // bubble sort ascending by date string comparison
    for (int i = 0; i < cnt - 1; i++)
        for (int j = 0; j < cnt - i - 1; j++)
        {
            const char* da = appts.get(indices[j]).getDate();
            const char* db = appts.get(indices[j + 1]).getDate();
            // Compare YYYY then MM then DD (parse from DD-MM-YYYY)
            // da[6..9] year, da[3..4] month, da[0..1] day
            bool swap = false;
            // compare year
            for (int k = 6; k <= 9; k++)
            {
                if (da[k] < db[k]) { swap = false; break; }
                if (da[k] > db[k]) { swap = true;  break; }
            }
            if (swap) { int tmp = indices[j]; indices[j] = indices[j + 1]; indices[j + 1] = tmp; }
        }

    if (cnt == 0) { addOutputLine("No appointments found."); return; }

    addOutputLine("ID  | Doctor Name          | Spec           | Date       | Time  | Status");
    addOutputLine("----+----------------------+----------------+------------+-------+-----------");

    for (int i = 0; i < cnt; i++)
    {
        const Appointment& a = appts.get(indices[i]);
        int di = docs.findByID(a.getDoctorID());
        char dname[50]; myCopy(dname, di >= 0 ? docs.get(di).getName() : "Unknown", 50);
        char spec[50];  myCopy(spec, di >= 0 ? docs.get(di).getSpecialization() : "-", 50);

        char line[200];
        snprintf(line, 200, "%-4d| %-20s | %-14s | %s | %s | %s",
            a.getAppointmentID(), dname, spec, a.getDate(), a.getTime(), a.getStatus());
        addOutputLine(line);
    }
    inputLabel.setString("Press Back to return to menu.");
}

void PatientScreen::doViewRecords()
{
    currentView = PatientView::VIEW_RECORDS;
    clearOutput();
    int pid = system.getPatients().get(patientIndex).getID();
    const Storage<Prescription>& rx = system.getPrescriptions();
    const Storage<Doctor>& docs = system.getDoctors();

    int indices[100]; int cnt = 0;
    for (int i = 0; i < rx.size() && cnt < 100; i++)
        if (rx.get(i).getPatientID() == pid) indices[cnt++] = i;

    if (cnt == 0) { addOutputLine("No medical records found."); inputLabel.setString("Press Back."); return; }

    // sort descending by date
    for (int i = 0; i < cnt - 1; i++)
        for (int j = 0; j < cnt - i - 1; j++)
        {
            const char* da = rx.get(indices[j]).getDate();
            const char* db = rx.get(indices[j + 1]).getDate();
            bool swap = false;
            for (int k = 6; k <= 9; k++)
            {
                if (da[k] > db[k]) { swap = false; break; }
                if (da[k] < db[k]) { swap = true;  break; }
            }
            if (swap) { int tmp = indices[j]; indices[j] = indices[j + 1]; indices[j + 1] = tmp; }
        }

    addOutputLine("Date       | Doctor           | Medicines                    | Notes");
    addOutputLine("-----------+------------------+------------------------------+------------------");
    for (int i = 0; i < cnt; i++)
    {
        const Prescription& p = rx.get(indices[i]);
        int di = docs.findByID(p.getDoctorID());
        char dname[50]; myCopy(dname, di >= 0 ? docs.get(di).getName() : "Unknown", 50);
        char line[250];
        snprintf(line, 250, "%s | %-16s | %-28s | %s",
            p.getDate(), dname, p.getMedicines(), p.getNotes());
        addOutputLine(line);
    }
    inputLabel.setString("Press Back to return to menu.");
}

void PatientScreen::doViewBills()
{
    currentView = PatientView::VIEW_BILLS;
    clearOutput();
    int pid = system.getPatients().get(patientIndex).getID();
    const Storage<Bill>& bills = system.getBills();

    addOutputLine("BillID | ApptID | Amount (PKR) | Status   | Date");
    addOutputLine("-------+--------+--------------+----------+-----------");

    float total = 0;
    bool any = false;
    for (int i = 0; i < bills.size(); i++)
    {
        const Bill& b = bills.get(i);
        if (b.getPatientID() != pid) continue;
        any = true;
        char amt[20]; myFtoa(b.getAmount(), amt);
        char line[150];
        snprintf(line, 150, "%-7d| %-7d| %-12s | %-8s | %s",
            b.getBillID(), b.getAppointmentID(), amt, b.getStatus(), b.getDate());
        addOutputLine(line);
        if (myEq(b.getStatus(), "Unpaid")) total += b.getAmount();
    }
    if (!any) { clearOutput(); addOutputLine("No bills found."); }
    else
    {
        char tline[60]; char tf[20]; myFtoa(total, tf);
        snprintf(tline, 60, "Total outstanding: PKR %s", tf);
        addOutputLine("---");
        addOutputLine(tline);
    }
    inputLabel.setString("Press Back to return to menu.");
}

void PatientScreen::startCancel()
{
    currentView = PatientView::CANCEL;
    pendingCount = 0;
    clearOutput();
    int pid = system.getPatients().get(patientIndex).getID();
    const Storage<Appointment>& appts = system.getAppointments();
    const Storage<Doctor>& docs = system.getDoctors();

    addOutputLine("Scheduled Appointments:");
    addOutputLine("ApptID | Doctor Name          | Date       | Time  ");
    bool any = false;
    for (int i = 0; i < appts.size() && pendingCount < 100; i++)
    {
        const Appointment& a = appts.get(i);
        if (a.getPatientID() == pid && myEq(a.getStatus(), "Scheduled"))
        {
            any = true;
            pendingIDs[pendingCount++] = a.getAppointmentID();
            int di = docs.findByID(a.getDoctorID());
            char dname[50]; myCopy(dname, di >= 0 ? docs.get(di).getName() : "Unknown", 50);
            char line[150];
            snprintf(line, 150, "%-7d| %-20s | %s | %s",
                a.getAppointmentID(), dname, a.getDate(), a.getTime());
            addOutputLine(line);
        }
    }
    if (!any)
    {
        clearOutput();
        addOutputLine("You have no scheduled appointments.");
        currentView = PatientView::MENU;
        inputLabel.setString("Press Back to return to menu.");
        return;
    }
    inputLabel.setString("Enter Appointment ID to cancel:");
    inputBox.clear(); inputBox.setSelected(true);
}

void PatientScreen::startPayBill()
{
    currentView = PatientView::PAY_BILL;
    pendingCount = 0;
    clearOutput();
    int pid = system.getPatients().get(patientIndex).getID();
    const Storage<Bill>& bills = system.getBills();

    addOutputLine("Unpaid Bills:");
    addOutputLine("BillID | ApptID | Amount (PKR) | Date");
    bool any = false;
    for (int i = 0; i < bills.size() && pendingCount < 100; i++)
    {
        const Bill& b = bills.get(i);
        if (b.getPatientID() == pid && myEq(b.getStatus(), "Unpaid"))
        {
            any = true;
            pendingIDs[pendingCount++] = b.getBillID();
            char amt[20]; myFtoa(b.getAmount(), amt);
            char line[120];
            snprintf(line, 120, "%-7d| %-7d| %-12s | %s",
                b.getBillID(), b.getAppointmentID(), amt, b.getDate());
            addOutputLine(line);
        }
    }
    if (!any)
    {
        clearOutput();
        addOutputLine("No Unpaid bills.");
        currentView = PatientView::MENU;
        inputLabel.setString("Press Back to return to menu.");
        return;
    }
    inputLabel.setString("Enter Bill ID to pay:");
    inputBox.clear(); inputBox.setSelected(true);
}

// ─── BOOKING FLOW ────────────────────────────────────────────
void PatientScreen::handleBookSpec(const char* input)
{
    if (myStrLen(input) == 0) { showMessage("Please enter a specialization."); return; }
    myCopy(bookSpec, input, 60);

    // Search doctors (case-insensitive)
    const Storage<Doctor>& docs = system.getDoctors();
    bookDocCount = 0;
    clearOutput();
    addOutputLine("ID  | Name                 | Specialization   | Fee");
    addOutputLine("----+----------------------+------------------+------");

    for (int i = 0; i < docs.size() && bookDocCount < 20; i++)
    {
        const char* sp = docs.get(i).getSpecialization();
        // case-insensitive compare
        int j = 0; bool match = true;
        while (bookSpec[j] || sp[j])
        {
            char ca = bookSpec[j]; char cb = sp[j];
            if (ca >= 'A' && ca <= 'Z') ca += 32;
            if (cb >= 'A' && cb <= 'Z') cb += 32;
            if (ca != cb) { match = false; break; }
            j++;
        }
        if (match)
        {
            bookDocResults[bookDocCount++] = docs.get(i).getID();
            char fee[20]; myFtoa(docs.get(i).getFee(), fee);
            char line[150];
            snprintf(line, 150, "%-4d| %-20s | %-16s | %s",
                docs.get(i).getID(), docs.get(i).getName(), sp, fee);
            addOutputLine(line);
        }
    }

    if (bookDocCount == 0)
    {
        showMessage("No doctors available for that specialization.");
        currentView = PatientView::MENU;
        return;
    }

    currentView = PatientView::BOOK_DOC;
    inputLabel.setString("Enter Doctor ID:");
    inputBox.clear(); inputBox.setSelected(true);
}

void PatientScreen::handleBookDoc(const char* input)
{
    bool ok; int id = myAtoi(input, ok);
    if (!ok || id <= 0) { showMessage("Invalid Doctor ID."); currentView = PatientView::MENU; return; }

    const Storage<Doctor>& docs = system.getDoctors();
    int di = docs.findByID(id);
    if (di < 0) { showMessage("Doctor not found."); currentView = PatientView::MENU; return; }

    // Verify it was in our results
    bool found = false;
    for (int i = 0; i < bookDocCount; i++)
        if (bookDocResults[i] == id) { found = true; break; }
    if (!found) { showMessage("Doctor not in listed results."); currentView = PatientView::MENU; return; }

    bookDocID = id;
    currentView = PatientView::BOOK_DATE;
    clearOutput();
    addOutputLine("Date format: DD-MM-YYYY");
    addOutputLine("Example: 25-06-2026");
    inputLabel.setString("Enter Date (DD-MM-YYYY):");
    inputBox.clear(); inputBox.setSelected(true);
}

void PatientScreen::handleBookDate(const char* input)
{
    if (!Validator::isValidDate(input))
    {
        showMessage("Invalid date. Use DD-MM-YYYY, current year or later.");
        inputLabel.setString("Enter Date (DD-MM-YYYY):");
        inputBox.clear();
        return;
    }
    myCopy(bookDate, input, 20);

    // Show available slots
    const char* allSlots[] = { "09:00","10:00","11:00","12:00","13:00","14:00","15:00","16:00" };
    const Storage<Appointment>& appts = system.getAppointments();

    availSlotCount = 0;
    clearOutput();
    addOutputLine("Available time slots:");

    for (int s = 0; s < 8; s++)
    {
        bool taken = false;
        for (int i = 0; i < appts.size(); i++)
        {
            const Appointment& a = appts.get(i);
            if (a.getDoctorID() == bookDocID
                && myEq(a.getDate(), bookDate)
                && myEq(a.getTime(), allSlots[s])
                && !myEq(a.getStatus(), "Cancelled"))
            {
                taken = true; break;
            }
        }
        if (!taken)
        {
            myCopy(availSlots[availSlotCount], allSlots[s], 10);
            availSlotCount++;
            addOutputLine(allSlots[s]);
        }
    }

    if (availSlotCount == 0)
    {
        showMessage("No slots available on this date. Press Back.");
        currentView = PatientView::MENU;
        return;
    }

    currentView = PatientView::BOOK_SLOT;
    inputLabel.setString("Enter time slot (e.g. 09:00):");
    inputBox.clear(); inputBox.setSelected(true);
}

void PatientScreen::handleBookSlot(const char* input)
{
    
        // Validate input is one of available slots
        bool valid = false;
        for (int i = 0; i < availSlotCount; i++)
            if (myEq(availSlots[i], input)) { valid = true; break; }
        if (!valid)
        {
            showMessage("Slot unavailable or invalid. Press Back to retry.");
            currentView = PatientView::MENU;
            return;
        }
        // Book it
        try
        {
            bool booked = system.bookAppointment(patientIndex, bookDocID, bookDate, input);
            if (booked)
            {
                updateBalanceText();
                const Storage<Appointment>& appts = system.getAppointments();
                int newID = appts.get(appts.size() - 1).getAppointmentID();
                char msg[80];
                snprintf(msg, 80, "Appointment booked! ID: %d", newID);
                showMessage(msg);
            }
        }
        catch (InsufficientFundsException&)
        {
            showMessage("Insufficient balance for this appointment.");
        }
        catch (SlotUnavailableException&)
        {
            showMessage("That slot is already taken.");
        }
        catch (...)
        {
            showMessage("Booking failed.");
        }
        currentView = PatientView::MENU;
        inputLabel.setString("Press Back to return to menu.");
    
}

void PatientScreen::handleCancel(const char* input)
{
    bool ok; int id = myAtoi(input, ok);
    if (!ok || id <= 0) { showMessage("Invalid ID."); currentView = PatientView::MENU; return; }

    int pid = system.getPatients().get(patientIndex).getID();
    bool cancelled = system.cancelAppointment(id, pid);
    if (cancelled)
    {
        updateBalanceText();
        showMessage("Appointment Cancelled. Fee refunded to balance.");
    }
    else
    {
        showMessage("Invalid appointment ID or not Scheduled.");
    }
    currentView = PatientView::MENU;
    inputLabel.setString("Press Back to return to menu.");
}

void PatientScreen::handlePayBill(const char* input)
{
    bool ok; int id = myAtoi(input, ok);
    if (!ok || id <= 0) { showMessage("Invalid Bill ID."); currentView = PatientView::MENU; return; }

    int pid = system.getPatients().get(patientIndex).getID();
    bool paid = system.payBill(id, pid);
    if (paid)
    {
        updateBalanceText();
        char buf[80]; char bal[20];
        myFtoa(system.getPatients().get(patientIndex).getBalance(), bal);
        snprintf(buf, 80, "Bill paid! Remaining balance: PKR %s", bal);
        showMessage(buf);
    }
    else
    {
        showMessage("Payment failed. Check balance or bill ID.");
    }
    currentView = PatientView::MENU;
    inputLabel.setString("Press Back to return to menu.");
}

void PatientScreen::handleTopUp(const char* input)
{
    bool ok; float amt = myAtof(input, ok);
    if (!ok || amt <= 0)
    {
        showMessage("Invalid amount. Enter a positive number.");
        inputLabel.setString("Enter amount (PKR) to top up:");
        inputBox.clear();
        return;
    }
    bool done = system.topUpBalance(patientIndex, amt);
    if (done)
    {
        updateBalanceText();
        char buf[80]; char bal[20];
        myFtoa(system.getPatients().get(patientIndex).getBalance(), bal);
        snprintf(buf, 80, "Balance updated. New balance: PKR %s", bal);
        showMessage(buf);
    }
    else
    {
        showMessage("Top-up failed.");
    }
    currentView = PatientView::MENU;
    inputLabel.setString("Press Back to return to menu.");
}

// ─── EVENT HANDLER ───────────────────────────────────────────
void PatientScreen::handleEvent(sf::Event& event, sf::RenderWindow& window)
{
    sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    if (event.type == sf::Event::MouseButtonPressed
        && event.mouseButton.button == sf::Mouse::Left)
        inputBox.setSelected(inputBox.contains(mouse));

    // In MENU view — menu button clicks
    if (currentView == PatientView::MENU)
    {
        for (int i = 0; i < 7; i++)
        {
            if (menuBtns[i].isClicked(event, mouse))
            {
                if (i == 0) {
                    currentView = PatientView::BOOK_SPEC;
                    clearOutput(); addOutputLine("Search doctors by specialization.");
                    inputLabel.setString("Enter specialization (e.g. Cardiology):");
                    inputBox.clear(); inputBox.setSelected(true);
                }
                else if (i == 1) startCancel();
                else if (i == 2) doViewAppointments();
                else if (i == 3) doViewRecords();
                else if (i == 4) doViewBills();
                else if (i == 5) startPayBill();
                else if (i == 6) {
                    currentView = PatientView::TOPUP;
                    clearOutput();
                    inputLabel.setString("Enter amount (PKR) to top up:");
                    inputBox.clear(); inputBox.setSelected(true);
                }
                return;
            }
        }
    }

    // Submit button or Enter
    bool submitted = actionBtn.isClicked(event, mouse)
        || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return);

    if (submitted)
    {
        const char* inp = inputBox.getText();
        if (currentView == PatientView::BOOK_SPEC) handleBookSpec(inp);
        else if (currentView == PatientView::BOOK_DOC)  handleBookDoc(inp);
        else if (currentView == PatientView::BOOK_DATE) handleBookDate(inp);
        else if (currentView == PatientView::BOOK_SLOT) handleBookSlot(inp);
        else if (currentView == PatientView::CANCEL)    handleCancel(inp);
        else if (currentView == PatientView::PAY_BILL)  handlePayBill(inp);
        else if (currentView == PatientView::TOPUP)     handleTopUp(inp);
    }

    if (backBtn.isClicked(event, mouse)) showMenu();

    if (logoutBtn.isClicked(event, mouse)) switchFlag = true;

    inputBox.handleInput(event);
}

void PatientScreen::update() {}

void PatientScreen::render(sf::RenderWindow& window)
{
    window.draw(headerBar);
    window.draw(headerText);
    window.draw(balanceText);

    if (currentView == PatientView::MENU)
    {
        for (int i = 0; i < 7; i++) menuBtns[i].draw(window);
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

ScreenType PatientScreen::getNextScreen() const { return ScreenType::LOGIN; }
bool       PatientScreen::shouldSwitch()  const { return switchFlag; }
void       PatientScreen::resetSwitch() { switchFlag = false; }
PatientScreen::~PatientScreen()
{
    delete[] outputLines;
}