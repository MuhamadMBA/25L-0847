#pragma once
#ifndef VALIDATOR_H
#define VALIDATOR_H

class Validator
{
public:

    static bool isValidID(int);

    static bool isValidAge(int);

    static bool isValidAmount(float);

    static bool isValidPositiveAmount(float);

    static bool isValidContact(const char*);

    static bool isValidGender(char);

    static bool isValidDate(const char*);

    static bool isValidTime(const char*);

    static bool isValidPassword(const char*);

    static bool isValidMenuChoice(int, int, int);
};

#endif
