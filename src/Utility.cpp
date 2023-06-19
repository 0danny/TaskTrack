#include "Utility.h"
#include <string>
#include <stdlib.h>
#include <windows.h>
#include <d3d11.h>
#include <wincodec.h>  // For WIC
#include <wincodecsdk.h>  // For WIC
#include <comdef.h>  // For _com_error
#include <imgui.h>
#include <iostream>
#include <DXGIMessages.h>// Include the header file for DXGetErrorString
#pragma comment(lib, "windowscodecs.lib")  // Add this line to your code

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

std::wstring Utility::stringToWString(const std::string & s)
{
	int len;
	int slength = (int)s.length() + 1;

	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);

	std::wstring r(buf);
	delete[] buf;

	return r;
}

char* Utility::numberToString(DWORD dw)
{
	const int bufferSize = 32; // Maximum number of characters needed for a DWORD
	char* buffer = new char[bufferSize]; // Allocate buffer on the heap

	// Use snprintf instead of sprintf_s for safer string formatting
	std::snprintf(buffer, bufferSize, "%lu", dw);

	return buffer;
}

std::string Utility::getSystemRootDirectory() {
    TCHAR windowsDirectory[MAX_PATH];
    GetWindowsDirectory(windowsDirectory, MAX_PATH);

    // Convert from TCHAR (which is wchar_t on Unicode Windows) to std::string
    std::wstring wideWindowsDirectory(windowsDirectory);
    std::string systemRootDirectory(wideWindowsDirectory.begin(), wideWindowsDirectory.end());

    return systemRootDirectory;
}

// Convert LPWSTR to std::string
std::string Utility::LPWSTRtoString(LPWSTR param)
{
    std::wstring ws(param);

    std::string str(ws.begin(), ws.end());

    return str;
}

HICON Utility::getProcessIcon(const std::string& path) {
	SHFILEINFO shFileInfo;
	ZeroMemory(&shFileInfo, sizeof(SHFILEINFO));

	std::wstring widePath = stringToWString(path);

	SHGetFileInfo(
		widePath.c_str(),
		0,
		&shFileInfo,
		sizeof(SHFILEINFO),
		SHGFI_ICON | SHGFI_LARGEICON
	);

	return shFileInfo.hIcon;
}

std::vector<BYTE> Utility::getBitmapData(HBITMAP hBitmap, BITMAP& bmp)
{
    HDC hdc = GetDC(NULL);
    HDC memDC = CreateCompatibleDC(hdc);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = bmp.bmWidth;
    bmi.bmiHeader.biHeight = bmp.bmHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    int bytesPerRow = ((bmp.bmWidth * bmi.bmiHeader.biBitCount + 31) / 32) * 4;
    int bufferSize = bytesPerRow * bmp.bmHeight;

    std::vector<BYTE> buffer(bufferSize);

    int result = GetDIBits(memDC, hBitmap, 0, bmp.bmHeight, &buffer[0], &bmi, DIB_RGB_COLORS);

    if (result == 0)
    {
        // GetDIBits failed
        buffer.clear();
    }

    DeleteDC(memDC);
    ReleaseDC(NULL, hdc);

    return buffer;
}

ID3D11ShaderResourceView* Utility::iconToImGuiTexture(ID3D11Device* device, HICON hIcon)
{
    ICONINFO iconInfo;
    GetIconInfo(hIcon, &iconInfo);

    BITMAP bmp;
    int result = GetObject(iconInfo.hbmColor, sizeof(BITMAP), &bmp);

    std::vector<BYTE> buffer = getBitmapData(iconInfo.hbmColor, bmp);

    if (buffer.empty())
    {
        DeleteObject(iconInfo.hbmColor);
        DeleteObject(iconInfo.hbmMask);
        return nullptr;
    }

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = bmp.bmWidth;
    desc.Height = bmp.bmHeight;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData = {};

    initData.pSysMem = &buffer[0];
    initData.SysMemPitch = bmp.bmWidth * 4;  // The stride is 4 bytes because the format is B8G8R8A8

    ID3D11Texture2D* pTexture = nullptr;
    HRESULT hr = device->CreateTexture2D(&desc, &initData, &pTexture);

    if (FAILED(hr) || pTexture == nullptr)
    {
        std::cout << "The code: " << std::hex << hr << std::endl;

        // Failed to create the texture, cleanup and return
        DeleteObject(iconInfo.hbmColor);
        DeleteObject(iconInfo.hbmMask);
        return nullptr;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;

    ID3D11ShaderResourceView* pTextureView = nullptr;
    hr = device->CreateShaderResourceView(pTexture, &srvDesc, &pTextureView);

    pTexture->Release();

    if (FAILED(hr) || pTextureView == nullptr)
    {
        // Failed to create the shader resource view, cleanup and return
        DeleteObject(iconInfo.hbmColor);
        DeleteObject(iconInfo.hbmMask);
        return nullptr;
    }

    // Cleanup GDI objects
    DeleteObject(iconInfo.hbmColor);
    DeleteObject(iconInfo.hbmMask);

    // Return as ImTextureID
    return pTextureView;
}