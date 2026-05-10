#pragma once
#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include "Storage.h"
#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Appointment.h"
#include "Bills.h"
#include "Prescription.h"

class FileHandler
{
private:

    const char* patientsFile;
    const char* doctorsFile;
    const char* adminFile;
    const char* appointmentsFile;
    const char* billsFile;
    const char* prescriptionsFile;
    const char* securityLogFile;
    const char* dischargedFile;

    void splitLine(const char*, char fields[][200], int&);

public:

    FileHandler();
    int getNextIDFromFile(const char*);


    void loadPatients(Storage<Patient>&);
    void loadDoctors(Storage<Doctor>&);
    void loadAdmins(Storage<Admin>&);
    void loadAppointments(Storage<Appointment>&);
    void loadBill(Storage<Bill>&);
    void loadPrescriptions(Storage<Prescription>&);

    void appendPatient(const Patient&);
    void appendDoctor(const Doctor&);
    void appendAdmin(const Admin&);
    void appendAppointment(const Appointment&);
    void appendBill(const Bill&);
    void appendPrescription(const Prescription&);

    void updatePatient(const Patient&);
    void updateDoctor(const Doctor&);
    void updateAppointment(const Appointment&);
    void updateBill(const Bill&);
    void updatePrescription(const Prescription&);

    void deletePatientByID(int);
    void deleteDoctorByID(int);
    void deleteAppointmentByID(int);
    void deleteBillByID(int);
    void deletePrescriptionByID(int);

    int getNextPatientID();
    int getNextDoctorID();
    int getNextAppointmentID();
    int getNextBillID();
    int getNextPrescriptionID();

    void writeSecurityLog(const char*, int, const char*);
    void viewSecurityLog() const;

    void appendDischargedPatient(const Patient&);

    ~FileHandler();
};

#endif
