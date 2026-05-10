#include "FileHandler.h"
#include "FileNotFoundException.h"

#include <iostream>
#include <fstream>
#include <cstdio>
#include <ctime>



int toInt(const char* text)
{
    int number = 0;
    int index = 0;

    while (text[index] != '\0')
    {
        number = number * 10 + (text[index] - '0');
        index++;
    }

    return number;
}

float toFloat(const char* text)
{
    float number = 0;
    int index = 0;

    while (text[index] != '\0' && text[index] != '.')
    {
        number = number * 10 + (text[index] - '0');
        index++;
    }

    if (text[index] == '.')
    {
        index++;

        float place = 0.1f;

        while (text[index] != '\0')
        {
            number = number + (text[index] - '0') * place;
            place = place / 10;
            index++;
        }
    }

    return number;
}

int FileHandler::getNextIDFromFile(const char* fileName)
{
    std::ifstream file(fileName);

    if (!file)
    {
        throw FileNotFoundException();
    }

    int maxID = 0;

    char line[1000];

    while (file.getline(line, 1000))
    {
        if (line[0] == '\0')
        {
            continue;
        }

        char fields[10][200];

        int fieldCount = 0;

        splitLine(line, fields, fieldCount);

        int id = toInt(fields[0]);

        if (id > maxID)
        {
            maxID = id;
        }
    }

    file.close();

    return maxID + 1;
}


FileHandler::FileHandler()
{
    patientsFile = "DataFiles/patients.txt";
    doctorsFile = "DataFiles/doctors.txt";
    adminFile = "DataFiles/admin.txt";
    appointmentsFile = "DataFiles/appointments.txt";
    billsFile = "DataFiles/bills.txt";
    prescriptionsFile = "DataFiles/prescriptions.txt";
    securityLogFile = "DataFiles/security_log.txt";
    dischargedFile = "DataFiles/discharged.txt";
}

void FileHandler::splitLine(const char* line, char fields[][200], int& fieldCount)
{
    fieldCount = 0;

    int row = 0;
    int col = 0;
    int index = 0;

    while (line[index] != '\0')
    {
        if (line[index] == ',')
        {
            fields[row][col] = '\0';

            row++;
            col = 0;
        }
        else
        {
            fields[row][col] = line[index];
            col++;
        }

        index++;
    }

    fields[row][col] = '\0';

    fieldCount = row + 1;
}

void FileHandler::loadPatients(Storage<Patient>& patients)
{
    std::ifstream file(patientsFile);

    if (!file)
    {
        throw FileNotFoundException();
    }

    char line[500];

    while (file.getline(line, 500))
    {
        if (line[0] == '\0')
        {
            continue;
        }

        char fields[10][200];
        int fieldCount = 0;

        splitLine(line, fields, fieldCount);

        if (fieldCount == 7)
        {
            int id = toInt(fields[0]);
            int age = toInt(fields[2]);
            char gender = fields[3][0];
            float balance = toFloat(fields[6]);

            Patient p(id, fields[1], fields[5], age, gender, fields[4], balance);

            patients.add(p);
        }
    }

    file.close();
}




void FileHandler::loadDoctors(Storage<Doctor>& doctors)
{
    std::ifstream file(doctorsFile);

    if (!file)
    {
        throw FileNotFoundException();
    }

    char line[500];

    while (file.getline(line, 500))
    {
        if (line[0] == '\0')
        {
            continue;
        }

        char fields[10][200];
        int fieldCount = 0;

        splitLine(line, fields, fieldCount);

        if (fieldCount == 6)
        {
            int id = toInt(fields[0]);

            float fee = toFloat(fields[5]);

            Doctor d(id, fields[1], fields[4], fields[2], fields[3], fee);

            doctors.add(d);
        }
    }

    file.close();
}



void FileHandler::loadAdmins(Storage<Admin>& admins)
{
    std::ifstream file(adminFile);

    if (!file)
    {
        throw FileNotFoundException();
    }

    char line[300];

    while (file.getline(line, 300))
    {
        if (line[0] == '\0')
        {
            continue;
        }

        char fields[5][200];
        int fieldCount = 0;

        splitLine(line, fields, fieldCount);

        if (fieldCount == 4 || fieldCount == 3)
        {
            int id = toInt(fields[0]);

            Admin a(id, fields[1], fields[2], fields[3]);

            admins.add(a);
        }
    }

    file.close();
}



