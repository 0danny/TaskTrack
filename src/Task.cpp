#include "Task.h"
#include <iostream>
#include "d3d11.h"
#include "imgui.h"

Task::Task() : id(), name(), originalId() {}

Task::~Task()
{
	if (texturePointer != nullptr)
	{
		texturePointer->Release();
	}

	delete[] name;
	delete[] id;
}