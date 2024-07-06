#include "pch.h"
#include "ImGuiWindow.h"

UImGuiWindow::UImGuiWindow(const std::string& Title, bool Open, ImGuiWindowFlags Flags, const std::function<void()>& RenderFunc)
	: m_Title(Title), m_Open(Open), m_Flags(Flags), m_RenderFuncMain(RenderFunc)
{
	LoadConfigSettings();
}

UImGuiWindow::UImGuiWindow(const std::string& Title, const std::function<void()>& RenderFunc)
	: UImGuiWindow(Title, true, 0, RenderFunc)
{
}

UImGuiWindow::~UImGuiWindow()
{
	SaveConfigSettings();
}

void UImGuiWindow::Render()
{
	if (m_Open)
	{
		ImGui::Begin(m_Title.c_str(), &m_Open, m_Flags);
		m_RenderFuncMain();
		for (const auto& func : m_RenderFuncArray)
		{
			func();
		}
		ImGui::End();
	}
}

bool& UImGuiWindow::GetOpenRef()
{
	return m_Open;
}

std::string UImGuiWindow::GetTitle() const
{
	return m_Title;
}

bool UImGuiWindow::IsOpen() const
{
	return m_Open;
}

const std::vector<std::function<void()>>& UImGuiWindow::GetRenderFuncArray() const
{
	return m_RenderFuncArray;
}

const std::unordered_map<std::string, int>& UImGuiWindow::GetRenderFuncMap() const
{
	return m_RenderFuncMap;
}

void UImGuiWindow::SetOpen(bool Open)
{
	m_Open = Open;
}

void UImGuiWindow::SetRenderFunc(const std::function<void()>& RenderFunc)
{
	m_RenderFuncMain = RenderFunc;
}

void UImGuiWindow::SetFlags(ImGuiWindowFlags Flags)
{
	m_Flags = Flags;
}

void UImGuiWindow::AddRenderFunc(const std::string& Key, const std::function<void()>& RenderFunc)
{
	auto it = m_RenderFuncMap.find(Key);
	if (it == m_RenderFuncMap.end())
	{
		m_RenderFuncArray.push_back(RenderFunc);
		m_RenderFuncMap[Key] = m_RenderFuncArray.size() - 1;
	}
	else
	{
		m_RenderFuncArray[it->second] = RenderFunc;
	}
}

void UImGuiWindow::EraseRenderFunc(const std::string& Key)
{
	auto it = m_RenderFuncMap.find(Key);
	if (it == m_RenderFuncMap.end())
	{
		return;
	}

	m_RenderFuncMap.erase(it->first);
	m_RenderFuncArray.erase(m_RenderFuncArray.begin() + it->second);
}

void UImGuiWindow::ClearRenderFunc()
{
	std::vector<std::function<void()>>().swap(m_RenderFuncArray);
	std::unordered_map<std::string, int>().swap(m_RenderFuncMap);
}

void UImGuiWindow::LoadConfigSettings()
{
}

void UImGuiWindow::SaveConfigSettings()
{
}