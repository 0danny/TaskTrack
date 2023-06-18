#pragma once
#include <vector>
#include <chrono>
#include <memory>
#include "Process.h"
#include "ApplicationTab.h"

class Renderer
{
private:
	const wchar_t* windowName = L"Task Track";

public:
	const int windowWidth = 700;
	const int windowHeight = 800;

	std::unique_ptr<ApplicationTab> appRenderer;

	Renderer();
	
	void renderUI();
	void setStyling();

	const wchar_t* getWindowName()
	{
		return windowName;
	}
};

