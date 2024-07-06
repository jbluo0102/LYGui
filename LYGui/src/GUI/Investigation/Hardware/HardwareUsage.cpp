#include "pch.h"
#include "HardwareUsage.h"

#include <imgui.h>
#include <imgui_extension.h>

// Hardware usage
#include <Windows.h>
#include <psapi.h> // Memory

static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
static int numProcessors;
static HANDLE self;

UHardwareUsage::UHardwareUsage()
{
	SYSTEM_INFO sysInfo;
	FILETIME ftime, fsys, fuser;

	GetSystemInfo(&sysInfo);
	numProcessors = sysInfo.dwNumberOfProcessors;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&lastCPU, &ftime, sizeof(FILETIME));

	self = GetCurrentProcess();
	GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
	memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
	memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));
}

UHardwareUsage& UHardwareUsage::GetInstance()
{
	static UHardwareUsage instance;
	return instance;
}

double UHardwareUsage::GetCPUPercentage()
{
	FILETIME ftime, fsys, fuser;
	ULARGE_INTEGER now, sys, user;
	static constexpr int sample = 10;
	static int count = 0;
	static double percent;

	count = (count + 1) % sample;
	if (count)
	{
		return percent * 100;
	}

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&now, &ftime, sizeof(FILETIME));

	GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
	memcpy(&sys, &fsys, sizeof(FILETIME));
	memcpy(&user, &fuser, sizeof(FILETIME));
	percent = (sys.QuadPart - lastSysCPU.QuadPart) + (user.QuadPart - lastUserCPU.QuadPart);
	percent /= (now.QuadPart - lastCPU.QuadPart);
	percent /= numProcessors;
	lastCPU = now;
	lastUserCPU = user;
	lastSysCPU = sys;

	return percent * 100;
}

double UHardwareUsage::GetMemoryPercentage()
{
	static double percent;
	static PROCESS_MEMORY_COUNTERS_EX pmc;
	static constexpr int sample = 10;
	static int count = 0;

	count = (count + 1) % sample;
	if (count)
	{
		return percent;
	}

	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	percent = pmc.WorkingSetSize / 171798691.f;
	return percent;
}

void UHardwareUsage::Render()
{
	const ImGuiIO& io = ImGui::GetIO();
	static PROCESS_MEMORY_COUNTERS_EX pmc;
	static ImGui::ScrollingBuffer fps_sample;
	static ImGui::ScrollingBuffer ping_sample;
	static ImGui::ScrollingBuffer memory_sample;
	static ImGui::ScrollingBuffer cpu_sample;
	static ImGui::ScrollingBuffer thread_sample;

	static float time = 0;
	constexpr int num_of_hardware = 2;

	// Get memory usage
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));

	time += ImGui::GetIO().DeltaTime;
	fps_sample.AddPoint(time, io.Framerate);
	ping_sample.AddPoint(time, 1000.f / io.Framerate);
	memory_sample.AddPoint(time, GetMemoryPercentage());
	cpu_sample.AddPoint(time, GetCPUPercentage());
	thread_sample.AddPoint(time, static_cast<float>(GApp::ThreadPool.get_tasks_running()) / GApp::MaxThread * 100.f);

	static float history = 3.f;
	ImGui::DragFloat("History", &history, 0.1f, 1.f, 30.f, "%.1f s", ImGuiSliderFlags_AlwaysClamp);

	static ImVec2 window_size;
	window_size = ImVec2(-1, (ImGui::GetWindowSize().y / num_of_hardware) - (70.f / num_of_hardware));
	ImPlotFlags flag = ImPlotFlags_NoMenus | ImPlotFlags_NoMouseText;

	if (ImPlot::BeginPlot("##fps/ping", window_size, flag))
	{
		ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_Lock, ImPlotAxisFlags_Lock);
		ImPlot::SetupAxisLimits(ImAxis_X1, time - history, time, ImGuiCond_Always);
		ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 200);

		ImPlot::PlotLine("fps", &fps_sample.Data[0].x, &fps_sample.Data[0].y, fps_sample.Data.size(), ImPlotLineFlags_Shaded, fps_sample.Offset, 2 * sizeof(float));
		ImPlot::PlotLine("ping", &ping_sample.Data[0].x, &ping_sample.Data[0].y, ping_sample.Data.size(), ImPlotLineFlags_Shaded, ping_sample.Offset, 2 * sizeof(float));

		ImPlot::EndPlot();
	}

	if (ImPlot::BeginPlot("##cpu/memory/thread", window_size, flag))
	{
		ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_Lock, ImPlotAxisFlags_Lock);
		ImPlot::SetupAxisLimits(ImAxis_X1, time - history, time, ImGuiCond_Always);
		ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100);

		ImPlot::PushStyleColor(ImPlotCol_Line, FColor::Purple);
		ImPlot::PlotLine("cpu", &cpu_sample.Data[0].x, &cpu_sample.Data[0].y, cpu_sample.Data.size(), ImPlotLineFlags_Shaded, cpu_sample.Offset, 2 * sizeof(float));
		ImPlot::PopStyleColor();

		ImPlot::PushStyleColor(ImPlotCol_Line, FColor::Green);
		ImPlot::PlotLine("memory", &memory_sample.Data[0].x, &memory_sample.Data[0].y, memory_sample.Data.size(), ImPlotLineFlags_Shaded, memory_sample.Offset, 2 * sizeof(float));
		ImPlot::PopStyleColor();

		ImPlot::PushStyleColor(ImPlotCol_Line, FColor::Red);
		ImPlot::PlotLine("thread", &thread_sample.Data[0].x, &thread_sample.Data[0].y, thread_sample.Data.size(), ImPlotLineFlags_Shaded, thread_sample.Offset, 2 * sizeof(float));
		ImPlot::PopStyleColor();

		ImPlot::EndPlot();
	}
}