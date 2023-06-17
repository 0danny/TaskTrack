#include "Renderer.h"
#include "imgui.h"
#include <iostream>
#include <vector>
#include "ProcessHandler.h"
#include "TaskTrack.h"
#include <chrono>

Renderer::Renderer() : nextUpdateTime(std::chrono::system_clock::now())
{
	std::cout << "Renderer Constructor()" << std::endl;;

	ProcessHandler::getPrivilege();
	processList = ProcessHandler::getProcessList();
}

void Renderer::renderUI()
{
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);

	bool windowOpen = true;

	auto now = std::chrono::system_clock::now();
	if (now >= nextUpdateTime) {
		//Runs every second.
		std::cout << "Updating process list." << std::endl;

		processList.clear();

		processList = ProcessHandler::getProcessList();

		nextUpdateTime = now + std::chrono::seconds(5);
	}

	ImGui::Begin("Main Window", &windowOpen, windowFlags);

	if (ImGui::BeginTabBar("TabBar"))
	{
		applicationsTab();

		processesTab();

		preformanceTab();

		ImGui::EndTabBar();
	}

	//Show the demo window.
	//ImGui::ShowDemoWindow();

	ImGui::End();
}

void Renderer::applicationsTab()
{
	if (ImGui::BeginTabItem("Applications"))
	{
		//Make the table view.
		ImGuiTableFlags flags = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter |
			ImGuiTableFlags_BordersV | ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_ScrollY;

		if (ImGui::BeginTable("ApplicationsTable", 3, flags))
		{
			ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
			ImGui::TableSetupColumn("Task");
			ImGui::TableSetupColumn("ID");
			ImGui::TableSetupColumn("RAM");
			ImGui::TableHeadersRow();
			ImGui::TableNextRow();

			for (Process const& item : processList)
			{
				drawTableRow(item);
			}

			if (ImGui::BeginPopup("ApplicationsContext"))
			{
				ImGui::SeparatorText("Context Menu");

				//Buttons for ending process etc.
				if (ImGui::Selectable("Kill Process"))
				{
					std::cout << "Killing process: " << currentSelected.id << std::endl;

					bool killedProcess = ProcessHandler::killProcess(currentSelected.originalId);

					std::cout << "Killed process: " << killedProcess << std::endl;
				}

				ImGui::EndPopup();
			}

			ImGui::EndTable();
		}

		ImGui::EndTabItem();
	}
}

void Renderer::drawTableRow(Process currentProcess)
{
	ImGui::TableSetColumnIndex(0);

	bool isSelected = false;
	if (ImGui::Selectable(currentProcess.name, isSelected, ImGuiSelectableFlags_SpanAllColumns)) {}

	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
	{
		std::cout << "Right Clicking on : " << currentProcess.name << std::endl;
		ImGui::OpenPopup("ApplicationsContext");

		currentSelected = currentProcess;
	}

	ImGui::TableSetColumnIndex(1);
	ImGui::Text(currentProcess.id);

	ImGui::TableSetColumnIndex(2);
	ImGui::Text("Testing.");

	ImGui::TableNextRow();
}


void Renderer::processesTab()
{
	if (ImGui::BeginTabItem("Processes"))
	{
		ImGui::Text("This is the Avocado tab!\nblah blah blah blah blah");


		ImGui::EndTabItem();
	}
}

void Renderer::preformanceTab()
{
	if (ImGui::BeginTabItem("Preformance"))
	{
		ImGui::Text("This is the Avocado tab!\nblah blah blah blah blah");

		ImGui::EndTabItem();
	}
}

void Renderer::setStyling()
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(2.0f);  // Scale all sizes by a factor, 1.5f for example
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);  // Center title text when larger

	style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
	style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	style.GrabRounding = style.FrameRounding = 2.3f;
}