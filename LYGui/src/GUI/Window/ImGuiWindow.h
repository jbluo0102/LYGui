#pragma once

#include <string>
#include <imgui.h>
#include <functional>
#include <vector>
#include <unordered_map>

class UImGuiWindow
{
protected:
	/** For window's title **/
	std::string m_Title;

	/** For window's flag control **/
	ImGuiWindowFlags m_Flags;

	/** For rendering main content **/
	std::function<void()> m_RenderFuncMain;

	/** For rendering exra content **/
	std::vector<std::function<void()>> m_RenderFuncArray;

	/** For indexing func array **/
	std::unordered_map<std::string, int> m_RenderFuncMap;

	/** Open the window if true **/
	bool m_Open;

	/** Load/Save config **/
	virtual void LoadConfigSettings();
	virtual void SaveConfigSettings();

public:
	/** Constructor/Desctructor **/
	UImGuiWindow(const std::string& Title, bool Open = true, ImGuiWindowFlags Flags = 0, const std::function<void()>& RenderFunc = nullptr);
	UImGuiWindow(const std::string& Title, const std::function<void()>& RenderFunc);
	virtual ~UImGuiWindow();
public:
	/** Get open ref **/
	bool& GetOpenRef();

	/** Returns window's title **/
	std::string GetTitle() const;

	/** Return true if window is opening **/
	bool IsOpen() const;

	/** Return render function container **/
	const std::vector<std::function<void()>>& GetRenderFuncArray() const;
	const std::unordered_map<std::string, int>& GetRenderFuncMap() const;

	/** Render the window **/
	void Render();

	/** Open/Close the window **/
	void SetOpen(bool Open);

	/** Set window **/
	void SetRenderFunc(const std::function<void()>& RenderFunc);
	void SetFlags(ImGuiWindowFlags Flags);

	/** Relpace the origin function if it's already exist **/
	void AddRenderFunc(const std::string& Key, const std::function<void()>& RenderFunc);

	/** Erase the render function by Key **/
	void EraseRenderFunc(const std::string& Key);

	/** Clear all render function except main **/
	void ClearRenderFunc();
};
