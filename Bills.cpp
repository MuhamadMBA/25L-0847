#include "Bills.h"

Bill::Bill()
{
    billID = 0;

    patientID = 0;

    appointmentID = 0;

    amount = 0;

    status[0] = '\0';

    date[0] = '\0';
}

Bill::Bill(int bID, int pID, int aID, float a, const char* s, const char* d)
{
    billID = bID;

    patientID = pID;

    appointmentID = aID;

    amount = a;

    int index = 0;

    while (s[index] != '\0')
    {
        status[index] = s[index];
        index++;
    }

    status[index] = '\0';

    int dindex = 0;

    while (d[dindex] != '\0')
    {
        date[dindex] = d[dindex];
        dindex++;
    }

    date[dindex] = '\0';
}

void Bill::setBillID(int id)
{
    billID = id;
}

int Bill::getBillID() const
{
    return billID;
}

int Bill::getID() const
{
    return billID;
}

void Bill::setPatientID(int id)
{
    patientID = id;
}

int Bill::getPatientID() const
{
    return patientID;
}

void Bill::setAppointmentID(int id)
{
    appointmentID = id;
}

int Bill::getAppointmentID() const
{
    return appointmentID;
}

void Bill::setAmount(float a)
{
    amount = a;
}

float Bill::getAmount() const
{
    return amount;
}

void Bill::setStatus(const char* s)
{
    int index = 0;

    while (s[index] != '\0')
    {
        status[index] = s[index];
        index++;
    }

    status[index] = '\0';
}

const char* Bill::getStatus() const
{
    return status;
}

void Bill::setDate(const char* d)
{
    int index = 0;

    while (d[index] != '\0')
    {
        date[index] = d[index];
        index++;
    }

    date[index] = '\0';
}

const char* Bill::getDate() const
{
    return date;
}

bool Bill::operator ==(const Bill& other) const
{
    return billID == other.billID;
}

std::ostream& operator <<(std::ostream& out, const Bill& other)
{
    out << "Bill ID: " << other.billID << std::endl;
    out << "Patient ID: " << other.patientID << std::endl;
    out << "Appointment ID: " << other.appointmentID << std::endl;
    out << "Amount: " << other.amount << std::endl;
    out << "Status: " << other.status << std::endl;
    out << "Date: " << other.date << std::endl;

    return out;
}

Bill::~Bill()
{

}
