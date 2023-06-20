#include "ProcessFactory.h"
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
#include "Process.h"
#include <memory>

bool ProcessFactory::getPrivilege()
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

/*
	NOTE

	I am well aware that this function is extremely expensive, and ontop of that it runs
	on the UI thread. However, keep in mind that this is more of a PoC as a learn more c++
	and become better at it over time.
*/
std::vector<std::unique_ptr<Task>> ProcessFactory::getProcessList(ID3D11Device* d3dDevice) {
	std::vector<std::unique_ptr<Task>> processList;

	HANDLE hndl = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS | TH32CS_SNAPMODULE, 0);
	if (hndl)
	{
		PROCESSENTRY32  process = { sizeof(PROCESSENTRY32) };
		Process32First(hndl, &process);
		do
		{
			std::unique_ptr<Task> info = std::make_unique<Task>();

			info->name = Utility::wcharToChar(process.szExeFile); // Assign the resulting char* to the info object's name field
			info->id = std::to_string(process.th32ProcessID);
			info->originalId = process.th32ProcessID;

			if (d3dDevice != nullptr)
			{
				HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, process.th32ProcessID);
				if (hProcess != NULL) {
					wchar_t processPath[MAX_PATH];
					if (GetModuleFileNameEx(hProcess, NULL, processPath, sizeof(processPath) / sizeof(wchar_t)))
					{
						std::string pPath = Utility::LPWSTRtoString(processPath);

						HICON icon = Utility::getProcessIcon(pPath);

						//d3dDevice
						info->texturePointer = Utility::iconToImGuiTexture(d3dDevice, icon);

						DestroyIcon(icon);
					}
					else
					{
						std::cout << "GetModuleFileNameEx failed with error code " << GetLastError() << std::endl;
					}

					CloseHandle(hProcess);
				}
				else
				{
					std::string systemRoot = Utility::getSystemRootDirectory();
					std::string defaultIconPath = systemRoot + "\\System32\\svchost.exe";

					//std::cout << "Could not call OpenProcess() on: " << info->name << " | Fallback: " << defaultIconPath << std::endl;

					HICON icon = Utility::getProcessIcon(defaultIconPath); // Assign fallback icon

					//d3dDevice
					info->texturePointer = Utility::iconToImGuiTexture(d3dDevice, icon);

					DestroyIcon(icon);
				}
			}

			processList.push_back(std::move(info));
		} while (Process32Next(hndl, &process));

		CloseHandle(hndl);
	}

	std::cout << "Collected " << processList.size() << " processes." << std::endl;

	return processList;
}


//Used to sort the process list by ID.
bool ProcessFactory::compareProcess(const Task& a, const Task& b)
{
	return a.originalId < b.originalId;
}

bool ProcessFactory::killProcess(DWORD processID)
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
