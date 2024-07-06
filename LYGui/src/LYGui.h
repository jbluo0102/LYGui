#pragma once

#include <ThreadPool/ThreadPool.h>
#include <PugiXml/pugixml.hpp>

#define IMGUI_INI "imgui.ini"
#define PROJECT_DIR std::format("{}", std::filesystem::current_path().parent_path().string())
#define TARGET_DIR std::format("{}\\bin\\x64\\Release", std::filesystem::current_path().parent_path().string())

#define Delay(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))

/** Global space **/
namespace GApp {
	/** Available thread will be max_thread - 1 (except main thread)**/
	inline const unsigned int MaxThread = std::thread::hardware_concurrency() - 1;

	inline BS::thread_pool ThreadPool(MaxThread + 1);

	/** A pointer to GLFW window **/
	inline GLFWwindow* Window = nullptr;

	/** For config settings **/
	inline pugi::xml_document XMLDocument;

	/** Main Thread **/
	void MainThread();

	/** Returns true if thread pool is busy **/
	bool IsThreadPoolRunning();

	/** Returns true if thread pool is busy **/
	bool IsThreadPoolQueueing();

	/** Close the app **/
	void CloseApp();

	/** Get fps **/
	float GetFPS();

	/** Get cpu percentage **/
	double GetCPUPercentage();

	/** Get Memory Percentage **/
	double GetMemoryPercentage();
}