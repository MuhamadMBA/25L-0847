#include "HospitalSystem.h"
#include "HospitalException.h"
#include "FileNotFoundException.h"
#include "InvalidInputException.h"
#include "InsufficientFundsException.h"
#include "SlotUnavailableException.h"
#include "Validator.h"
#include <iostream>

// ---------------------------------------------------------------
// Helper: compare two char arrays without using <cstring>
// ---------------------------------------------------------------
static bool strMatch(const char* a, const char* b)
{
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0')
    {
        if (a[i] != b[i]) return false;
        i++;
    }
    return a[i] == '\0' && b[i] == '\0';
}

// ---------------------------------------------------------------
// Constructor / Destructor
// ---------------------------------------------------------------

HospitalSystem::HospitalSystem()
{
    // FileHandler initialises its own file paths in its constructor.
    // Storage objects initialise themselves (count = 0) in their constructor.
}

HospitalSystem::~HospitalSystem()
{
}

// ---------------------------------------------------------------
// loadSystem
// Loads every data file into the matching Storage object.
// Call this once at program startup.
// ---------------------------------------------------------------

void HospitalSystem::loadSystem()
{
    fileHandler.loadPatients(patients);
    fileHandler.loadDoctors(doctors);
    fileHandler.loadAdmins(admins);
    fileHandler.loadAppointments(appointments);
    fileHandler.loadBill(bills);
    fileHandler.loadPrescriptions(prescriptions);
}

// ---------------------------------------------------------------
// Login
// Returns the storage index on success, -1 on failure.
// failCount is incremented by the caller; when it reaches 3
// the caller should lock the session and log via FileHandler.
// ---------------------------------------------------------------

int HospitalSystem::loginPatient(int id, const char* password, int& failCount)
{
    for (int i = 0; i < patients.size(); i++)
    {
        if (patients.get(i).getID() == id)
        {
            if (strMatch(patients.get(i).getPassword(), password))
            {
                failCount = 0;
                return i;
            }
        }
    }

    failCount++;

    if (failCount >= 3)
    {
        fileHandler.writeSecurityLog("Patient", id, "FAILED - session locked");
    }
    else
    {
        fileHandler.writeSecurityLog("Patient", id, "FAILED");
    }

    return -1;
}

int HospitalSystem::loginDoctor(int id, const char* password, int& failCount)
{
    for (int i = 0; i < doctors.size(); i++)
    {
        if (doctors.get(i).getID() == id)
        {
            if (strMatch(doctors.get(i).getPassword(), password))
            {
                failCount = 0;
                return i;
            }
        }
    }

    failCount++;

    if (failCount >= 3)
    {
        fileHandler.writeSecurityLog("Doctor", id, "FAILED - session locked");
    }
    else
    {
        fileHandler.writeSecurityLog("Doctor", id, "FAILED");
    }

    return -1;
}

int HospitalSystem::loginAdmin(int id, const char* password, int& failCount)
{
    for (int i = 0; i < admins.size(); i++)
    {
        if (admins.get(i).getID() == id)
        {
            if (strMatch(admins.get(i).getPassword(), password))
            {
                failCount = 0;
                return i;
            }
        }
    }

    failCount++;

    if (failCount >= 3)
    {
        fileHandler.writeSecurityLog("Admin", id, "FAILED - session locked");
    }
    else
    {
        fileHandler.writeSecurityLog("Admin", id, "FAILED");
    }

    return -1;
}

// ---------------------------------------------------------------
// Patient features
// ---------------------------------------------------------------

