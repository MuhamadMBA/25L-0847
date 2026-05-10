#include "Validator.h"

bool Validator::isValidID(int id)
{
    return id > 0;
}

bool Validator::isValidAge(int age)
{
    return age > 0 && age <= 120;
}


bool Validator::isValidAmount(float amount)
{
    return amount >= 0;
}


bool Validator::isValidContact(const char* contact)
{
    int length = 0;

    while (contact[length] != '\0')
    {
        if (contact[length] < '0' || contact[length] > '9')
        {
            return false;
        }

        length++;
    }

    return length == 11;
}


bool Validator::isValidGender(char gender)
{
    return (gender == 'M' || gender == 'm' || gender == 'F' || gender == 'f');
}

bool Validator::isValidMenuChoice(int choice, int min, int max)
{
    return choice >= min && choice <= max;
}


//Date formating: DD-MM-YYYY

bool Validator::isValidDate(const char* date)
{
    if (date[2] != '-' || date[5] != '-')
    {
        return false;
    }

    int length = 0;

    while (date[length] != '\0')
    {
        length++;
    }

    if (length != 10)
    {
        return false;
    }

    for (int i = 0; i < 10; i++)
    {
        if (i == 2 || i == 5)
        {
            continue;
        }

        if (date[i] < '0' || date[i] > '9')
        {
            return false;
        }
    }

    int day = (date[0] - '0') * 10 + (date[1] - '0');
    int month = (date[3] - '0') * 10 + (date[4] - '0');
    int year = (date[6] - '0') * 1000 + (date[7] - '0') * 100 + (date[8] - '0') * 10 + (date[9] - '0');

    if (year < 2026)
    {
        return false;
    }

    if (month < 1 || month > 12)
    {
        return false;
    }

    if (day < 1 || day > 31)
    {
        return false;
    }

    return true;
}
// Amount must be  greater than zero 
bool Validator::isValidPositiveAmount(float amount)
{
    return amount > 0;
}

// Time format: HH:MM  24-hour
bool Validator::isValidTime(const char* t)
{
    int length = 0;

    while (t[length] != '\0')
    {
        length++;
    }

    if (length != 5)
    {
        return false;
    }

    if (t[2] != ':')
    {
        return false;
    }

    for (int i = 0; i < 5; i++)
    {
        if (i == 2) continue;

        if (t[i] < '0' || t[i] > '9')
        {
            return false;
        }
    }

    int hour = (t[0] - '0') * 10 + (t[1] - '0');
    int minute = (t[3] - '0') * 10 + (t[4] - '0');

    return hour >= 0 && hour <= 23 && minute >= 0 && minute <= 59;
}

// Password: at least 6 characters, no spaces
bool Validator::isValidPassword(const char* pwd)
{
    int length = 0;

    while (pwd[length] != '\0')
    {
        if (pwd[length] == ' ')
        {
            return false;
        }

        length++;
    }

    return length >= 6;
}
