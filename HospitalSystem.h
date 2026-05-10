#pragma once
#ifndef HOSPITALSYSTEM_H
#define HOSPITALSYSTEM_H

#include "Storage.h"
#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Appointment.h"
#include "Bills.h"
#include "Prescription.h"
#include "FileHandler.h"

class HospitalSystem
{
private:

    Storage<Patient>      patients;
    Storage<Doctor>       doctors;
    Storage<Admin>        admins;
    Storage<Appointment>  appointments;
    Storage<Bill>         bills;
    Storage<Prescription> prescriptions;

    FileHandler fileHandler;

public:

    HospitalSystem();

    // -------------------------------------------------------
    // System
    // -------------------------------------------------------

    void loadSystem();

    // -------------------------------------------------------
    // Login
    // Returns the index inside the storage on success.
    // Returns -1 on failure (wrong ID or wrong password).
    // Logs failed attempts via FileHandler.
    // -------------------------------------------------------

    int loginPatient(int id, const char* password, int& failCount);
    int loginDoctor(int id, const char* password, int& failCount);
    int loginAdmin(int id, const char* password, int& failCount);

    // -------------------------------------------------------
    // Patient features
    // -------------------------------------------------------

    bool bookAppointment(int patientIndex, int doctorID, const char* date, const char* time);
    bool cancelAppointment(int appointmentID, int patientID);
    bool payBill(int billID, int patientID);
    bool topUpBalance(int patientIndex, float amount);

    void viewPatientAppointments(int patientID) const;
    void viewPatientBills(int patientID) const;
    void viewPatientPrescriptions(int patientID) const;
    void searchDoctorsBySpecialization(const char* spec) const;

    // -------------------------------------------------------
    // Doctor features
    // -------------------------------------------------------

    bool markAppointmentComplete(int appointmentID, int doctorID);
    bool markNoShow(int appointmentID, int doctorID);
    bool writePrescription(int appointmentID, int doctorID,
        const char* medicines, const char* notes,
        const char* date);

    void viewTodayAppointments(int doctorID, const char* today) const;
    void viewPatientHistory(int patientID) const;

    // -------------------------------------------------------
    // Admin features
    // -------------------------------------------------------

    bool addDoctor(const char* name, const char* specialization,
        const char* contact, const char* password, float fee);
    bool removeDoctor(int doctorID);
    bool dischargePatient(int patientID);

    void viewAllPatients() const;
    void viewAllDoctors() const;
    void viewAllAppointments() const;
    void viewUnpaidBills() const;
    void viewSecurityLog() const;
    void generateDailyReport(const char* date) const;

    // -------------------------------------------------------
    // Storage accessors (read-only)   used by GUI later
    // -------------------------------------------------------

    const Storage<Patient>& getPatients()      const;
    const Storage<Doctor>& getDoctors()       const;
    const Storage<Admin>& getAdmins()        const;
    const Storage<Appointment>& getAppointments()  const;
    const Storage<Bill>& getBills()         const;
    const Storage<Prescription>& getPrescriptions() const;

    ~HospitalSystem();
};

#endif
