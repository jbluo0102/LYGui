#include "pch.h"
#include <GUI/Main/Application.h>

int WinMain()
{
	auto f = GApp::ThreadPool.submit([]() { GApp::MainThread(); });
	f.get();

	return 0;
}