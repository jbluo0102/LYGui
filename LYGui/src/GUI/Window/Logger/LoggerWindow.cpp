#include "pch.h"
#include "LoggerWindow.h"

ULoggerWindow::ULoggerWindow() :
	UImGuiWindow("Console", [this]() { RenderConsole(); })
{
}

void ULoggerWindow::RenderConsole()
{
	ImGui::Text("Filter:");
	ImGui::SameLine();

	static ImGuiTextFilter filter;
	ImGui::BeginChild("Filter", ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetFrameHeightWithSpacing()));
	filter.Draw("", ImGui::GetWindowWidth() - 60.f);
	ImGui::EndChild();

	ImGui::BeginChild("ConsoleContent", ImVec2(0, 0), true);

	for (const auto& [color, message] : ULogger::GetInstance().GetHistories())
	{
		if (filter.PassFilter(message.c_str()))
		{
			ImGui::TextColored(color, message.c_str());

			if (ImGui::GetScrollY() == ImGui::GetScrollMaxY())
			{
				ImGui::SetScrollHereY(1.0f);
			}
		}
	}

	ImGui::EndChild();
}