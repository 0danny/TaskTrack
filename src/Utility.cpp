#include "Utility.h"
#include <string>
#include <stdlib.h>
#include <windows.h>

char* Utility::wcharToChar(const wchar_t* wstr)
{
    if (wstr == nullptr)
        return nullptr;

    size_t convertedChars = 0;
    size_t bufferCount = wcslen(wstr) + 1;

    char* nameBuffer = new char[bufferCount]; // allocate the buffer on the heap

    wcstombs_s(&convertedChars, nameBuffer, bufferCount, wstr, bufferCount - 1);

    return nameBuffer;
}

char* Utility::numberToString(DWORD dw)
{
    const int bufferSize = 32; // Maximum number of characters needed for a DWORD
    char* buffer = new char[bufferSize]; // Allocate buffer on the heap

    // Use snprintf instead of sprintf_s for safer string formatting
    std::snprintf(buffer, bufferSize, "%lu", dw);

    return buffer;
}