bool HospitalSystem::bookAppointment(int patientIndex, int doctorID,
    const char* date, const char* time)
{
    // 1. Validate date and time format
    if (!Validator::isValidDate(date))
    {
        throw InvalidInputException();
    }

    if (!Validator::isValidTime(time))
    {
        throw InvalidInputException();
    }

    // 2. Find doctor
    int doctorIndex = doctors.findByID(doctorID);

    if (doctorIndex == -1)
    {
        throw InvalidInputException();
    }

    Patient& patient = patients.getReference(patientIndex);
    Doctor   doctor = doctors.get(doctorIndex);

    // 3. Check patient balance covers the doctor fee
    if (patient.getBalance() < doctor.getFee())
    {
        throw InsufficientFundsException();
    }

    // 4. Check slot conflict using Appointment::operator==
    //    operator== returns true when same doctor + same date + same time
    //    and neither appointment is Cancelled
    Appointment probe(0, patient.getID(), doctorID, date, time, "Scheduled");

    for (int i = 0; i < appointments.size(); i++)
    {
        if (appointments.get(i) == probe)
        {
            throw SlotUnavailableException();
        }
    }

    // 5. Generate next IDs
    int newApptID = fileHandler.getNextAppointmentID();
    int newBillID = fileHandler.getNextBillID();

    // 6. Create appointment and bill
    //    Bill is Unpaid   patient pays separately via payBill()
    Appointment newAppt(newApptID, patient.getID(), doctorID, date, time, "Scheduled");

    Bill newBill(newBillID, patient.getID(), newApptID,
        doctor.getFee(), "Unpaid", date);

    // 7. Deduct doctor fee from patient balance
    patient -= doctor.getFee();
    fileHandler.updatePatient(patient);

    // 8. Persist appointment and bill to files
    fileHandler.appendAppointment(newAppt);
    fileHandler.appendBill(newBill);

    // 9. Update in-memory storage
    appointments.add(newAppt);
    bills.add(newBill);


    return true;
}

bool HospitalSystem::cancelAppointment(int appointmentID, int patientID)
{
    int index = appointments.findByID(appointmentID);

    if (index == -1)
    {
        return false;
    }

    Appointment& appt = appointments.getReference(index);

    // Ownership check   patient can only cancel their own appointments
    if (appt.getPatientID() != patientID)
    {
        return false;
    }

    // Can only cancel a Scheduled appointment
    if (!strMatch(appt.getStatus(), "Scheduled"))
    {
        return false;
    }

    // Update appointment status
    appt.setStatus("Cancelled");
    fileHandler.updateAppointment(appt);

    // Find the corresponding bill and handle refund
    for (int i = 0; i < bills.size(); i++)
    {
        Bill& bill = bills.getReference(i);

        if (bill.getAppointmentID() == appointmentID)
        {
            // If the bill was already paid, refund the patient using operator+=
            // Refund if bill is Unpaid (fee was deducted at booking time)
            if (strMatch(bill.getStatus(), "Unpaid") || strMatch(bill.getStatus(), "Paid"))
            {
                int patientIndex = patients.findByID(patientID);
                if (patientIndex != -1)
                {
                    patients.getReference(patientIndex) += bill.getAmount();
                    fileHandler.updatePatient(patients.get(patientIndex));
                }
            }

            // Mark bill as Cancelled regardless
            bill.setStatus("Cancelled");
            fileHandler.updateBill(bill);
            break;
        }
    }

    return true;
}

bool HospitalSystem::payBill(int billID, int patientID)
{
    // 1. Find the bill
    int billIndex = bills.findByID(billID);

    if (billIndex == -1)
    {
        return false;
    }

    Bill& bill = bills.getReference(billIndex);

    // 2. Ownership check
    if (bill.getPatientID() != patientID)
    {
        return false;
    }

    // 3. Only unpaid bills can be paid
    if (!strMatch(bill.getStatus(), "Unpaid"))
    {
        return false;
    }

    // 4. Find the patient
    int patientIndex = patients.findByID(patientID);

    if (patientIndex == -1)
    {
        return false;
    }

    Patient& patient = patients.getReference(patientIndex);

    // 5. Check balance
    if (patient.getBalance() < bill.getAmount())
    {
        throw InsufficientFundsException();
    }

    // 6. Deduct from patient balance using operator-=
    patient -= bill.getAmount();

    // 7. Mark bill as Paid
    bill.setStatus("Paid");

    // 8. Persist both changes to files
    fileHandler.updatePatient(patient);
    fileHandler.updateBill(bill);

    return true;
}

bool HospitalSystem::topUpBalance(int patientIndex, float amount)
{
    // Amount must be positive
    if (!Validator::isValidPositiveAmount(amount))
    {
        throw InvalidInputException();
    }

    Patient& patient = patients.getReference(patientIndex);

    // Add to balance using operator+=
    patient += amount;

    fileHandler.updatePatient(patient);

    return true;
}

