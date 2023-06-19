#pragma once
#include "Task.h"
#include <memory>
#include <vector>
#include <chrono>
#include "d3d11.h"

class ApplicationTab
{
private:
	std::vector<std::unique_ptr<Task>> processList;

	Task* selectedProcess;

	std::chrono::system_clock::time_point nextUpdateTime;

	int refreshTime = 5;

public:
	ApplicationTab();

	void renderTab();
	void createContextMenu();
	void drawTableRow(const std::unique_ptr<Task>& process);

	ID3D11Device* g_pd3dDevice;
};