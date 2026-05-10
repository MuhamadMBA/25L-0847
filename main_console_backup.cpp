//#include <iostream>
//#include "HospitalSystem.h"
//#include "HospitalException.h"
//#include "InvalidInputException.h"
//#include "InsufficientFundsException.h"
//#include "SlotUnavailableException.h"
//#include "Validator.h"
//
//// ================================================================
////  UTILITY
//// ================================================================
//
//static void clearInput()
//{
//    std::cin.clear();
//    std::cin.ignore(1000, '\n');
//}
//
//static void printLine()
//{
//    std::cout << "================================" << std::endl;
//}
//
//// ================================================================
////  MAIN MENU
//// ================================================================
//
//static void showMainMenu()
//{
//    printLine();
//    std::cout << "   MediCore Hospital System" << std::endl;
//    printLine();
//    std::cout << "  1. Login as Patient" << std::endl;
//    std::cout << "  2. Login as Doctor" << std::endl;
//    std::cout << "  3. Login as Admin" << std::endl;
//    std::cout << "  4. Exit" << std::endl;
//    printLine();
//    std::cout << "  Choice: ";
//}
//
//// ================================================================
////  LOGIN HELPERS
////  Each returns the storage index on success, -1 if locked out.
//// ================================================================
//
//static int handleLogin(HospitalSystem& hospital, const char* role)
//{
//    int failCount = 0;
//
//    while (failCount < 3)
//    {
//        int  id;
//        char password[50];
//
//        std::cout << std::endl;
//        std::cout << "--- " << role << " Login ---" << std::endl;
//        std::cout << "  ID       : "; std::cin >> id;
//        std::cout << "  Password : "; std::cin >> password;
//
//        int index = -1;
//
//        // Call the correct login function based on role string
//        if (role[0] == 'P')      index = hospital.loginPatient(id, password, failCount);
//        else if (role[0] == 'D') index = hospital.loginDoctor(id, password, failCount);
//        else if (role[0] == 'A') index = hospital.loginAdmin(id, password, failCount);
//
//        if (index >= 0)
//        {
//            std::cout << "  Login successful." << std::endl;
//            return index;
//        }
//
//        int remaining = 3 - failCount;
//
//        if (remaining > 0)
//        {
//            std::cout << "  Wrong ID or password. "
//                << remaining << " attempt(s) left." << std::endl;
//        }
//    }
//
//    std::cout << std::endl;
//    std::cout << "  Session locked after 3 failed attempts." << std::endl;
//    std::cout << "  This event has been logged." << std::endl;
//
//    return -1;
//}
//
//// ================================================================
////  PATIENT SESSION
//// ================================================================
//
//static void showPatientMenu(const char* name, float balance)
//{
//    printLine();
//    std::cout << "  Patient: " << name
//        << "  |  Balance: " << balance << std::endl;
//    printLine();
//    std::cout << "  1. Book Appointment" << std::endl;
//    std::cout << "  2. Cancel Appointment" << std::endl;
//    std::cout << "  3. View My Appointments" << std::endl;
//    std::cout << "  4. View My Bills" << std::endl;
//    std::cout << "  5. View My Prescriptions" << std::endl;
//    std::cout << "  6. Top Up Balance" << std::endl;
//    std::cout << "  7. Pay Bill" << std::endl;
//    std::cout << "  8. Logout" << std::endl;
//    printLine();
//    std::cout << "  Choice: ";
//}
//
//// Ask user to pick a doctor (shows list first, then asks for ID)
//static int pickDoctor(HospitalSystem& hospital)
//{
//    char spec[50];
//
//    std::cout << std::endl;
//    std::cout << "  Search by specialization (or press Enter to see all): ";
//    clearInput();
//    std::cin.getline(spec, 50);
//
//    std::cout << std::endl;
//
//    if (spec[0] == '\0')
//    {
//        // Show all doctors
//        const Storage<Doctor>& docs = hospital.getDoctors();
//
//        if (docs.size() == 0)
//        {
//            std::cout << "  No doctors registered." << std::endl;
//            return -1;
//        }
//
//        for (int i = 0; i < docs.size(); i++)
//        {
//            std::cout << docs.get(i);
//            std::cout << "----------------------------" << std::endl;
//        }
//    }
//    else
//    {
//        hospital.searchDoctorsBySpecialization(spec);
//    }
//
//    int doctorID;
//    std::cout << "  Enter Doctor ID: ";
//    std::cin >> doctorID;
//
//    return doctorID;
//}
//
//static void handleBookAppointment(HospitalSystem& hospital, int patientIndex)
//{
//    int doctorID = pickDoctor(hospital);
//
//    if (doctorID <= 0)
//    {
//        std::cout << "  Invalid doctor selection." << std::endl;
//        return;
//    }
//
//    char date[20];
//    char time[20];
//
//    std::cout << "  Date (DD-MM-YYYY): "; std::cin >> date;
//    std::cout << "  Time (HH:MM)     : "; std::cin >> time;
//
//    if (!Validator::isValidDate(date))
//    {
//        std::cout << "  Invalid date format. Use DD-MM-YYYY." << std::endl;
//        return;
//    }
//
//    if (!Validator::isValidTime(time))
//    {
//        std::cout << "  Invalid time format. Use HH:MM (e.g. 09:00)." << std::endl;
//        return;
//    }
//
//    try
//    {
//        bool ok = hospital.bookAppointment(patientIndex, doctorID, date, time);
//
//        if (ok)
//        {
//            std::cout << std::endl;
//            std::cout << "  Appointment booked successfully." << std::endl;
//            std::cout << "  Fee has been deducted from your balance." << std::endl;
//            std::cout << "  A bill has been created." << std::endl;
//        }
//    }
//    catch (InsufficientFundsException& e)
//    {
//        std::cout << "  Error: " << e.what()
//            << " — please top up your balance first." << std::endl;
//    }
//    catch (SlotUnavailableException& e)
//    {
//        std::cout << "  Error: " << e.what()
//            << " — choose a different date or time." << std::endl;
//    }
//    catch (InvalidInputException& e)
//    {
//        std::cout << "  Error: " << e.what() << std::endl;
//    }
//    catch (HospitalException& e)
//    {
//        std::cout << "  System error: " << e.what() << std::endl;
//    }
//}
//
//static void handleCancelAppointment(HospitalSystem& hospital, int patientID)
//{
//    std::cout << "  Enter Appointment ID to cancel: ";
//    int apptID;
//    std::cin >> apptID;
//
//    bool ok = hospital.cancelAppointment(apptID, patientID);
//
//    if (ok)
//    {
//        std::cout << "  Appointment cancelled successfully." << std::endl;
//    }
//    else
//    {
//        std::cout << "  Could not cancel. Check the ID and that the"
//            " appointment is still Scheduled." << std::endl;
//    }
//}
//
//static void runPatientSession(HospitalSystem& hospital, int patientIndex)
//{
//    int patientID = hospital.getPatients().get(patientIndex).getID();
//    const char* name = hospital.getPatients().get(patientIndex).getName();
//
//    int choice;
//
//    do
//    {
//        float balance = hospital.getPatients().get(patientIndex).getBalance();
//        showPatientMenu(name, balance);
//        std::cin >> choice;
//
//        std::cout << std::endl;
//
//        switch (choice)
//        {
//        case 1:
//            handleBookAppointment(hospital, patientIndex);
//            break;
//
//        case 2:
//            handleCancelAppointment(hospital, patientID);
//            break;
//
//        case 3:
//            std::cout << "--- Your Appointments ---" << std::endl;
//            hospital.viewPatientAppointments(patientID);
//            break;
//
//        case 4:
//            std::cout << "--- Your Bills ---" << std::endl;
//            hospital.viewPatientBills(patientID);
//            break;
//
//        case 5:
//            std::cout << "--- Your Prescriptions ---" << std::endl;
//            hospital.viewPatientPrescriptions(patientID);
//            break;
//
//        case 6:
//        {
//            float amount;
//            std::cout << "  Enter amount to top up: ";
//            std::cin >> amount;
//
//            try
//            {
//                hospital.topUpBalance(patientIndex, amount);
//                std::cout << "  Balance updated. New balance: "
//                    << hospital.getPatients().get(patientIndex).getBalance()
//                    << std::endl;
//            }
//            catch (InvalidInputException& e)
//            {
//                std::cout << "  Error: " << e.what() << std::endl;
//            }
//            break;
//        }
//
//        case 7:
//        {
//            std::cout << "--- Your Unpaid Bills ---" << std::endl;
//            hospital.viewPatientBills(patientID);
//
//            int billID;
//            std::cout << "  Enter Bill ID to pay (0 to cancel): ";
//            std::cin >> billID;
//
//            if (billID <= 0) break;
//
//            try
//            {
//                bool ok = hospital.payBill(billID, patientID);
//
//                if (ok)
//                    std::cout << "  Bill paid successfully." << std::endl;
//                else
//                    std::cout << "  Bill not found or already paid." << std::endl;
//            }
//            catch (InsufficientFundsException& e)
//            {
//                std::cout << "  Error: " << e.what()
//                    << " — top up your balance first." << std::endl;
//            }
//            break;
//        }
//
//        case 8:
//            std::cout << "  Logged out." << std::endl;
//            break;
//
//        default:
//            std::cout << "  Invalid choice." << std::endl;
//        }
//
//    } while (choice != 8);
//}
//
//// ================================================================
////  DOCTOR SESSION  (Step 7)
//// ================================================================
//
//static void showDoctorMenu(const char* name)
//{
//    printLine();
//    std::cout << "  Dr. " << name << std::endl;
//    printLine();
//    std::cout << "  1. View Today's Appointments" << std::endl;
//    std::cout << "  2. Mark Appointment Complete" << std::endl;
//    std::cout << "  3. Mark No-Show" << std::endl;
//    std::cout << "  4. Write Prescription" << std::endl;
//    std::cout << "  5. View Patient History" << std::endl;
//    std::cout << "  6. Logout" << std::endl;
//    printLine();
//    std::cout << "  Choice: ";
//}
//
//static void handleWritePrescription(HospitalSystem& hospital, int doctorID)
//{
//    int apptID;
//    char medicines[200];
//    char notes[200];
//    char date[20];
//
//    std::cout << "  Appointment ID  : "; std::cin >> apptID;
//    clearInput();
//    std::cout << "  Medicines       : "; std::cin.getline(medicines, 200);
//    std::cout << "  Notes           : "; std::cin.getline(notes, 200);
//    std::cout << "  Date (DD-MM-YYYY): "; std::cin >> date;
//
//    try
//    {
//        bool ok = hospital.writePrescription(apptID, doctorID,
//            medicines, notes, date);
//        if (ok)
//            std::cout << "  Prescription written successfully." << std::endl;
//        else
//            std::cout << "  Failed. Check appointment ID, ownership,"
//            " status (must be Completed),"
//            " and that no prescription exists yet." << std::endl;
//    }
//    catch (InvalidInputException& e)
//    {
//        std::cout << "  Error: " << e.what() << std::endl;
//    }
//}
//
//static void runDoctorSession(HospitalSystem& hospital, int doctorIndex)
//{
//    int         doctorID = hospital.getDoctors().get(doctorIndex).getID();
//    const char* name = hospital.getDoctors().get(doctorIndex).getName();
//
//    int choice;
//
//    do
//    {
//        showDoctorMenu(name);
//        std::cin >> choice;
//        std::cout << std::endl;
//
//        switch (choice)
//        {
//        case 1:
//        {
//            char today[20];
//            std::cout << "  Enter today's date (DD-MM-YYYY): ";
//            std::cin >> today;
//            std::cout << std::endl;
//            hospital.viewTodayAppointments(doctorID, today);
//            break;
//        }
//
//        case 2:
//        {
//            int apptID;
//            std::cout << "  Appointment ID to mark Complete: ";
//            std::cin >> apptID;
//
//            bool ok = hospital.markAppointmentComplete(apptID, doctorID);
//            std::cout << (ok ? "  Marked as Completed." :
//                "  Failed. Check ID, ownership, and status.") << std::endl;
//            break;
//        }
//
//        case 3:
//        {
//            int apptID;
//            std::cout << "  Appointment ID to mark No-Show: ";
//            std::cin >> apptID;
//
//            bool ok = hospital.markNoShow(apptID, doctorID);
//            std::cout << (ok ? "  Marked as No-Show." :
//                "  Failed. Check ID, ownership, and status.") << std::endl;
//            break;
//        }
//
//        case 4:
//            handleWritePrescription(hospital, doctorID);
//            break;
//
//        case 5:
//        {
//            int patientID;
//            std::cout << "  Enter Patient ID: ";
//            std::cin >> patientID;
//            std::cout << std::endl;
//            hospital.viewPatientHistory(patientID);
//            break;
//        }
//
//        case 6:
//            std::cout << "  Logged out." << std::endl;
//            break;
//
//        default:
//            std::cout << "  Invalid choice." << std::endl;
//        }
//
//    } while (choice != 6);
//}
//
//// ================================================================
////  ADMIN SESSION  (Step 8)
//// ================================================================
//
//static void showAdminMenu(const char* name)
//{
//    printLine();
//    std::cout << "  Admin: " << name << std::endl;
//    printLine();
//    std::cout << "  1.  Add Doctor" << std::endl;
//    std::cout << "  2.  Remove Doctor" << std::endl;
//    std::cout << "  3.  View All Patients" << std::endl;
//    std::cout << "  4.  View All Doctors" << std::endl;
//    std::cout << "  5.  View All Appointments" << std::endl;
//    std::cout << "  6.  View Unpaid Bills" << std::endl;
//    std::cout << "  7.  Discharge Patient" << std::endl;
//    std::cout << "  8.  View Security Log" << std::endl;
//    std::cout << "  9.  Generate Daily Report" << std::endl;
//    std::cout << "  10. Logout" << std::endl;
//    printLine();
//    std::cout << "  Choice: ";
//}
//
//static void handleAddDoctor(HospitalSystem& hospital)
//{
//    char name[50], spec[50], contact[20], password[50];
//    float fee;
//
//    clearInput();
//    std::cout << "  Name           : "; std::cin.getline(name, 50);
//    std::cout << "  Specialization : "; std::cin.getline(spec, 50);
//    std::cout << "  Contact        : "; std::cin.getline(contact, 20);
//    std::cout << "  Password       : "; std::cin.getline(password, 50);
//    std::cout << "  Fee            : "; std::cin >> fee;
//
//    try
//    {
//        bool ok = hospital.addDoctor(name, spec, contact, password, fee);
//
//        if (ok)
//            std::cout << "  Doctor added successfully." << std::endl;
//    }
//    catch (InvalidInputException& e)
//    {
//        std::cout << "  Error: " << e.what() << std::endl;
//    }
//}
//
//static void runAdminSession(HospitalSystem& hospital, int adminIndex)
//{
//    const char* name = hospital.getAdmins().get(adminIndex).getName();
//
//    int choice;
//
//    do
//    {
//        showAdminMenu(name);
//        std::cin >> choice;
//        std::cout << std::endl;
//
//        switch (choice)
//        {
//        case 1:
//            handleAddDoctor(hospital);
//            break;
//
//        case 2:
//        {
//            int docID;
//            std::cout << "  Doctor ID to remove: "; std::cin >> docID;
//            bool ok = hospital.removeDoctor(docID);
//            if (ok)
//                std::cout << "  Doctor removed." << std::endl;
//            else
//                std::cout << "  Failed. Doctor not found or has active scheduled appointments." << std::endl;
//            break;
//        }
//
//        case 3:
//            std::cout << "--- All Patients ---" << std::endl;
//            hospital.viewAllPatients();
//            break;
//
//        case 4:
//            std::cout << "--- All Doctors ---" << std::endl;
//            hospital.viewAllDoctors();
//            break;
//
//        case 5:
//            std::cout << "--- All Appointments ---" << std::endl;
//            hospital.viewAllAppointments();
//            break;
//
//        case 6:
//            std::cout << "--- Unpaid Bills ---" << std::endl;
//            hospital.viewUnpaidBills();
//            break;
//
//        case 7:
//        {
//            int patID;
//            std::cout << "  Patient ID to discharge: "; std::cin >> patID;
//            bool ok = hospital.dischargePatient(patID);
//            if (ok)
//                std::cout << "  Patient discharged and archived." << std::endl;
//            else
//                std::cout << "  Failed. Patient not found, has unpaid bills,"
//                " or has active scheduled appointments." << std::endl;
//            break;
//        }
//
//        case 8:
//            std::cout << "--- Security Log ---" << std::endl;
//            hospital.viewSecurityLog();
//            break;
//
//        case 9:
//        {
//            char date[20];
//            std::cout << "  Date for report (DD-MM-YYYY): "; std::cin >> date;
//            std::cout << std::endl;
//            hospital.generateDailyReport(date);
//            break;
//        }
//
//        case 10:
//            std::cout << "  Logged out." << std::endl;
//            break;
//
//        default:
//            std::cout << "  Invalid choice." << std::endl;
//        }
//
//    } while (choice != 10);
//}
//
//// ================================================================
////  MAIN
//// ================================================================
//
//int main()
//{
//    HospitalSystem hospital;
//
//    try
//    {
//        hospital.loadSystem();
//    }
//    catch (HospitalException& e)
//    {
//        std::cout << "Startup error: " << e.what() << std::endl;
//        return 1;
//    }
//
//    std::cout << "MediCore loaded. "
//        << "Patients: " << hospital.getPatients().size()
//        << "  Doctors: " << hospital.getDoctors().size()
//        << "  Admins: " << hospital.getAdmins().size()
//        << std::endl;
//
//    int choice;
//
//    do
//    {
//        showMainMenu();
//        std::cin >> choice;
//
//        if (!Validator::isValidMenuChoice(choice, 1, 4))
//        {
//            std::cout << "  Invalid option." << std::endl;
//            continue;
//        }
//
//        if (choice == 4) break;
//
//        int index = -1;
//
//        if (choice == 1) index = handleLogin(hospital, "Patient");
//        else if (choice == 2) index = handleLogin(hospital, "Doctor");
//        else if (choice == 3) index = handleLogin(hospital, "Admin");
//
//        if (index == -1) continue;   // locked out — back to main menu
//
//        if (choice == 1) runPatientSession(hospital, index);
//        else if (choice == 2) runDoctorSession(hospital, index);
//        else if (choice == 3) runAdminSession(hospital, index);
//
//    } while (choice != 4);
//
//    std::cout << std::endl << "Goodbye." << std::endl;
//
//    return 0;
//}
