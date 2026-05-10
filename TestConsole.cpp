// ================================================================
// TestConsole.cpp  —  Step 9: Console test for all backend logic
// 
// HOW TO USE:
//   Temporarily replace main() with testMain() by commenting out
//   the real main and calling testMain() instead.
//   Or compile separately: g++ TestConsole.cpp ... -o test
//
// All tests print PASS or FAIL clearly.
// ================================================================

#include <iostream>
#include "HospitalSystem.h"
#include "HospitalException.h"
#include "InvalidInputException.h"
#include "InsufficientFundsException.h"
#include "SlotUnavailableException.h"
#include "Validator.h"

// ---------------------------------------------------------------
// Tiny test helpers
// ---------------------------------------------------------------

static int passed = 0;
static int failed = 0;

static void check(const char* testName, bool condition)
{
    if (condition)
    {
        std::cout << "  [PASS] " << testName << std::endl;
        passed++;
    }
    else
    {
        std::cout << "  [FAIL] " << testName << std::endl;
        failed++;
    }
}

static void section(const char* title)
{
    std::cout << std::endl;
    std::cout << "=== " << title << " ===" << std::endl;
}

// ---------------------------------------------------------------
// Tests
// ---------------------------------------------------------------

static void testValidator()
{
    section("Validator");

    check("isValidID: 1 is valid", Validator::isValidID(1));
    check("isValidID: 0 is invalid", !Validator::isValidID(0));
    check("isValidID: -5 is invalid", !Validator::isValidID(-5));

    check("isValidAge: 25 is valid", Validator::isValidAge(25));
    check("isValidAge: 0 is invalid", !Validator::isValidAge(0));
    check("isValidAge: 151 is invalid", !Validator::isValidAge(151));

    check("isValidAmount: 100 is valid", Validator::isValidAmount(100.0f));
    check("isValidAmount: -1 is invalid", !Validator::isValidAmount(-1.0f));

    check("isValidPositiveAmount: 50 valid", Validator::isValidPositiveAmount(50.0f));
    check("isValidPositiveAmount: 0 invalid", !Validator::isValidPositiveAmount(0.0f));

    check("isValidGender: M valid", Validator::isValidGender('M'));
    check("isValidGender: F valid", Validator::isValidGender('F'));
    check("isValidGender: X invalid", !Validator::isValidGender('X'));

    check("isValidDate: 15-06-2025 valid", Validator::isValidDate("15-06-2025"));
    check("isValidDate: bad format invalid", !Validator::isValidDate("2025/06/15"));
    check("isValidDate: month 13 invalid", !Validator::isValidDate("01-13-2025"));

    check("isValidTime: 09:00 valid", Validator::isValidTime("09:00"));
    check("isValidTime: 23:59 valid", Validator::isValidTime("23:59"));
    check("isValidTime: 25:00 invalid", !Validator::isValidTime("25:00"));
    check("isValidTime: bad format invalid", !Validator::isValidTime("9:00"));

    check("isValidPassword: abc123 valid", Validator::isValidPassword("abc123"));
    check("isValidPassword: short invalid", !Validator::isValidPassword("abc"));
    check("isValidPassword: space invalid", !Validator::isValidPassword("abc 12"));

    check("isValidMenuChoice: 2 in [1,4]", Validator::isValidMenuChoice(2, 1, 4));
    check("isValidMenuChoice: 5 out [1,4]", !Validator::isValidMenuChoice(5, 1, 4));
}

static void testStorage()
{
    section("Storage<T>");

    Storage<Bill> store;

    Bill b1(1, 10, 100, 250.0f, "Unpaid", "01-01-2025");
    Bill b2(2, 11, 101, 150.0f, "Paid", "02-01-2025");
    Bill b3(3, 12, 102, 300.0f, "Unpaid", "03-01-2025");

    store.add(b1);
    store.add(b2);
    store.add(b3);

    check("size() == 3", store.size() == 3);
    check("findByID(2) == 1", store.findByID(2) == 1);
    check("findByID(99) == -1", store.findByID(99) == -1);
    check("removeByID(2) returns true", store.removeByID(2));
    check("size() == 2 after remove", store.size() == 2);
    check("findByID(2) == -1 after remove", store.findByID(2) == -1);
    check("removeByID(99) returns false", !store.removeByID(99));
}

