#pragma once
#include "GUI/Window/ImGuiWindow.h"
class ULoggerWindow :
	public UImGuiWindow
{
	void RenderConsole();

public:
	ULoggerWindow();
};
