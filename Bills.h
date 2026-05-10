#pragma once
#ifndef BILL_H
#define BILL_H

#include <iostream>

class Bill
{
private:

    int billID;

    int patientID;

    int appointmentID;

    float amount;

    char status[20];

    char date[20];

public:

    Bill();

    Bill(int bId, int pId, int aId, float a, const char* s, const char* d);

    void setBillID(int bId);
    int getBillID() const;
    int getID() const;

    void setPatientID(int pId);
    int getPatientID() const;

    void setAppointmentID(int aId);
    int getAppointmentID() const;

    void setAmount(float);
    float getAmount() const;

    void setStatus(const char* s);
    const char* getStatus() const;

    void setDate(const char* d);
    const char* getDate() const;

    bool operator == (const Bill& other) const;

    friend std::ostream& operator <<(std::ostream& out, const Bill& other);

    ~Bill();
};

#endif
