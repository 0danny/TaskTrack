#pragma once
#include <vector>
#include <chrono>
#include <memory>
#include "Process.h"
#include "ApplicationTab.h"
#include "d3d11.h"

class Renderer
{
private:
	const wchar_t* windowName = L"Task Track";

public:
	const int windowWidth = 700;
	const int windowHeight = 800;

	ID3D11Device* g_pd3dDevice;

	std::unique_ptr<ApplicationTab> appRenderer;

	Renderer();
	
	void renderUI();
	void setStyling();

	void setDevice(ID3D11Device* g_pd3dDevice);

	const wchar_t* getWindowName()
	{
		return windowName;
	}
};

