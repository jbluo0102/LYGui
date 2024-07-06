#pragma once

#include <memory>
#include <string>

class UImGuiToolWindow
{
protected:
	/** For window's title **/
	std::string m_Title;

	/** Open all window if true **/
	bool m_Open;

	/** Windows **/
	std::unique_ptr<class UImGuiWindow> m_ToolWindow;
	std::unique_ptr<class UImGuiWindow> m_DetailWindow;
	std::unique_ptr<class UImGuiWindow> m_ViewportWindow;

	/** For rendering **/
	virtual void RenderTool();
	virtual void RenderDetail();
	virtual void RenderViewport();

public:
	/** Constructor/Descturctor **/
	UImGuiToolWindow(const std::string& Title, bool Open = true);
	virtual ~UImGuiToolWindow();
public:
	/** For rendering all window **/
	void Render();

	/** Render if set to true **/
	void SetOpen(bool Open);
	void SetToolOpen(bool Open);
	void SetDetailOpen(bool Open);
	void SetViewportOpen(bool Open);

	/** Returns m_Open's ref **/
	bool& GetOpenRef();

	/** Returns true if window is open **/
	bool IsOpen() const;

	/** Returns window's title **/
	std::string GetTitle() const;
};
