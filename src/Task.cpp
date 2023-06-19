#include "Task.h"
#include <iostream>

Task::Task() : id(), name(), originalId() {}

Task::~Task()
{
	if(icon != nullptr)
		DestroyIcon(icon);
}