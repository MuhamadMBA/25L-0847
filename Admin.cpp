#include "Admin.h"

Admin::Admin() : Person()
{
    role[0] = '\0';
}

Admin::Admin(int i, const char* n, const char* p, const char* r) : Person(i, n, p)
{
    int index = 0;

    while (r[index] != '\0')
    {
        role[index] = r[index];
        index++;
    }

    role[index] = '\0';
}

void Admin::setRole(const char* r)
{
    int index = 0;

    while (r[index] != '\0')
    {
        role[index] = r[index];
        index++;
    }

    role[index] = '\0';
}

const char* Admin::getRole() const
{
    return role;
}

bool Admin::operator == (const Admin& other) const
{
    return id == other.id;
}

std::ostream& operator<<(std::ostream& out, const Admin& other)
{
    out << "Admin ID: " << other.id << std::endl;

    out << "Name: " << other.name << std::endl;

    out << "Role: " << other.role << std::endl;

    return out;
}

void Admin::displayMenu()
{

}

void Admin::displayInfo()
{

}

Admin::~Admin()
{

}


