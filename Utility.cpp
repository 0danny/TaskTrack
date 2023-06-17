#include "Utility.h"
#include <string>
#include <stdlib.h>
#include <windows.h>

char* Utility::wcharToChar(const wchar_t* wstr)
{
    if (wstr == nullptr)
        return nullptr;

    size_t len = wcslen(wstr) + 1;
    char* str = new char[len];

    size_t convertedChars = 0;
    wcstombs_s(&convertedChars, str, len, wstr, _TRUNCATE);

    return str;
}