void HospitalSystem::viewPatientAppointments(int patientID) const
{
    bool found = false;

    for (int i = 0; i < appointments.size(); i++)
    {
        if (appointments.get(i).getPatientID() == patientID)
        {
            std::cout << appointments.get(i);
            std::cout << "----------------------------" << std::endl;
            found = true;
        }
    }

    if (!found)
    {
        std::cout << "No appointments found." << std::endl;
    }
}

void HospitalSystem::viewPatientBills(int patientID) const
{
    bool found = false;

    for (int i = 0; i < bills.size(); i++)
    {
        if (bills.get(i).getPatientID() == patientID)
        {
            std::cout << bills.get(i);
            std::cout << "----------------------------" << std::endl;
            found = true;
        }
    }

    if (!found)
    {
        std::cout << "No bills found." << std::endl;
    }
}

void HospitalSystem::viewPatientPrescriptions(int patientID) const
{
    bool found = false;

    for (int i = 0; i < prescriptions.size(); i++)
    {
        if (prescriptions.get(i).getPatientID() == patientID)
        {
            std::cout << prescriptions.get(i);
            std::cout << "----------------------------" << std::endl;
            found = true;
        }
    }

    if (!found)
    {
        std::cout << "  No prescriptions found." << std::endl;
    }
}

void HospitalSystem::searchDoctorsBySpecialization(const char* spec) const
{
    bool found = false;

    for (int i = 0; i < doctors.size(); i++)
    {
        const char* docSpec = doctors.get(i).getSpecialization();

        int sLen = 0;
        while (spec[sLen] != '\0') sLen++;

        int dLen = 0;
        while (docSpec[dLen] != '\0') dLen++;

        bool matched = false;

        for (int d = 0; d <= dLen - sLen && !matched; d++)
        {
            bool same = true;

            for (int s = 0; s < sLen && same; s++)
            {
                char a = docSpec[d + s];
                char b = spec[s];

                // Lowercase both before comparing   manual tolower
                if (a >= 'A' && a <= 'Z') a = a + 32;
                if (b >= 'A' && b <= 'Z') b = b + 32;

                if (a != b) same = false;
            }

            if (same) matched = true;
        }

        if (matched)
        {
            std::cout << doctors.get(i);
            std::cout << "----------------------------" << std::endl;
            found = true;
        }
    }

    if (!found)
    {
        std::cout << "No doctors found for that specialization." << std::endl;
    }
}

// ---------------------------------------------------------------
// Doctor features
// ---------------------------------------------------------------

bool HospitalSystem::markAppointmentComplete(int appointmentID, int doctorID)
{
    int index = appointments.findByID(appointmentID);

    if (index == -1)
    {
        return false;
    }

    Appointment& appt = appointments.getReference(index);

    // Ownership check   doctor can only update their own appointments
    if (appt.getDoctorID() != doctorID)
    {
        return false;
    }

    // Can only complete a Scheduled appointment
    if (!strMatch(appt.getStatus(), "Scheduled"))
    {
        return false;
    }

    appt.setStatus("Completed");

    fileHandler.updateAppointment(appt);

    return true;
}

bool HospitalSystem::markNoShow(int appointmentID, int doctorID)
{
    int index = appointments.findByID(appointmentID);

    if (index == -1)
    {
        return false;
    }

    Appointment& appt = appointments.getReference(index);

    if (appt.getDoctorID() != doctorID)
    {
        return false;
    }

    if (!strMatch(appt.getStatus(), "Scheduled"))
    {
        return false;
    }

    appt.setStatus("No-Show");

    // PDF: mark no-show also cancels the corresponding bill (no refund)
    for (int i = 0; i < bills.size(); i++)
    {
        Bill& bill = bills.getReference(i);

        if (bill.getAppointmentID() == appointmentID &&
            strMatch(bill.getStatus(), "Unpaid"))
        {
            bill.setStatus("Cancelled");
            fileHandler.updateBill(bill);
            break;
        }
    }

    fileHandler.updateAppointment(appt);

    return true;
}

