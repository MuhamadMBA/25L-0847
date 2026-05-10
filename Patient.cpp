#include "Patient.h"

Patient::Patient() : Person()
{
    age = 0;

    gender = ' ';

    contact[0] = '\0';

    balance = 0;
}

Patient::Patient(int i, const char* n, const char* p, int a, char g, const char* c, float b) : Person(i, n, p)
{
    age = a;
    int index = 0;

    while (c[index] != '\0')
    {
        contact[index] = c[index];
        index++;
    }
    gender = g;

    contact[index] = '\0';

    balance = b;

}


void Patient::setAge(int a)
{
    age = a;
}

int Patient::getAge() const
{
    return age;
}

void Patient::setGender(char g)
{
    gender = g;
}

char Patient::getGender() const
{
    return gender;
}

void Patient::setContact(const char* c)
{
    int index = 0;
    while (c[index] != '\0')
    {
        contact[index] = c[index];
        index++;
    }

    contact[index] = '\0';

}

const char* Patient::getContact() const
{
    return contact;
}

void Patient::setBalance(float b)
{
    balance = b;
}

float Patient::getBalance() const
{
    return balance;
}

Patient& Patient:: operator +=(float amount)
{
    balance = balance + amount;
    return *this;
}

Patient& Patient:: operator -=(float amount)
{
    balance = balance - amount;
    return *this;
}

bool Patient::operator == (const Patient& other) const
{
    return (id == other.id);
}

void Patient::displayMenu()
{

}

void Patient::displayInfo()
{

}

std::ostream& operator << (std::ostream& out, const Patient& p)
{
    out << "Patient ID: " << p.id << std::endl;

    out << "Name: " << p.name << std::endl;

    out << "Age: " << p.age << std::endl;

    out << "Gender: " << p.gender << std::endl;

    out << "Contact: " << p.contact << std::endl;

    out << "Balance: " << p.balance << std::endl;

    return out;
}

Patient :: ~Patient()
{

}

