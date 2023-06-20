#pragma once
#include <Windows.h>
#include <memory>
#include "imgui.h"
#include "d3d11.h"
#include <string>

class Task
{
public:
	DWORD originalId;
	std::string id;
	std::string name;

	ID3D11ShaderResourceView* texturePointer = nullptr;

	Task();
	~Task();
};