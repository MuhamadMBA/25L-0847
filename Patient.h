#pragma once
#ifndef PATIENT_H
#define PATIENT_H

#include "Person.h"
#include <iostream>

class Patient : public Person
{
private:

    int age;

    char gender;

    char contact[20];

    float balance;

public:

    Patient();

    Patient(int i, const char* n, const char* p, int a, char g, const char* c, float b);

    void setAge(int);
    int getAge() const;

    void setGender(char);
    char getGender() const;

    void setContact(const char*);
    const char* getContact() const;

    void setBalance(float);
    float getBalance() const;

    Patient& operator += (float);

    Patient& operator -= (float);

    bool operator == (const Patient&) const;

    friend std::ostream& operator << (std::ostream&, const Patient&);

    void displayMenu();

    void displayInfo();

    ~Patient();
};

#endif