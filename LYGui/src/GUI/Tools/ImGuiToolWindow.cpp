#include "pch.h"
#include "ImGuiToolWindow.h"

#include <GUI/Window/ImGuiWindow.h>

UImGuiToolWindow::UImGuiToolWindow(const std::string& Title, bool Open)
	: m_Title(Title), m_Open(Open)
{
	m_ToolWindow = std::make_unique<UImGuiWindow>(m_Title, [this]() { RenderTool(); });
	m_DetailWindow = std::make_unique<UImGuiWindow>(std::format("Details[{}]", m_Title), [this]() { RenderDetail(); });
	m_ViewportWindow = std::make_unique<UImGuiWindow>(std::format("Viewport[{}]", m_Title), [this]() { RenderViewport(); });
}

UImGuiToolWindow::~UImGuiToolWindow()
{
}

void UImGuiToolWindow::Render()
{
	if (m_Open)
	{
		m_ToolWindow->Render();
		m_DetailWindow->Render();
		m_ViewportWindow->Render();
	}
}

void UImGuiToolWindow::SetOpen(bool Open)
{
	m_Open = Open;
	SetToolOpen(Open);
	SetDetailOpen(Open);
	SetViewportOpen(Open);
}

void UImGuiToolWindow::SetToolOpen(bool Open)
{
	m_ToolWindow->SetOpen(Open);
}

void UImGuiToolWindow::SetDetailOpen(bool Open)
{
	m_DetailWindow->SetOpen(Open);
}

void UImGuiToolWindow::SetViewportOpen(bool Open)
{
	m_ViewportWindow->SetOpen(Open);
}

bool& UImGuiToolWindow::GetOpenRef()
{
	return m_Open;
}

bool UImGuiToolWindow::IsOpen() const
{
	return m_Open;
}

std::string UImGuiToolWindow::GetTitle() const
{
	return m_Title;
}

void UImGuiToolWindow::RenderTool()
{
	if (ImGui::CollapsingHeader("Show Windows"))
	{
		ImGui::Checkbox("Details", &m_DetailWindow->GetOpenRef());
		ImGui::Checkbox("Viewport", &m_ViewportWindow->GetOpenRef());
		ImGui::Separator();
	}
}

void UImGuiToolWindow::RenderDetail()
{
}

void UImGuiToolWindow::RenderViewport()
{
}