bool HospitalSystem::writePrescription(int appointmentID, int doctorID,
    const char* medicines, const char* notes,
    const char* date)
{
    // 1. Find the appointment
    int apptIndex = appointments.findByID(appointmentID);

    if (apptIndex == -1)
    {
        return false;
    }

    Appointment& appt = appointments.getReference(apptIndex);

    // 2. Ownership check
    if (appt.getDoctorID() != doctorID)
    {
        return false;
    }

    // 3. Can only prescribe for a Completed appointment
    if (!strMatch(appt.getStatus(), "Completed"))
    {
        return false;
    }

    // 4. One prescription per appointment   check none exists yet
    for (int i = 0; i < prescriptions.size(); i++)
    {
        if (prescriptions.get(i).getAppointmentID() == appointmentID)
        {
            return false;
        }
    }

    // 5. Validate inputs
    if (medicines[0] == '\0' || notes[0] == '\0')
    {
        throw InvalidInputException();
    }

    if (!Validator::isValidDate(date))
    {
        throw InvalidInputException();
    }

    // 6. Generate ID and create prescription
    int newPresID = fileHandler.getNextPrescriptionID();

    Prescription newPres(newPresID,
        appt.getPatientID(),
        doctorID,
        appointmentID,
        medicines,
        notes,
        date);

    // 7. Persist and update storage
    fileHandler.appendPrescription(newPres);

    prescriptions.add(newPres);

    return true;
}

void HospitalSystem::viewTodayAppointments(int doctorID, const char* today) const
{
    bool found = false;

    for (int i = 0; i < appointments.size(); i++)
    {
        const Appointment& appt = appointments.get(i);

        if (appt.getDoctorID() == doctorID && strMatch(appt.getDate(), today))
        {
            std::cout << appt;
            std::cout << "----------------------------" << std::endl;
            found = true;
        }
    }

    if (!found)
    {
        std::cout << "  No appointments for today (" << today << ")." << std::endl;
    }
}

void HospitalSystem::viewPatientHistory(int patientID) const
{
    // Collect matching appointment indices
    int indices[100];
    int count = 0;

    for (int i = 0; i < appointments.size() && count < 100; i++)
    {
        if (appointments.get(i).getPatientID() == patientID)
        {
            indices[count++] = i;
        }
    }

    if (count == 0)
    {
        std::cout << "  No medical history found." << std::endl;
        return;
    }

    // Manual bubble sort by date ascending
    // Date is DD-MM-YYYY: build YYYYMMDD key for lexicographic comparison
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = 0; j < count - i - 1; j++)
        {
            const char* da = appointments.get(indices[j]).getDate();
            const char* db = appointments.get(indices[j + 1]).getDate();

            char keyA[9];
            char keyB[9];

            keyA[0] = da[6]; keyA[1] = da[7]; keyA[2] = da[8]; keyA[3] = da[9];
            keyA[4] = da[3]; keyA[5] = da[4];
            keyA[6] = da[0]; keyA[7] = da[1];
            keyA[8] = '\0';

            keyB[0] = db[6]; keyB[1] = db[7]; keyB[2] = db[8]; keyB[3] = db[9];
            keyB[4] = db[3]; keyB[5] = db[4];
            keyB[6] = db[0]; keyB[7] = db[1];
            keyB[8] = '\0';

            bool swap = false;
            for (int k = 0; k < 8; k++)
            {
                if (keyA[k] > keyB[k]) { swap = true;  break; }
                if (keyA[k] < keyB[k]) { swap = false; break; }
            }

            if (swap)
            {
                int temp = indices[j];
                indices[j] = indices[j + 1];
                indices[j + 1] = temp;
            }
        }
    }

    // Print sorted appointments with their prescription if it exists
    for (int i = 0; i < count; i++)
    {
        const Appointment& appt = appointments.get(indices[i]);

        std::cout << appt;

        bool hasPres = false;

        for (int p = 0; p < prescriptions.size(); p++)
        {
            if (prescriptions.get(p).getAppointmentID() == appt.getAppointmentID())
            {
                std::cout << "  >> Prescription:" << std::endl;
                std::cout << prescriptions.get(p);
                hasPres = true;
                break;
            }
        }

        if (!hasPres)
        {
            std::cout << "  >> No prescription for this appointment." << std::endl;
        }

        std::cout << "----------------------------" << std::endl;
    }
}

// ---------------------------------------------------------------
// Admin features
// ---------------------------------------------------------------

