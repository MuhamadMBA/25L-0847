#pragma once
#ifndef ADMIN_H
#define ADMIN_H

#include "Person.h"
#include <iostream>

class Admin : public Person
{
private:

    char role[30];

public:

    Admin();

    Admin(int i, const char* n, const char* p, const char* r);

    void setRole(const char* r);
    const char* getRole() const;

    bool operator == (const Admin& other) const;

    friend std::ostream& operator << (std::ostream& out, const Admin& other);

    void displayMenu();

    void displayInfo();

    ~Admin();
};

#endif

