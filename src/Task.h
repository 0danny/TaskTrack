#pragma once
#include <Windows.h>
#include <memory>

class Task
{
public:
	DWORD originalId;
	char* id;
	char* name;

	Task();
};