#include "Person.h"

Person::Person()
{
    id = 0;
    name[0] = '\0';
    password[0] = '\0';
}

Person::Person(int i, const char* n, const char* p)
{
    id = i;
    int ind = 0;
    while (n[ind] != '\0')
    {
        name[ind] = n[ind];
        ind++;
    }
    name[ind] = '\0';
    ind = 0;
    while (p[ind] != '\0')
    {
        password[ind] = p[ind];
        ind++;
    }
    password[ind] = '\0';
}

void Person::setID(int value)
{
    id = value;
}

int Person::getID() const
{
    return id;
}

void Person::setName(const char* n)
{
    int index = 0;
    while (n[index] != '\0')
    {
        name[index] = n[index];
        index++;
    }
    name[index] = '\0';
}

const char* Person::getName() const
{
    return name;
}

void Person::setPassword(const char* p)
{
    int index = 0;
    while (p[index] != '\0')
    {
        password[index] = p[index];
        index++;
    }
    password[index] = '\0';
}

const char* Person::getPassword() const
{
    return password;
}

Person::~Person()
{
}