bool HospitalSystem::addDoctor(const char* name, const char* specialization,
    const char* contact, const char* password, float fee)
{
    if (!Validator::isValidContact(contact))
    {
        throw InvalidInputException();
    }

    if (!Validator::isValidPassword(password))
    {
        throw InvalidInputException();
    }

    if (!Validator::isValidPositiveAmount(fee))
    {
        throw InvalidInputException();
    }

    int newID = fileHandler.getNextDoctorID();

    Doctor newDoctor(newID, name, password, specialization, contact, fee);

    fileHandler.appendDoctor(newDoctor);

    doctors.add(newDoctor);

    return true;
}

bool HospitalSystem::removeDoctor(int doctorID)
{
    int index = doctors.findByID(doctorID);

    if (index == -1)
    {
        return false;
    }

    // Safety check   cannot remove a doctor with active Scheduled appointments
    for (int i = 0; i < appointments.size(); i++)
    {
        const Appointment& appt = appointments.get(i);

        if (appt.getDoctorID() == doctorID &&
            strMatch(appt.getStatus(), "Scheduled"))
        {
            return false;
        }
    }

    fileHandler.deleteDoctorByID(doctorID);

    doctors.removeByID(doctorID);

    return true;
}

bool HospitalSystem::dischargePatient(int patientID)
{
    int patientIndex = patients.findByID(patientID);

    if (patientIndex == -1)
    {
        return false;
    }

    // Cannot discharge if patient has unpaid bills
    for (int i = 0; i < bills.size(); i++)
    {
        if (bills.get(i).getPatientID() == patientID &&
            strMatch(bills.get(i).getStatus(), "Unpaid"))
        {
            return false;
        }
    }

    // Cannot discharge if patient has Scheduled appointments
    for (int i = 0; i < appointments.size(); i++)
    {
        if (appointments.get(i).getPatientID() == patientID &&
            strMatch(appointments.get(i).getStatus(), "Scheduled"))
        {
            return false;
        }
    }

    // Archive to discharged.txt
    fileHandler.appendDischargedPatient(patients.get(patientIndex));

    // Remove from all active files
    fileHandler.deletePatientByID(patientID);

    for (int i = 0; i < appointments.size(); i++)
    {
        if (appointments.get(i).getPatientID() == patientID)
        {
            fileHandler.deleteAppointmentByID(appointments.get(i).getID());
        }
    }

    for (int i = 0; i < bills.size(); i++)
    {
        if (bills.get(i).getPatientID() == patientID)
        {
            fileHandler.deleteBillByID(bills.get(i).getID());
        }
    }

    for (int i = 0; i < prescriptions.size(); i++)
    {
        if (prescriptions.get(i).getPatientID() == patientID)
        {
            fileHandler.deletePrescriptionByID(prescriptions.get(i).getID());
        }
    }

    // Remove from in-memory storage (backwards to avoid index shift)
    patients.removeByID(patientID);

    for (int i = appointments.size() - 1; i >= 0; i--)
    {
        if (appointments.get(i).getPatientID() == patientID)
        {
            appointments.remove(i);
        }
    }

    for (int i = bills.size() - 1; i >= 0; i--)
    {
        if (bills.get(i).getPatientID() == patientID)
        {
            bills.remove(i);
        }
    }

    for (int i = prescriptions.size() - 1; i >= 0; i--)
    {
        if (prescriptions.get(i).getPatientID() == patientID)
        {
            prescriptions.remove(i);
        }
    }

    return true;
}

void HospitalSystem::viewAllPatients() const
{
    if (patients.size() == 0)
    {
        std::cout << "  No patients registered." << std::endl;
        return;
    }

    for (int i = 0; i < patients.size(); i++)
    {
        std::cout << patients.get(i);
        std::cout << "----------------------------" << std::endl;
    }
}

void HospitalSystem::viewAllDoctors() const
{
    if (doctors.size() == 0)
    {
        std::cout << "  No doctors registered." << std::endl;
        return;
    }

    for (int i = 0; i < doctors.size(); i++)
    {
        std::cout << doctors.get(i);
        std::cout << "----------------------------" << std::endl;
    }
}

void HospitalSystem::viewAllAppointments() const
{
    if (appointments.size() == 0)
    {
        std::cout << "  No appointments found." << std::endl;
        return;
    }

    for (int i = 0; i < appointments.size(); i++)
    {
        std::cout << appointments.get(i);
        std::cout << "----------------------------" << std::endl;
    }
}

