#pragma once
#ifndef INVALIDINPUTEXCEPTION_H
#define INVALIDINPUTEXCEPTION_H

#include "HospitalException.h"

class InvalidInputException : public HospitalException
{
public:

    InvalidInputException();

    ~InvalidInputException();
};

#endif
