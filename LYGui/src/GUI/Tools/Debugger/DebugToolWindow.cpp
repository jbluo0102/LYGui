#include "pch.h"
#include "DebugToolWindow.h"

#include <GUI/Window/ImGuiWindow.h>

UDebugToolWindow::UDebugToolWindow()
	: UImGuiToolWindow("Debugger", true)
{
	m_TreeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen;

	m_LoadFuture = GApp::ThreadPool.submit([]() {});
	m_AllocateFuture = GApp::ThreadPool.submit([]() {});
}

void UDebugToolWindow::RenderTool()
{
	UImGuiToolWindow::RenderTool();

	RenderHardware();
	RenderLayout();
	RenderUIDesign();
	RenderFunctional();
}

void UDebugToolWindow::RenderDetail()
{
	ImGui::BulletText(std::format("Max thread           : {}", GApp::ThreadPool.get_thread_count()).c_str());
	ImGui::BulletText(std::format("Num of thread queed  : {}", GApp::ThreadPool.get_tasks_queued()).c_str());
	ImGui::BulletText(std::format("Num of thread running: {}", GApp::ThreadPool.get_tasks_running()).c_str());
	ImGui::BulletText(std::format("Num of thread total  : {}", GApp::ThreadPool.get_tasks_total()).c_str());
}

void UDebugToolWindow::RenderViewport()
{
	RenderFuncMap();
}

