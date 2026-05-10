#include "Doctor.h"


Doctor::Doctor() : Person()
{
    specialization[0] = '\0';

    contact[0] = '\0';

    fee = 0;
}

Doctor::Doctor(int i, const char* n, const char* p, const char* s, const char* c, float f) : Person(i, n, p)
{
    int index = 0;

    while (s[index] != '\0')
    {
        specialization[index] = s[index];
        index++;
    }

    specialization[index] = '\0';

    index = 0;

    while (c[index] != '\0')
    {
        contact[index] = c[index];
        index++;
    }

    contact[index] = '\0';

    fee = f;
}

void Doctor::setSpecialization(const char* s)
{
    int index = 0;

    while (s[index] != '\0')
    {
        specialization[index] = s[index];
        index++;
    }

    specialization[index] = '\0';
}

const char* Doctor::getSpecialization() const
{
    return specialization;
}

void Doctor::setContact(const char* c)
{
    int index = 0;

    while (c[index] != '\0')
    {
        contact[index] = c[index];
        index++;
    }

    contact[index] = '\0';
}

const char* Doctor::getContact() const
{
    return contact;
}

void Doctor::setFee(float f)
{
    fee = f;
}

float Doctor::getFee() const
{
    return fee;
}

bool Doctor::operator == (const Doctor& other) const
{
    return id == other.id;
}

std::ostream& operator<<(std::ostream& out, const Doctor& other)
{
    out << "Doctor ID: " << other.id << std::endl;

    out << "Name: " << other.name << std::endl;

    out << "Specialization: " << other.specialization << std::endl;

    out << "Contact: " << other.contact << std::endl;

    out << "Fee: " << other.fee << std::endl;

    return out;
}

void Doctor::displayMenu()
{

}

void Doctor::displayInfo()
{

}

Doctor::~Doctor()
{

}