static void testLoginAndLoad(HospitalSystem& hospital)
{
    section("loadSystem + Login");

    check("patients loaded > 0", hospital.getPatients().size() > 0);
    check("doctors loaded > 0", hospital.getDoctors().size() > 0);
    check("admins loaded > 0", hospital.getAdmins().size() > 0);

    // Valid patient login — use first patient in storage
    int failCount = 0;
    int patID = hospital.getPatients().get(0).getID();
    const char* patPass = hospital.getPatients().get(0).getPassword();

    int idx = hospital.loginPatient(patID, patPass, failCount);
    check("valid patient login returns index >= 0", idx >= 0);
    check("failCount reset to 0 on success", failCount == 0);

    // Wrong password
    int fc2 = 0;
    int idx2 = hospital.loginPatient(patID, "wrongpass", fc2);
    check("wrong password returns -1", idx2 == -1);
    check("failCount incremented to 1", fc2 == 1);

    // Lock after 3 failures
    int fc3 = 0;
    hospital.loginPatient(patID, "x", fc3);
    hospital.loginPatient(patID, "x", fc3);
    hospital.loginPatient(patID, "x", fc3);
    check("failCount reaches 3 after lockout", fc3 == 3);
}

static void testAppointmentBooking(HospitalSystem& hospital)
{
    section("Appointment Booking");

    // Need a patient with enough balance and a valid doctor
    if (hospital.getPatients().size() == 0 || hospital.getDoctors().size() == 0)
    {
        std::cout << "  [SKIP] No patients or doctors in test data." << std::endl;
        return;
    }

    int patientIndex = 0;
    int doctorID = hospital.getDoctors().get(0).getID();
    float fee = hospital.getDoctors().get(0).getFee();

    // Top up balance to ensure patient can afford it
    try { hospital.topUpBalance(patientIndex, fee + 500.0f); }
    catch (...) {}

    int apptsBefore = hospital.getAppointments().size();
    int billsBefore = hospital.getBills().size();

    // Book a fresh slot
    bool booked = false;
    try
    {
        booked = hospital.bookAppointment(patientIndex, doctorID,
            "25-12-2025", "10:00");
    }
    catch (HospitalException& e)
    {
        std::cout << "  [INFO] bookAppointment threw: " << e.what() << std::endl;
    }

    check("appointment count increased by 1",
        hospital.getAppointments().size() == apptsBefore + 1);
    check("bill count increased by 1",
        hospital.getBills().size() == billsBefore + 1);

    // Book same slot again — should throw SlotUnavailableException
    bool slotBlocked = false;
    try
    {
        hospital.bookAppointment(patientIndex, doctorID, "25-12-2025", "10:00");
    }
    catch (SlotUnavailableException&)
    {
        slotBlocked = true;
    }
    catch (HospitalException&) {}

    check("duplicate slot throws SlotUnavailableException", slotBlocked);

    // Test insufficient funds
    // Set balance to 0 by finding a patient with 0 balance — or just test the exception path
    bool insufficientThrown = false;
    try
    {
        // Force insufficient funds: top up 0 is invalid, so temporarily test with a huge fee
        // Instead we test the exception by passing a negative amount to topUp
        hospital.topUpBalance(patientIndex, -1.0f);
    }
    catch (InvalidInputException&)
    {
        insufficientThrown = true;
    }
    check("topUpBalance(-1) throws InvalidInputException", insufficientThrown);

    // Cancel the appointment we just booked
    int newApptID = hospital.getAppointments().get(
        hospital.getAppointments().size() - 1).getID();

    int patientID = hospital.getPatients().get(patientIndex).getID();

    bool cancelled = hospital.cancelAppointment(newApptID, patientID);
    check("cancelAppointment returns true", cancelled);

    // Cancel again — should fail (already Cancelled)
    bool doubleCancelled = hospital.cancelAppointment(newApptID, patientID);
    check("double-cancel returns false", !doubleCancelled);
}