void FileHandler::loadAppointments(Storage<Appointment>& appointments)
{
    std::ifstream file(appointmentsFile);

    if (!file)
    {
        throw FileNotFoundException();
    }

    char line[500];

    while (file.getline(line, 500))
    {
        if (line[0] == '\0')
        {
            continue;
        }

        char fields[10][200];

        int fieldCount = 0;

        splitLine(line, fields, fieldCount);

        if (fieldCount == 6)
        {
            int appointmentID = toInt(fields[0]);

            int patientID = toInt(fields[1]);

            int doctorID = toInt(fields[2]);

            Appointment a(appointmentID, patientID, doctorID, fields[3], fields[4], fields[5]);

            appointments.add(a);
        }
    }

    file.close();
}


void FileHandler::loadBill(Storage<Bill>& bills)
{
    std::ifstream file(billsFile);

    if (!file)
    {
        throw FileNotFoundException();
    }

    char line[500];

    while (file.getline(line, 500))
    {
        if (line[0] == '\0')
        {
            continue;
        }

        char fields[10][200];

        int fieldCount = 0;

        splitLine(line, fields, fieldCount);

        if (fieldCount == 6)
        {
            int billID = toInt(fields[0]);

            int patientID = toInt(fields[1]);

            int appointmentID = toInt(fields[2]);

            float amount = toFloat(fields[3]);

            Bill b(billID, patientID, appointmentID, amount, fields[4], fields[5]);

            bills.add(b);
        }
    }

    file.close();
}



void FileHandler::loadPrescriptions(Storage<Prescription>& prescriptions)
{
    std::ifstream file(prescriptionsFile);

    if (!file)
    {
        throw FileNotFoundException();
    }

    char line[1000];

    while (file.getline(line, 1000))
    {
        if (line[0] == '\0')
        {
            continue;
        }

        char fields[10][200];

        int fieldCount = 0;

        splitLine(line, fields, fieldCount);

        if (fieldCount == 7)
        {
            int prescriptionID = toInt(fields[0]);

            int patientID = toInt(fields[1]);

            int doctorID = toInt(fields[2]);

            int appointmentID = toInt(fields[3]);

            Prescription p(prescriptionID, patientID, doctorID, appointmentID, fields[4], fields[5], fields[6]);

            prescriptions.add(p);
        }
    }

    file.close();
}


// Appendd Functions

void FileHandler::appendPatient(const Patient& p)
{
    std::ofstream file(patientsFile, std::ios::app);

    if (!file)
    {
        throw FileNotFoundException();
    }

    file << p.getID() << "," << p.getName() << "," << p.getAge() << "," << p.getGender() << "," << p.getContact() << "," << p.getPassword() << "," << p.getBalance() << std::endl;

    file.close();
}



void FileHandler::appendDoctor(const Doctor& d)
{
    std::ofstream file(doctorsFile, std::ios::app);

    if (!file)
    {
        throw FileNotFoundException();
    }

    file << d.getID() << "," << d.getName() << "," << d.getSpecialization() << "," << d.getContact() << "," << d.getPassword() << "," << d.getFee() << std::endl;

    file.close();
}



void FileHandler::appendAdmin(const Admin& a)
{
    std::ofstream file(adminFile, std::ios::app);

    if (!file)
    {
        throw FileNotFoundException();
    }

    file << a.getID() << "," << a.getName() << "," << a.getPassword() << "," << a.getRole() << std::endl;

    file.close();
}


void FileHandler::appendAppointment(const Appointment& a)
{
    std::ofstream file(appointmentsFile, std::ios::app);

    if (!file)
    {
        throw FileNotFoundException();
    }

    file << a.getAppointmentID() << "," << a.getPatientID() << "," << a.getDoctorID() << "," << a.getDate() << "," << a.getTime() << "," << a.getStatus() << std::endl;

    file.close();
}

