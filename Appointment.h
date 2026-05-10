#pragma once
#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <iostream>

class Appointment
{
private:

    int appointmentID;

    int patientID;

    int doctorID;

    char date[20];

    char time[20];

    char status[20];

public:

    Appointment();

    Appointment(int aid, int pid, int did, const char* d, const char* t, const char* s);

    void setAppointmentID(int aid);
    int getAppointmentID() const;
    int getID() const;

    void setPatientID(int pid);
    int getPatientID() const;

    void setDoctorID(int did);
    int getDoctorID() const;

    void setDate(const char* d);
    const char* getDate() const;

    void setTime(const char* t);
    const char* getTime() const;

    void setStatus(const char* s);
    const char* getStatus() const;

    bool operator == (const Appointment& other) const;

    friend std::ostream& operator <<(std::ostream&, const Appointment&);

    ~Appointment();
};

#endif