void UDebugToolWindow::RenderHardware()
{
	if (ImGui::CollapsingHeader("Hardware", m_TreeNodeFlags))
	{
		ImGui::SeparatorText("CPU");
		{
			static int cpu_performance = 50;
			if (ImGui::SliderInt("Test##cpu", &cpu_performance, 0, 100))
			{
				GApp::ThreadPool.submit([this]() { RandomTask(); });
			}
		}

		ImGui::SeparatorText("Stack Allocation");
		{
			static std::vector<float> arr;

			ImGui::PushID("stack");
			ImGui::BulletText("Allocate 10 billion float of vector");
			if (ImGui::Button("Allocate", m_AllocateFuture._Is_ready()))
			{
				if (arr.empty())
				{
					m_AllocateFuture = GApp::ThreadPool.submit([]() {
						FTimer timer("Stack Allocation");
						arr = std::vector<float>(1000000000, 0);
						});
				}
				else
				{
					LOG(Warning, "Stack has already been allocated");
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("Clear", m_AllocateFuture._Is_ready()))
			{
				if (!arr.empty())
				{
					m_AllocateFuture = GApp::ThreadPool.submit([]() {
						FTimer timer("Stack De-Allocation");
						std::vector<float>().swap(arr);
						});
				}
				else
				{
					LOG(Warning, "Stack has already been clear");
				}
			}

			if (!m_AllocateFuture._Is_ready())
			{
				ImGui::SameLine();
				ImGui::Loading("Allocating", FColor::Yellow);
			}

			ImGui::PopID();
		}

		ImGui::SeparatorText("Thread Pool");
		{
			ImGui::BulletText("Status(Except Main Thread): ");
			ImGui::SameLine();
			GApp::IsThreadPoolRunning() ? ImGui::Loading("Busy", FColor::Yellow) : ImGui::TextColored(FColor::Green, "Idle");

			ImGui::BulletTextColored(FColor::Blue, "Wait for single task");
			ImGui::SameLine();
			if (ImGui::Button("Test##SingleTask", m_LoadFuture._Is_ready()))
			{
				m_LoadFuture = GApp::ThreadPool.submit([this]() { RandomTask(); });
			}

			ImGui::BulletTextColored(FColor::Blue, "No wait for 24 tasks");
			ImGui::SameLine();
			if (ImGui::Button("Test##NoWait"))
			{
				for (int idx = 0; idx < GApp::MaxThread * 2; idx++)
				{
					GApp::ThreadPool.submit(&UDebugToolWindow::RandomTask, this);
				}
			}

			ImGui::BulletTextColored(FColor::Blue, "Wait for 5 tasks    ");
			ImGui::SameLine();
			if (ImGui::Button("Test##Wait"))
			{
				std::vector<std::future<void>> futures;
				futures.reserve(5);
				for (int idx = 0; idx < 5; idx++)
				{
					futures.emplace_back(GApp::ThreadPool.submit(&UDebugToolWindow::RandomTask, this));
				}

				for (const auto& future : futures)
				{
					future.wait();
				}
			}

			ImGui::BulletTextColored(FColor::Blue, "Parrallel Loop      ");
			ImGui::SameLine();
			if (ImGui::Button("Test##Parrallel"))
			{
				GApp::ThreadPool.parallelize_loop(GApp::MaxThread, [this](const int start, const int end) {
					for (int idx = start; idx < end; idx++)
					{
						RandomTask();
					}
					});
			}
		}

		ImGui::Separator();
	}
}

void UDebugToolWindow::RenderLayout()
{
	if (ImGui::CollapsingHeader("Layout", m_TreeNodeFlags))
	{
		static unsigned int count = 0;
		static auto flags = ImGuiTreeNodeFlags_DefaultOpen;
		ImGui::SeparatorText("Modify detail on details window");
		if (ImGui::Button("Add##Details"))
		{
			unsigned int idx = count++;
			m_DetailWindow->AddRenderFunc(std::format("{}", idx), [idx]() {
				if (ImGui::TreeNodeExColored(FColor::Green, std::format("[{}]Root...", idx).c_str(), flags))
				{
					if (ImGui::TreeNodeExColored(FColor::Yellow, "Child[1]...", flags))
					{
						if (ImGui::TreeNodeExColored(FColor::Red, std::format("Child[2]...", idx).c_str(), flags))
						{
							ImGui::Text("Some details");
							ImGui::TreePop();
						}
						ImGui::TreePop();
					}
					ImGui::TreePop();
				}
				});
		}

		ImGui::SameLine();

		if (ImGui::Button("Clear All Details"))
		{
			m_DetailWindow->ClearRenderFunc();
		}

		static char input[128];
		ImGui::InputText("##Key", input, sizeof(input));
		ImGui::SameLine();
		if (ImGui::Button("Erase Details"))
		{
			m_DetailWindow->EraseRenderFunc(input);
		}
	}
}

void UDebugToolWindow::RenderUIDesign()
{
	if (ImGui::CollapsingHeader("UI Design", m_TreeNodeFlags))
	{
		ImGui::SeparatorText("Show Window");
		static UImGuiWindow window("Test", false, 0, [this]() { ImGui::Loading("Loading"); });
		ImGui::Checkbox("Show##Window", &window.GetOpenRef());
		window.Render();

		ImGui::SeparatorText("Font");

		static float font_size = 1.f;
		if (ImGui::DragFloat("Font Scale", &font_size, 0.1f, 1.f, 2.f))
		{
			ImGui::GetIO().FontGlobalScale = font_size;
		}

		ImGui::Separator();
	}
}

void UDebugToolWindow::RenderFuncMap()
{
	auto& map = m_DetailWindow->GetRenderFuncMap();
	for (const auto& [key, value] : map)
	{
		ImGui::BulletText(std::format("<name: {:2}, index: {:2}>", key, value).c_str());
	}
}

void UDebugToolWindow::RenderFunctional()
{
	if (ImGui::CollapsingHeader("Functions", m_TreeNodeFlags))
	{
		ImGui::SeparatorText("Exit App");
		if (ImGui::Button("Exit"))
		{
			GApp::CloseApp();
		}

		ImGui::Separator();
	}
}

void UDebugToolWindow::RandomTask()
{
	std::thread::id id = std::this_thread::get_id();
	std::ostringstream oss;
	oss << id;

	int random_ms = FMath::GetRandom<int>(10000000, 15000000);

	LOG(Warning, std::format("Thread[{}]Start task", oss.str()));
	volatile double x = 0.0;
	volatile double res = 0.0;
	for (int i = 0; i < random_ms; i++)
	{
		res = std::sin(x) * std::cos(x) + std::tan(x);
		x += 0.001;
	}
	LOG(Warning, std::format("Thread[{}]End task", oss.str()));
}