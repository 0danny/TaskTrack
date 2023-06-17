#include "ProcessHandler.h"
#include <windows.h>
#include <psapi.h>
#include <vector>
#include <TlHelp32.h>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <tchar.h>
#include "Utility.h"
#include "imgui.h"
#include <algorithm>

bool ProcessHandler::getPrivilege()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tokenPrivileges;
	LUID luid;

	// Open the process token
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		std::cout << "Failed to open process token." << std::endl;
		return false;
	}

	// Lookup the privilege value
	if (!LookupPrivilegeValue(nullptr, SE_DEBUG_NAME, &luid))
	{
		std::cout << "Failed to lookup privilege value." << std::endl;
		CloseHandle(hToken);
		return false;
	}

	// Set up the token privileges structure
	tokenPrivileges.PrivilegeCount = 1;
	tokenPrivileges.Privileges[0].Luid = luid;
	tokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// Adjust the token privileges
	if (!AdjustTokenPrivileges(hToken, FALSE, &tokenPrivileges, sizeof(TOKEN_PRIVILEGES), nullptr, nullptr))
	{
		std::cout << "Failed to adjust token privileges." << std::endl;
		CloseHandle(hToken);
		return false;
	}

	// Check if the privilege was successfully enabled
	if (GetLastError() != ERROR_SUCCESS)
	{
		std::cout << "Failed to enable the privilege." << std::endl;
		CloseHandle(hToken);
		return false;
	}

	// Clean up and close the handle
	CloseHandle(hToken);

	std::cout << "Successfully enabled SeDebug privilege." << std::endl;

	return true;
}

std::vector<Process> ProcessHandler::getProcessList() {
	std::vector<Process> processList;

	HANDLE hndl = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS | TH32CS_SNAPMODULE, 0);
	if (hndl)
	{
		PROCESSENTRY32  process = { sizeof(PROCESSENTRY32) };
		Process32First(hndl, &process);
		do
		{
			Process info = {};

			std::string processIdString = std::to_string(process.th32ProcessID);

			info.originalId = process.th32ProcessID;

			info.id = new char[processIdString.length() + 1]; // allocate memory
			strcpy_s(info.id, processIdString.length() + 1, processIdString.c_str()); //Copy the ID into the heap allocated for info.id

			info.name = Utility::wcharToChar(process.szExeFile);

			processList.push_back(info);
		} while (Process32Next(hndl, &process));

		CloseHandle(hndl);
	}

	std::cout << "Collected " << processList.size() << " processes." << std::endl;

	// Sort the vector by the value field using the comparison function
	std::sort(processList.begin(), processList.end(), compareProcess);

	return processList;
}

//Used to sort the process list by ID.
bool ProcessHandler::compareProcess(const Process& a, const Process& b)
{
	return a.originalId < b.originalId;
}

bool ProcessHandler::killProcess(DWORD processID)
{
	const auto processHandle = OpenProcess(PROCESS_TERMINATE, false, processID);

	if (processHandle == 0)
	{
		std::cout << "Could not open a handle to process: " << processID << std::endl;

		return false;
	}

	bool result = TerminateProcess(processHandle, 1);

	CloseHandle(processHandle);

	return result;
}
