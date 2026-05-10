#include "Prescription.h"

Prescription::Prescription()
{
    prescriptionID = 0;

    patientID = 0;

    doctorID = 0;

    appointmentID = 0;

    medicines[0] = '\0';

    notes[0] = '\0';

    date[0] = '\0';
}

Prescription::Prescription(int prID, int pID, int dID, int aID, const char* m, const char* n, const char* dt)
{
    prescriptionID = prID;

    patientID = pID;

    doctorID = dID;

    appointmentID = aID;

    int index = 0;

    while (m[index] != '\0')
    {
        medicines[index] = m[index];
        index++;
    }

    medicines[index] = '\0';

    int  nindex = 0;

    while (n[nindex] != '\0')
    {
        notes[nindex] = n[nindex];
        nindex++;
    }

    notes[nindex] = '\0';

    int dtindex = 0;

    while (dt[dtindex] != '\0')
    {
        date[dtindex] = dt[dtindex];
        dtindex++;
    }

    date[dtindex] = '\0';
}

void Prescription::setPrescriptionID(int id)
{
    prescriptionID = id;
}

int Prescription::getPrescriptionID() const
{
    return prescriptionID;
}

int Prescription::getID() const
{
    return prescriptionID;
}

void Prescription::setPatientID(int id)
{
    patientID = id;
}

int Prescription::getPatientID() const
{
    return patientID;
}

void Prescription::setDoctorID(int id)
{
    doctorID = id;
}

int Prescription::getDoctorID() const
{
    return doctorID;
}

void Prescription::setAppointmentID(int id)
{
    appointmentID = id;
}

int Prescription::getAppointmentID() const
{
    return appointmentID;
}

void Prescription::setMedicines(const char* m)
{
    int index = 0;

    while (m[index] != '\0')
    {
        medicines[index] = m[index];
        index++;
    }

    medicines[index] = '\0';
}

const char* Prescription::getMedicines() const
{
    return medicines;
}

void Prescription::setNotes(const char* n)
{
    int index = 0;

    while (n[index] != '\0')
    {
        notes[index] = n[index];
        index++;
    }

    notes[index] = '\0';
}

const char* Prescription::getNotes() const
{
    return notes;
}

void Prescription::setDate(const char* dt)
{
    int index = 0;

    while (dt[index] != '\0')
    {
        date[index] = dt[index];
        index++;
    }

    date[index] = '\0';
}

const char* Prescription::getDate() const
{
    return date;
}


bool Prescription::operator == (const Prescription& other) const
{
    return prescriptionID == other.prescriptionID;
}

std::ostream& operator << (std::ostream& out,
    const Prescription& pr)
{
    out << "Prescription ID: " << pr.prescriptionID << std::endl;

    out << "Patient ID: " << pr.patientID << std::endl;

    out << "Doctor ID: " << pr.doctorID << std::endl;

    out << "Appointment ID: " << pr.appointmentID << std::endl;

    out << "Medicines: " << pr.medicines << std::endl;

    out << "Notes: " << pr.notes << std::endl;

    out << "Date: " << pr.date << std::endl;

    return out;
}

Prescription::~Prescription()
{

}



