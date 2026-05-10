#ifndef DOCTOR_H
#define DOCTOR_H

#include "Person.h"
#include <iostream>

class Doctor : public Person
{
private:

    char specialization[50];

    char contact[20];

    float fee;

public:

    Doctor();

    Doctor(int, const char*, const char*, const char*, const char*, float);

    void setSpecialization(const char*);
    const char* getSpecialization() const;

    void setContact(const char*);
    const char* getContact() const;

    void setFee(float);
    float getFee() const;

    bool operator == (const Doctor&) const;

    friend std::ostream& operator <<(std::ostream&, const Doctor&);

    void displayMenu();

    void displayInfo();

    ~Doctor();
};

#endif
