#pragma once
#include <Windows.h>
#include <string>

namespace Utility
{
	char* wcharToChar(const wchar_t* wchar);
	char* numberToString(DWORD dw);
};