void FileHandler::appendBill(const Bill& b)
{
    std::ofstream file(billsFile, std::ios::app);

    if (!file)
    {
        throw FileNotFoundException();
    }

    file << b.getBillID() << "," << b.getPatientID() << "," << b.getAppointmentID() << "," << b.getAmount() << "," << b.getStatus() << "," << b.getDate() << std::endl;

    file.close();
}

void FileHandler::appendPrescription(const Prescription& p)
{
    std::ofstream file(prescriptionsFile, std::ios::app);

    if (!file)
    {
        throw FileNotFoundException();
    }

    file << p.getPrescriptionID() << "," << p.getPatientID() << "," << p.getDoctorID() << "," << p.getAppointmentID() << "," << p.getMedicines() << "," << p.getNotes() << "," << p.getDate() << std::endl;

    file.close();
}

// UPDATE FUNCTIONS

void FileHandler::updatePatient(const Patient& updatedPatient)
{
    std::ifstream inFile(patientsFile);

    if (!inFile)
    {
        throw FileNotFoundException();
    }

    std::ofstream outFile("DataFiles/temp.txt");

    if (!outFile)
    {
        throw FileNotFoundException();
    }

    char line[500];

    while (inFile.getline(line, 500))
    {
        if (line[0] == '\0')
        {
            continue;
        }

        char fields[10][200];

        int fieldCount = 0;

        splitLine(line, fields, fieldCount);

        int id = toInt(fields[0]);

        if (id == updatedPatient.getID())
        {
            outFile << updatedPatient.getID() << ","
                << updatedPatient.getName() << ","
                << updatedPatient.getAge() << ","
                << updatedPatient.getGender() << ","
                << updatedPatient.getContact() << ","
                << updatedPatient.getPassword() << ","
                << updatedPatient.getBalance() << std::endl;
        }
        else
        {
            outFile << line << std::endl;
        }
    }

    inFile.close();

    outFile.close();

    remove(patientsFile);

    (void)rename("DataFiles/temp.txt", patientsFile);
}

void FileHandler::updateDoctor(const Doctor& updatedDoctor)
{
    std::ifstream inFile(doctorsFile);

    if (!inFile)
    {
        throw FileNotFoundException();
    }

    std::ofstream outFile("DataFiles/temp.txt");

    if (!outFile)
    {
        throw FileNotFoundException();
    }

    char line[500];

    while (inFile.getline(line, 500))
    {
        if (line[0] == '\0')
        {
            continue;
        }

        char fields[10][200];

        int fieldCount = 0;

        splitLine(line, fields, fieldCount);

        int id = toInt(fields[0]);

        if (id == updatedDoctor.getID())
        {
            outFile << updatedDoctor.getID() << ","
                << updatedDoctor.getName() << ","
                << updatedDoctor.getSpecialization() << ","
                << updatedDoctor.getContact() << ","
                << updatedDoctor.getPassword() << ","
                << updatedDoctor.getFee() << std::endl;
        }
        else
        {
            outFile << line << std::endl;
        }
    }

    inFile.close();

    outFile.close();

    remove(doctorsFile);

    (void)rename("DataFiles/temp.txt", doctorsFile);
}

void FileHandler::updateAppointment(const Appointment& updatedAppointment)
{
    std::ifstream inFile(appointmentsFile);

    if (!inFile)
    {
        throw FileNotFoundException();
    }

    std::ofstream outFile("DataFiles/temp.txt");

    if (!outFile)
    {
        throw FileNotFoundException();
    }

    char line[500];

    while (inFile.getline(line, 500))
    {
        if (line[0] == '\0')
        {
            continue;
        }

        char fields[10][200];

        int fieldCount = 0;

        splitLine(line, fields, fieldCount);

        int id = toInt(fields[0]);

        if (id == updatedAppointment.getAppointmentID())
        {
            outFile << updatedAppointment.getAppointmentID() << ","
                << updatedAppointment.getPatientID() << ","
                << updatedAppointment.getDoctorID() << ","
                << updatedAppointment.getDate() << ","
                << updatedAppointment.getTime() << ","
                << updatedAppointment.getStatus() << std::endl;
        }
        else
        {
            outFile << line << std::endl;
        }
    }

    inFile.close();

    outFile.close();

    remove(appointmentsFile);

    (void)rename("DataFiles/temp.txt", appointmentsFile);
}


