#include "ApplicationTab.h"
#include "ProcessFactory.h"
#include "imgui.h"
#include <thread>
#include <mutex>
#include <memory>
#include <shared_mutex>

ApplicationTab::ApplicationTab()
{
	ProcessFactory::getPrivilege();

	std::unique_lock<std::shared_mutex> lock(processListMutex);
	processList = ProcessFactory::getProcessList();

	// Start the thread on construction
	processListThread = std::thread(&ApplicationTab::processListUpdater, this);
}

ApplicationTab::~ApplicationTab()
{
	shouldUpdateList = false;

	if (processListThread.joinable()) {
		processListThread.join();
	}
}

void ApplicationTab::renderTab()
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

			//std::shared_lock<std::shared_mutex> lock(processListMutex);
			for (const auto& process : processList) {
				drawTableRow(process);
			}

			if (ImGui::BeginPopup("ApplicationsContext"))
			{
				ImGui::SeparatorText("Context Menu");

				//Buttons for ending process etc.
				if (ImGui::Selectable("Kill Process"))
				{
					//std::cout << "Killing process: " << currentSelected->id << std::endl;

					//bool killedProcess = ProcessFactory::killProcess(currentSelected->originalId);

					//std::cout << "Killed process: " << killedProcess << std::endl;
				}

				ImGui::EndPopup();
			}

			ImGui::EndTable();
		}

		ImGui::EndTabItem();
	}
}

void ApplicationTab::drawTableRow(const std::unique_ptr<Task>& currentProcess)
{
	ImGui::TableSetColumnIndex(0);

	bool isSelected = false;
	if (ImGui::Selectable(currentProcess->name, isSelected, ImGuiSelectableFlags_SpanAllColumns)) {}

	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
	{
		std::cout << "Right Clicking on : " << currentProcess->name << std::endl;
		ImGui::OpenPopup("ApplicationsContext");

		//currentSelected = currentProcess;
	}

	ImGui::TableSetColumnIndex(1);
	ImGui::Text(currentProcess->id);

	ImGui::TableSetColumnIndex(2);
	ImGui::Text("Testing.");

	ImGui::TableNextRow();
}

void ApplicationTab::processListUpdater()
{
	while (shouldUpdateList)
	{
		// Update process list
		std::unique_lock<std::shared_mutex> lock(processListMutex);
		processList = ProcessFactory::getProcessList();

		std::cout << "[Process Updater Thread]: Updating the list - " << processList.size() << " items." << std::endl;

		std::this_thread::sleep_for(std::chrono::seconds(refreshTime));
	}
}