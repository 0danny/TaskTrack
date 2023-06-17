#pragma once
#include <vector>
#include "ProcessHandler.h"
#include <chrono>

class Renderer
{
private:
	const wchar_t* windowName = L"Task Track";
	std::vector<Process> processList;

public:
	const int windowWidth = 700;
	const int windowHeight = 800;

	std::chrono::system_clock::time_point nextUpdateTime;

	Process currentSelected = {};

	Renderer();
	
	void renderUI();
	void applicationsTab();
	void processesTab();
	void preformanceTab();
	void setStyling();

	void drawTableRow(Process currentProcess);

	const wchar_t* getWindowName()
	{
		return windowName;
	}
};

