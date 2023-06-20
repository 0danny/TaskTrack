#pragma once
#include "Task.h"
#include <memory>
#include <vector>
#include <chrono>
#include "d3d11.h"
#include <mutex>
#include <atomic>

class ApplicationTab
{
private:
	Task* selectedProcess;
	int refreshTime = 5;

	std::mutex processListMutex;
	std::atomic_bool isRunning;
	std::atomic_bool forceSort;
	std::unique_ptr<std::thread> updateThread;

	std::vector<std::unique_ptr<Task>> processList; // used for reading (GUI thread)

public:
	//Constructor & Deconstructor
	ApplicationTab();
	~ApplicationTab();

	void renderTab();
	void createContextMenu();
	void updateProcessList();
	void drawProcessList();
	void drawTableRow(const std::unique_ptr<Task>& process);

	//Sorting Functions
	void checkSorting();

	//Per Column Sorting
	void sortColumns(int columnIndex, int sortDirection);

	ID3D11Device* g_pd3dDevice;
};