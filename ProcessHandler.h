#pragma once
#include <vector>
#include <windows.h>
#include <string>
#include <iostream>

struct Process
{
    DWORD originalId;
    char* id;
    char* name;

    ~Process()
    {
        //Free the id & name.
        //delete[] id;
        // Only delete name if it was dynamically allocated
        if (name != nullptr) {
            delete[] name;
        }
    }
};

namespace ProcessHandler
{
    std::vector<Process> getProcessList();
    
    bool getPrivilege();

    bool compareProcess(const Process& a, const Process& b);

    bool killProcess(DWORD processID);
};