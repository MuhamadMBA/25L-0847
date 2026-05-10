#pragma once
#ifndef PERSON_H
#define PERSON_H

class Person
{
protected:

    int id;
    char name[50];
    char password[50];

public:
    Person();

    Person(int i, const char* n, const char* p);

    void setID(int);
    int getID() const;

    void setName(const char*);

    const char* getName() const;

    void setPassword(const char*);

    const char* getPassword() const;

    virtual void displayMenu() = 0;

    virtual void displayInfo() = 0;

    virtual ~Person();
};

#endif