#include "Appointment.h"

Appointment::Appointment()
{
    appointmentID = 0;

    patientID = 0;

    doctorID = 0;

    date[0] = '\0';

    time[0] = '\0';

    status[0] = '\0';
}

Appointment::Appointment(int aID, int pID, int dID, const char* d, const char* t, const char* s)
{
    appointmentID = aID;

    patientID = pID;

    doctorID = dID;

    int index = 0;

    while (d[index] != '\0')
    {
        date[index] = d[index];
        index++;
    }

    date[index] = '\0';

    int tindex = 0;

    while (t[tindex] != '\0')
    {
        time[tindex] = t[tindex];
        tindex++;
    }

    time[tindex] = '\0';

    int sindex = 0;

    while (s[sindex] != '\0')
    {
        status[sindex] = s[sindex];
        sindex++;
    }

    status[sindex] = '\0';
}

void Appointment::setAppointmentID(int id)
{
    appointmentID = id;
}

int Appointment::getAppointmentID() const
{
    return appointmentID;
}

int Appointment::getID() const
{
    return appointmentID;
}

void Appointment::setPatientID(int id)
{
    patientID = id;
}

int Appointment::getPatientID() const
{
    return patientID;
}

void Appointment::setDoctorID(int id)
{
    doctorID = id;
}

int Appointment::getDoctorID() const
{
    return doctorID;
}

void Appointment::setDate(const char* d)
{
    int index = 0;

    while (d[index] != '\0')
    {
        date[index] = d[index];
        index++;
    }

    date[index] = '\0';
}

const char* Appointment::getDate() const
{
    return date;
}

void Appointment::setTime(const char* t)
{
    int index = 0;

    while (t[index] != '\0')
    {
        time[index] = t[index];
        index++;
    }

    time[index] = '\0';
}


const char* Appointment::getTime() const
{
    return time;
}

void Appointment::setStatus(const char* s)
{
    int index = 0;

    while (s[index] != '\0')
    {
        status[index] = s[index];
        index++;
    }

    status[index] = '\0';
}

const char* Appointment::getStatus() const
{
    return status;
}

// Helper: compare two char arrays manually
static bool strEqual(const char* a, const char* b)
{
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0')
    {
        if (a[i] != b[i]) return false;
        i++;
    }
    return a[i] == '\0' && b[i] == '\0';
}

// Helper: check if a status string equals "Cancelled"
static bool isCancelled(const char* s)
{
    const char* word = "Cancelled";
    int i = 0;
    while (s[i] != '\0' && word[i] != '\0')
    {
        if (s[i] != word[i]) return false;
        i++;
    }
    return s[i] == '\0' && word[i] == '\0';
}

// operator== checks for a time-slot conflict, NOT ID equality.
// Two appointments conflict when:
//   - same doctor
//   - same date
//   - same time slot
//   - neither appointment is cancelled
bool Appointment::operator ==(const Appointment& other) const
{
    if (isCancelled(status) || isCancelled(other.status))
    {
        return false;
    }

    return  doctorID == other.doctorID &&
        strEqual(date, other.date) &&
        strEqual(time, other.time);
}


std::ostream& operator <<(std::ostream& out, const Appointment& other)
{
    out << "Appointment ID: " << other.appointmentID << std::endl;

    out << "Patient ID: " << other.patientID << std::endl;

    out << "Doctor ID: " << other.doctorID << std::endl;

    out << "Date: " << other.date << std::endl;

    out << "Time: " << other.time << std::endl;

    out << "Status: " << other.status << std::endl;

    return out;
}

Appointment::~Appointment()
{

}
