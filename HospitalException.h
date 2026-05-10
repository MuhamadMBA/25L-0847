#pragma once
#ifndef HOSPITALEXCEPTION_H
#define HOSPITALEXCEPTION_H

#include <exception>

class HospitalException : public std::exception
{
protected:

    const char* message;

public:

    HospitalException();

    HospitalException(const char*);

    const char* what() const noexcept;

    virtual ~HospitalException();
};

#endif
