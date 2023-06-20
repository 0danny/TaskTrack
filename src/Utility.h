#pragma once
#include <Windows.h>
#include <string>
#include "d3d11.h"
#include <imgui.h>
#include <vector>

namespace Utility
{
	std::string wcharToChar(const wchar_t* wstr);

	HICON getProcessIcon(const std::string& path);

	std::wstring stringToWString(const std::string& s);

	ID3D11ShaderResourceView* iconToImGuiTexture(ID3D11Device* device, HICON hIcon);

	std::vector<BYTE> getBitmapData(HBITMAP hBitmap, BITMAP& bmp);

	std::string LPWSTRtoString(LPWSTR param);

	std::string getSystemRootDirectory();
};

