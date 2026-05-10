#pragma once
#ifndef FILENOTFOUNDEXCEPTION_H
#define FILENOTFOUNDEXCEPTION_H

#include "HospitalException.h"

class FileNotFoundException : public HospitalException
{
public:

    FileNotFoundException();

    ~FileNotFoundException();
};

#endif