void HospitalSystem::viewUnpaidBills() const
{
    bool found = false;

    for (int i = 0; i < bills.size(); i++)
    {
        if (strMatch(bills.get(i).getStatus(), "Unpaid"))
        {
            std::cout << bills.get(i);
            std::cout << "----------------------------" << std::endl;
            found = true;
        }
    }

    if (!found)
    {
        std::cout << "  No unpaid bills." << std::endl;
    }
}

void HospitalSystem::viewSecurityLog() const
{
    fileHandler.viewSecurityLog();
}

void HospitalSystem::generateDailyReport(const char* date) const
{
    std::cout << "========================================" << std::endl;
    std::cout << "  DAILY REPORT -- " << date << std::endl;
    std::cout << "========================================" << std::endl;

    // Appointments for this date, sorted by time (bubble sort)
    int indices[200];
    int count = 0;

    for (int i = 0; i < appointments.size() && count < 200; i++)
    {
        if (strMatch(appointments.get(i).getDate(), date))
        {
            indices[count++] = i;
        }
    }

    // Sort by time string   HH:MM sorts correctly as plain text
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = 0; j < count - i - 1; j++)
        {
            const char* ta = appointments.get(indices[j]).getTime();
            const char* tb = appointments.get(indices[j + 1]).getTime();

            bool swap = false;

            for (int k = 0; k < 5; k++)
            {
                if (ta[k] > tb[k]) { swap = true;  break; }
                if (ta[k] < tb[k]) { swap = false; break; }
            }

            if (swap)
            {
                int tmp = indices[j];
                indices[j] = indices[j + 1];
                indices[j + 1] = tmp;
            }
        }
    }

    std::cout << std::endl;
    std::cout << "  Appointments (" << count << "):" << std::endl;
    std::cout << "  ----------------------" << std::endl;

    int completed = 0, cancelled = 0, noshow = 0, scheduled = 0;

    for (int i = 0; i < count; i++)
    {
        const Appointment& a = appointments.get(indices[i]);
        std::cout << a;

        if (strMatch(a.getStatus(), "Completed")) completed++;
        else if (strMatch(a.getStatus(), "Cancelled")) cancelled++;
        else if (strMatch(a.getStatus(), "No-Show"))   noshow++;
        else                                           scheduled++;
    }

    std::cout << "  Summary: Completed=" << completed
        << "  Cancelled=" << cancelled
        << "  No-Show=" << noshow
        << "  Scheduled=" << scheduled << std::endl;

    // Revenue from paid bills on this date
    std::cout << std::endl;
    std::cout << "  Billing:" << std::endl;
    std::cout << "  ----------------------" << std::endl;

    float totalRevenue = 0, totalUnpaid = 0;
    int   paidCount = 0, unpaidCount = 0;

    for (int i = 0; i < bills.size(); i++)
    {
        const Bill& b = bills.get(i);

        if (strMatch(b.getDate(), date))
        {
            if (strMatch(b.getStatus(), "Paid"))
            {
                totalRevenue += b.getAmount();
                paidCount++;
            }
            else if (strMatch(b.getStatus(), "Unpaid"))
            {
                totalUnpaid += b.getAmount();
                unpaidCount++;
            }
        }
    }

    std::cout << "  Paid bills:   " << paidCount << "  Revenue: " << totalRevenue << std::endl;
    std::cout << "  Unpaid bills: " << unpaidCount << "  Pending: " << totalUnpaid << std::endl;
    std::cout << std::endl;
    std::cout << "  Total patients: " << patients.size()
        << "  Total doctors: " << doctors.size() << std::endl;
    std::cout << "========================================" << std::endl;
}

// ---------------------------------------------------------------
// Storage accessors
// ---------------------------------------------------------------

const Storage<Patient>& HospitalSystem::getPatients() const
{
    return patients;
}

const Storage<Doctor>& HospitalSystem::getDoctors() const
{
    return doctors;
}

const Storage<Admin>& HospitalSystem::getAdmins() const
{
    return admins;
}

const Storage<Appointment>& HospitalSystem::getAppointments() const
{
    return appointments;
}

const Storage<Bill>& HospitalSystem::getBills() const
{
    return bills;
}

const Storage<Prescription>& HospitalSystem::getPrescriptions() const
{
    return prescriptions;
}