void FileHandler::updateBill(const Bill& updatedBill)
{
    std::ifstream inFile(billsFile);

    if (!inFile)
    {
        throw FileNotFoundException();
    }

    std::ofstream outFile("DataFiles/temp.txt");

    if (!outFile)
    {
        throw FileNotFoundException();
    }

    char line[500];

    while (inFile.getline(line, 500))
    {
        if (line[0] == '\0')
        {
            continue;
        }

        char fields[10][200];

        int fieldCount = 0;

        splitLine(line, fields, fieldCount);

        int id = toInt(fields[0]);

        if (id == updatedBill.getBillID())
        {
            outFile << updatedBill.getBillID() << ","
                << updatedBill.getPatientID() << ","
                << updatedBill.getAppointmentID() << ","
                << updatedBill.getAmount() << ","
                << updatedBill.getStatus() << ","
                << updatedBill.getDate() << std::endl;
        }
        else
        {
            outFile << line << std::endl;
        }
    }

    inFile.close();

    outFile.close();

    remove(billsFile);

    (void)rename("DataFiles/temp.txt", billsFile);
}


void FileHandler::updatePrescription(const Prescription& updatedPrescription)
{
    std::ifstream inFile(prescriptionsFile);

    if (!inFile)
    {
        throw FileNotFoundException();
    }

    std::ofstream outFile("DataFiles/temp.txt");

    if (!outFile)
    {
        throw FileNotFoundException();
    }

    char line[1000];

    while (inFile.getline(line, 1000))
    {
        if (line[0] == '\0')
        {
            continue;
        }

        char fields[10][200];

        int fieldCount = 0;

        splitLine(line, fields, fieldCount);

        int id = toInt(fields[0]);

        if (id == updatedPrescription.getPrescriptionID())
        {
            outFile << updatedPrescription.getPrescriptionID() << ","
                << updatedPrescription.getPatientID() << ","
                << updatedPrescription.getDoctorID() << ","
                << updatedPrescription.getAppointmentID() << ","
                << updatedPrescription.getMedicines() << ","
                << updatedPrescription.getNotes() << ","
                << updatedPrescription.getDate() << std::endl;
        }
        else
        {
            outFile << line << std::endl;
        }
    }

    inFile.close();

    outFile.close();

    remove(prescriptionsFile);

    (void)rename("DataFiles/temp.txt", prescriptionsFile);
}


// DELEte FUNCTIOSN

void FileHandler::deletePatientByID(int deleteID)
{
    std::ifstream inFile(patientsFile);

    if (!inFile)
    {
        throw FileNotFoundException();
    }

    std::ofstream outFile("DataFiles/temp.txt");

    if (!outFile)
    {
        throw FileNotFoundException();
    }

    char line[500];

    while (inFile.getline(line, 500))
    {
        if (line[0] == '\0')
        {
            continue;
        }

        char fields[10][200];

        int fieldCount = 0;

        splitLine(line, fields, fieldCount);

        int id = toInt(fields[0]);

        if (id != deleteID)
        {
            outFile << line << std::endl;
        }
    }

    inFile.close();

    outFile.close();

    remove(patientsFile);

    (void)rename("DataFiles/temp.txt", patientsFile);
}


void FileHandler::deleteDoctorByID(int deleteID)
{
    std::ifstream inFile(doctorsFile);

    if (!inFile)
    {
        throw FileNotFoundException();
    }

    std::ofstream outFile("DataFiles/temp.txt");

    if (!outFile)
    {
        throw FileNotFoundException();
    }

    char line[500];

    while (inFile.getline(line, 500))
    {
        if (line[0] == '\0')
        {
            continue;
        }

        char fields[10][200];

        int fieldCount = 0;

        splitLine(line, fields, fieldCount);

        int id = toInt(fields[0]);

        if (id != deleteID)
        {
            outFile << line << std::endl;
        }
    }

    inFile.close();

    outFile.close();

    remove(doctorsFile);

    (void)rename("DataFiles/temp.txt", doctorsFile);
}

