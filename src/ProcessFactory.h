#pragma once
#include <vector>
#include <windows.h>
#include <string>
#include <iostream>
#include "Task.h"
#include <memory>

namespace ProcessFactory
{
	std::vector<std::unique_ptr<Task>> getProcessList();

	bool getPrivilege();

	bool compareProcess(const Task& a, const Task& b);

	bool killProcess(DWORD processID);
};
