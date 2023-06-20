#include "ApplicationTab.h"
#include "ProcessFactory.h"
#include "imgui.h"
#include <memory>
#include "imgui_impl_dx11.h"
#include "Utility.h"
#include "TaskTrack.h"
#include <thread>
#include <format>

ApplicationTab::ApplicationTab() : isRunning(true)
{
	ProcessFactory::getPrivilege();

	processList = ProcessFactory::getProcessList(g_pd3dDevice);

	// Start the update thread
	updateThread = std::make_unique<std::thread>(&ApplicationTab::updateProcessList, this);
}

ApplicationTab::~ApplicationTab()
{
	// Stop the update thread
	isRunning = false;

	if (updateThread && updateThread->joinable())
	{
		updateThread->join();
	}
}

void ApplicationTab::updateProcessList()
{
	while (isRunning)
	{
		// Sleep for one second
		std::this_thread::sleep_for(std::chrono::seconds(refreshTime));

		// Update the processListBuffer
		std::vector<std::unique_ptr<Task>> newList = ProcessFactory::getProcessList(g_pd3dDevice);

		// Lock the mutex before accessing processList and processListBuffer
		std::lock_guard<std::mutex> lock(processListMutex);
		processList = std::move(newList);

		forceSort = true;
	}
}

void ApplicationTab::renderTab()
{
	if (ImGui::BeginTabItem("Applications"))
	{
		//Make the table view.
		ImGuiTableFlags flags = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter |
			ImGuiTableFlags_BordersV | ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Sortable;

		if (ImGui::BeginTable("ApplicationsTable", 3, flags))
		{
			ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
			ImGui::TableSetupColumn("Task");
			ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_PreferSortDescending);
			ImGui::TableSetupColumn("RAM");
			ImGui::TableHeadersRow();

			drawProcessList();

			createContextMenu();

			ImGui::EndTable();
		}

		ImGui::EndTabItem();
	}
}

void ApplicationTab::drawProcessList()
{
	ImGuiListClipper clipper;
	clipper.Begin(processList.size());

	// Lock the mutex before accessing processList
	std::lock_guard<std::mutex> lock(processListMutex);

	while (clipper.Step())
	{
		for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
		{
			// The process to be drawn.
			ImGui::TableNextRow();

			const auto& process = processList[i];

			drawTableRow(process);
		}
	}

	checkSorting();
}

void ApplicationTab::checkSorting()
{
	ImGuiTableSortSpecs* sort_specs = ImGui::TableGetSortSpecs();

	if (sort_specs)
	{
		if (sort_specs->SpecsDirty || forceSort)
		{
			std::cout << "Column being modified [" << sort_specs->Specs->ColumnIndex << "]" << " in direction: " << sort_specs->Specs->SortDirection << std::endl;

			sortColumns(sort_specs->Specs->ColumnIndex, sort_specs->Specs->SortDirection);

			sort_specs->SpecsDirty = false;

			forceSort = false;
		}
	}
}

void ApplicationTab::sortColumns(int columnIndex, int sortDirection)
{
	std::sort(processList.begin(), processList.end(), [&](const auto& lhs, const auto& rhs)
		{
			switch (columnIndex)
			{
			//Name Column
			case 0:
				return (sortDirection == 1) ? (lhs->name < rhs->name) : (lhs->name > rhs->name);
				break;
			//ID Column
			case 1:
				return (sortDirection == 1) ? (lhs->originalId < rhs->originalId) : (lhs->originalId > rhs->originalId);
				break;
			}
		});
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

	if (currentProcess->texturePointer != nullptr)
	{
		ImGui::Image(currentProcess->texturePointer, ImVec2(16, 16));  // Draw

		ImGui::SameLine();
	}

	bool isSelected = false;
	if (ImGui::Selectable(currentProcess->name.c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns)) {}

	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
	{
		selectedProcess = currentProcess.get();

		std::cout << "Right Clicking on : " << selectedProcess->name << std::endl;

		ImGui::OpenPopup("ApplicationsContext");
	}

	ImGui::TableSetColumnIndex(1);
	ImGui::Text(currentProcess->id.c_str());

	ImGui::TableSetColumnIndex(2);
	ImGui::Text("Testing.");
}