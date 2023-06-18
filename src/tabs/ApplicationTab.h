#pragma once
#include "Task.h"
#include <memory>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include <shared_mutex>

class ApplicationTab
{
private:
	std::thread processListThread;
	std::vector<std::unique_ptr<Task>> processList;
	std::shared_mutex processListMutex;

	int refreshTime = 5;
	bool shouldUpdateList = true;

	void processListUpdater();

public:
	ApplicationTab();
	~ApplicationTab();

	void renderTab();
	void drawTableRow(const std::unique_ptr<Task>& process);
};