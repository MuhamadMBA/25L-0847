#include "HospitalException.h"

HospitalException::HospitalException()
{
    message = "Hospital system error";
}

HospitalException::HospitalException(const char* msg)
{
    message = msg;
}

const char* HospitalException::what() const noexcept
{
    return message;
}

HospitalException::~HospitalException()
{
}
