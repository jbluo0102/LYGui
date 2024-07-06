#pragma once

#include <string>
#include <memory>
#include <unordered_map>

class Application
{
	/** Constructor/Desctructor **/
	Application();
	virtual ~Application();

	/** Widnows **/
	std::unique_ptr<class ULoggerWindow> m_ConsoleWindow;
	std::unique_ptr<class UImGuiWindow> m_HardwareUsageWindow;

	/** Tools **/
	std::unique_ptr<class UDebugToolWindow> m_DebugToolWindow;
	std::unique_ptr<class UOpenCVToolWindow> m_OpenCVToolWindow;
	std::unique_ptr<class UPlotToolWindow> m_PlotToolWindow;

	/** Futures **/
	std::future<void> m_LoginFuture;

	/** Configs **/
	int m_WindowMode;
	int m_ThemeColor;
	int m_FontStyle;

	bool m_IsLogin;

protected:
	void RenderDockSpace();
	void RenderMenuBar();
	void RenderLoginWindow();

	void LoadConfig();
	void SaveConfig();

public:
	static Application& GetInstance();
public:
	void Init();
	void Render();
};