void FileHandler::deleteAppointmentByID(int deleteID)
{
    std::ifstream inFile(appointmentsFile);

    if (!inFile)
    {
        throw FileNotFoundException();
    }

    std::ofstream outFile("DataFiles/temp.txt");

    if (!outFile)
    {
        throw FileNotFoundException();
    }

    char line[500];

    while (inFile.getline(line, 500))
    {
        if (line[0] == '\0')
        {
            continue;
        }

        char fields[10][200];

        int fieldCount = 0;

        splitLine(line, fields, fieldCount);

        int id = toInt(fields[0]);

        if (id != deleteID)
        {
            outFile << line << std::endl;
        }
    }

    inFile.close();

    outFile.close();

    remove(appointmentsFile);

    (void)rename("DataFiles/temp.txt", appointmentsFile);
}



void FileHandler::deleteBillByID(int deleteID)
{
    std::ifstream inFile(billsFile);

    if (!inFile)
    {
        throw FileNotFoundException();
    }

    std::ofstream outFile("DataFiles/temp.txt");

    if (!outFile)
    {
        throw FileNotFoundException();
    }

    char line[500];

    while (inFile.getline(line, 500))
    {
        if (line[0] == '\0')
        {
            continue;
        }

        char fields[10][200];

        int fieldCount = 0;

        splitLine(line, fields, fieldCount);

        int id = toInt(fields[0]);

        if (id != deleteID)
        {
            outFile << line << std::endl;
        }
    }

    inFile.close();

    outFile.close();

    remove(billsFile);

    (void)rename("DataFiles/temp.txt", billsFile);
}



void FileHandler::deletePrescriptionByID(int deleteID)
{
    std::ifstream inFile(prescriptionsFile);

    if (!inFile)
    {
        throw FileNotFoundException();
    }

    std::ofstream outFile("DataFiles/temp.txt");

    if (!outFile)
    {
        throw FileNotFoundException();
    }

    char line[1000];

    while (inFile.getline(line, 1000))
    {
        if (line[0] == '\0')
        {
            continue;
        }

        char fields[10][200];

        int fieldCount = 0;

        splitLine(line, fields, fieldCount);

        int id = toInt(fields[0]);

        if (id != deleteID)
        {
            outFile << line << std::endl;
        }
    }

    inFile.close();

    outFile.close();

    remove(prescriptionsFile);

    (void)rename("DataFiles/temp.txt", prescriptionsFile);
}

// Get Next ID Functions

int FileHandler::getNextPatientID()
{
    return getNextIDFromFile(patientsFile);
}

int FileHandler::getNextDoctorID()
{
    return getNextIDFromFile(doctorsFile);
}

int FileHandler::getNextAppointmentID()
{
    return getNextIDFromFile(appointmentsFile);
}

int FileHandler::getNextBillID()
{
    return getNextIDFromFile(billsFile);
}

int FileHandler::getNextPrescriptionID()
{
    return getNextIDFromFile(prescriptionsFile);
}

void FileHandler::writeSecurityLog(const char* role, int enteredID, const char* result)
{
    std::ofstream file(securityLogFile, std::ios::app);

    if (!file)
    {
        throw FileNotFoundException();
    }

    time_t now = time(0);

    tm localTime;

    localtime_s(&localTime, &now);

    char timestamp[30];

    strftime(timestamp, 30, "%d-%m-%Y %H:%M:%S", &localTime);

    file << timestamp << "," << role << "," << enteredID << "," << result << std::endl;

    file.close();
}

void FileHandler::viewSecurityLog() const
{
    std::ifstream file(securityLogFile);

    if (!file)
    {
        throw FileNotFoundException();
    }

    char line[500];

    bool empty = true;

    while (file.getline(line, 500))
    {
        if (line[0] != '\0')
        {
            std::cout << line << std::endl;

            empty = false;
        }
    }

    if (empty)
    {
        std::cout << "No security events logged." << std::endl;
    }

    file.close();
}

void FileHandler::appendDischargedPatient(const Patient& p)
{
    std::ofstream file(dischargedFile, std::ios::app);

    if (!file)
    {
        throw FileNotFoundException();
    }

    file << p.getID() << "," << p.getName() << "," << p.getAge() << "," << p.getGender() << "," << p.getContact() << "," << p.getPassword() << "," << p.getBalance() << std::endl;

    file.close();
}











FileHandler::~FileHandler()
{

}