static void testBilling(HospitalSystem& hospital)
{
    section("Billing");

    // Find an unpaid bill
    int billID = -1;
    int patientID = -1;

    for (int i = 0; i < hospital.getBills().size(); i++)
    {
        const Bill& b = hospital.getBills().get(i);

        bool isUnpaid = true;
        const char* s = b.getStatus();
        const char* u = "Unpaid";
        int k = 0;
        while (s[k] != '\0' && u[k] != '\0' && s[k] == u[k]) k++;
        isUnpaid = (s[k] == '\0' && u[k] == '\0');

        if (isUnpaid)
        {
            billID = b.getID();
            patientID = b.getPatientID();
            break;
        }
    }

    if (billID == -1)
    {
        std::cout << "  [SKIP] No unpaid bills in test data." << std::endl;
        return;
    }

    // Find patient index and top up enough to pay
    int patIndex = hospital.getPatients().findByID(patientID);
    float amount = 0;

    for (int i = 0; i < hospital.getBills().size(); i++)
    {
        if (hospital.getBills().get(i).getID() == billID)
        {
            amount = hospital.getBills().get(i).getAmount();
            break;
        }
    }

    try { hospital.topUpBalance(patIndex, amount + 100.0f); }
    catch (...) {}

    bool paid = false;
    try { paid = hospital.payBill(billID, patientID); }
    catch (HospitalException& e)
    {
        std::cout << "  [INFO] payBill threw: " << e.what() << std::endl;
    }

    check("payBill returns true", paid);

    // Pay again — should fail (already Paid)
    bool doublePay = true;
    try { doublePay = hospital.payBill(billID, patientID); }
    catch (...) { doublePay = false; }

    check("double-pay returns false", !doublePay);

    // Wrong patient trying to pay someone else's bill
    bool wrongOwner = false;
    // find any other patient ID
    for (int i = 0; i < hospital.getPatients().size(); i++)
    {
        int othID = hospital.getPatients().get(i).getID();
        if (othID != patientID)
        {
            wrongOwner = !hospital.payBill(billID, othID);
            break;
        }
    }
    check("wrong-owner pay returns false", wrongOwner);
}

static void testAdminFeatures(HospitalSystem& hospital)
{
    section("Admin — addDoctor / removeDoctor");

    int doctorsBefore = hospital.getDoctors().size();

    bool added = false;
    try
    {
        added = hospital.addDoctor("Test Doctor", "Neurology",
            "03001234567", "testpass1", 500.0f);
    }
    catch (HospitalException& e)
    {
        std::cout << "  [INFO] addDoctor threw: " << e.what() << std::endl;
    }

    check("addDoctor increases doctor count",
        hospital.getDoctors().size() == doctorsBefore + 1);

    // Get the new doctor's ID
    int newDocID = hospital.getDoctors().get(
        hospital.getDoctors().size() - 1).getID();

    bool removed = hospital.removeDoctor(newDocID);
    check("removeDoctor returns true for new doctor with no appointments", removed);

    check("doctor count back to original",
        hospital.getDoctors().size() == doctorsBefore);

    // Remove non-existent doctor
    bool badRemove = hospital.removeDoctor(99999);
    check("removeDoctor(invalid ID) returns false", !badRemove);
}

static void testDoctorFeatures(HospitalSystem& hospital)
{
    section("Doctor — mark complete, mark no-show");

    // Find a scheduled appointment
    int apptID = -1;
    int doctorID = -1;

    for (int i = 0; i < hospital.getAppointments().size(); i++)
    {
        const Appointment& a = hospital.getAppointments().get(i);
        const char* s = a.getStatus();
        const char* t = "Scheduled";
        int k = 0;
        while (s[k] != '\0' && t[k] != '\0' && s[k] == t[k]) k++;

        if (s[k] == '\0' && t[k] == '\0')
        {
            apptID = a.getID();
            doctorID = a.getDoctorID();
            break;
        }
    }

    if (apptID == -1)
    {
        std::cout << "  [SKIP] No scheduled appointments in test data." << std::endl;
        return;
    }

    bool completed = hospital.markAppointmentComplete(apptID, doctorID);
    check("markAppointmentComplete returns true", completed);

    // Try completing again — must fail
    bool doubleComplete = hospital.markAppointmentComplete(apptID, doctorID);
    check("double-complete returns false", !doubleComplete);

    // Wrong doctor
    bool wrongDoc = hospital.markAppointmentComplete(apptID, doctorID + 999);
    check("wrong-doctor mark returns false", !wrongDoc);
}

// ---------------------------------------------------------------
// Entry point for test run
// ---------------------------------------------------------------

int testMain()
{
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "  MediCore — Backend Test Suite (Step 9)" << std::endl;
    std::cout << "========================================" << std::endl;

    // Tests that need no file data
    testValidator();
    testStorage();

    // Tests that need real data from files
    HospitalSystem hospital;

    try
    {
        hospital.loadSystem();
    }
    catch (HospitalException& e)
    {
        std::cout << std::endl;
        std::cout << "[ERROR] Could not load system: " << e.what() << std::endl;
        std::cout << "Make sure DataFiles/ folder exists with .txt files." << std::endl;
        std::cout << std::endl;
        std::cout << "Validator + Storage tests above ran independently." << std::endl;
        return 1;
    }

    testLoginAndLoad(hospital);
    testAppointmentBooking(hospital);
    testBilling(hospital);
    testDoctorFeatures(hospital);
    testAdminFeatures(hospital);

    // Final summary
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "  Results: " << passed << " passed, "
        << failed << " failed" << std::endl;
    std::cout << "========================================" << std::endl;

    return (failed == 0) ? 0 : 1;
}