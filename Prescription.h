#ifndef PRESCRIPTION_H
#define PRESCRIPTION_H

#include <iostream>

class Prescription
{
private:

    int prescriptionID;

    int patientID;

    int doctorID;

    int appointmentID;

    char medicines[200];

    char notes[300];

    char date[20];

public:

    Prescription();

    Prescription(int p, int pID, int dId, int aId, const char* m, const char* n, const char* d);

    void setPrescriptionID(int p);
    int getPrescriptionID() const;
    int getID() const;

    void setPatientID(int pID);
    int getPatientID() const;

    void setDoctorID(int dId);
    int getDoctorID() const;

    void setAppointmentID(int aId);
    int getAppointmentID() const;

    void setMedicines(const char* m);
    const char* getMedicines() const;

    void setNotes(const char* n);
    const char* getNotes() const;

    void setDate(const char* d);
    const char* getDate() const;

    bool operator == (const Prescription& other) const;

    friend std::ostream& operator <<(std::ostream&, const Prescription&);

    ~Prescription();
};

#endif
