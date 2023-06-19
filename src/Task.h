#pragma once
#include <Windows.h>
#include <memory>
#include "imgui.h"
#include "d3d11.h"

class Task
{
public:
	DWORD originalId;
	char* id;
	char* name;

	ID3D11ShaderResourceView* texturePointer = nullptr;

	Task();
	~Task();
};