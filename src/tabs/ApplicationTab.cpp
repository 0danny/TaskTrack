#include "ApplicationTab.h"
#include "ProcessFactory.h"
#include "imgui.h"
#include <memory>
#include "imgui_impl_dx11.h"
#include "Utility.h"
#include "TaskTrack.h"

ApplicationTab::ApplicationTab()
{
	ProcessFactory::getPrivilege();

	processList = ProcessFactory::getProcessList();
}

void ApplicationTab::renderTab()
{
	auto now = std::chrono::system_clock::now();

	if (now >= nextUpdateTime) {
		//Runs every second.
		std::cout << "Updating process list." << std::endl;

		processList = ProcessFactory::getProcessList();

		nextUpdateTime = now + std::chrono::seconds(refreshTime);
	}

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

			ImGuiListClipper clipper;
			clipper.Begin(processList.size());

			while (clipper.Step())
			{
				for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
				{
					// The process to be drawn.
					const auto& process = processList[i];

					ImGui::TableNextRow();
					
					drawTableRow(process);
				}
			}

			createContextMenu();

			ImGui::EndTable();
		}

		ImGui::EndTabItem();
	}
}

void ApplicationTab::createContextMenu()
{
	if (ImGui::BeginPopup("ApplicationsContext"))
	{
		//Buttons for ending process etc.
		if (ImGui::Selectable("Kill Process"))
		{
			bool killedProcess = ProcessFactory::killProcess(selectedProcess->originalId);

			std::cout << "Killing process: " << selectedProcess->id << " | Result -> " << killedProcess << std::endl;
		}

		ImGui::EndPopup();
	}
}

void ApplicationTab::drawTableRow(const std::unique_ptr<Task>& currentProcess)
{
	ImGui::TableSetColumnIndex(0);

	if (currentProcess->icon != NULL)
	{
		ImTextureID texture = Utility::iconToImGuiTexture(g_pd3dDevice, currentProcess->icon);
		if (texture != nullptr)
		{
			ImGui::Image(texture, ImVec2(16, 16));  // Draw
		}

		ImGui::SameLine();
	}

	bool isSelected = false;
	if (ImGui::Selectable(currentProcess->name, isSelected, ImGuiSelectableFlags_SpanAllColumns)) {}

	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
	{
		selectedProcess = currentProcess.get();

		std::cout << "Right Clicking on : " << selectedProcess->name << std::endl;

		ImGui::OpenPopup("ApplicationsContext");
	}

	ImGui::TableSetColumnIndex(1);
	ImGui::Text(currentProcess->id);

	ImGui::TableSetColumnIndex(2);
	ImGui::Text("Testing.");
}