#pragma once
#include <vector>
#include <windows.h>
#include <string>
#include <iostream>
#include "Task.h"
#include <memory>
#include "d3d11.h"

namespace ProcessFactory
{
	std::vector<std::unique_ptr<Task>> getProcessList(ID3D11Device* d3dDevice);

	bool getPrivilege();

	bool compareProcess(const Task& a, const Task& b);

	bool killProcess(DWORD processID);
};
