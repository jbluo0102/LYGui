#include "pch.h"
#include "Application.h"

#include <GUI/GUI.h>
#include <opencv2/core/ocl.hpp>
// #include <winbase.h>

Application::Application()
{
	m_ConsoleWindow = std::make_unique<ULoggerWindow>();
	m_HardwareUsageWindow = std::make_unique<UImGuiWindow>("HardwareUsage", true, 0, []() { UHardwareUsage::GetInstance().Render(); });

	m_DebugToolWindow = std::make_unique<UDebugToolWindow>();
	m_OpenCVToolWindow = std::make_unique<UOpenCVToolWindow>();
	m_PlotToolWindow = std::make_unique<UPlotToolWindow>();

	m_LoginFuture = GApp::ThreadPool.submit([]() {});

	m_WindowMode = 1;
	m_ThemeColor = 2;
	m_FontStyle = 1;

	m_IsLogin = false;
}

Application::~Application()
{
	SaveConfig();
}

void Application::RenderDockSpace()
{
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
}

void Application::RenderMenuBar()
{
	ImGui::BeginMainMenuBar();

	// App title
	{
		if (ImGui::BeginMenu("LYGui##Menu"))
		{
			if (ImGui::MenuItem("About##Menu"))
			{
			}
			if (ImGui::MenuItem("Contact##Menu"))
			{
			}

			ImGui::EndMenu();
		}
	}

	// Windows
	{
		static bool show_imgui_window = false;
		static bool show_implot_window = false;

		if (ImGui::BeginMenu("Windows"))
		{
			ImGui::MenuItem("Hardware Usage", nullptr, &m_HardwareUsageWindow->GetOpenRef());
			ImGui::MenuItem("Console", nullptr, &m_ConsoleWindow->GetOpenRef());
			if (ImGui::BeginMenu("Manual"))
			{
				ImGui::MenuItem("ImGui", nullptr, &show_imgui_window);
				ImGui::MenuItem("ImPlot", nullptr, &show_implot_window);

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		if (show_imgui_window)
		{
			ImGui::ShowDemoWindow(&show_imgui_window);
		}
		if (show_implot_window)
		{
			ImPlot::ShowDemoWindow(&show_implot_window);
		}
	}

	// Tools
	{
		if (ImGui::BeginMenu("Tools##Menu"))
		{
			if (ImGui::MenuItem("Debugger##Menu", nullptr, &m_DebugToolWindow->GetOpenRef()))
			{
				m_DebugToolWindow->SetOpen(m_DebugToolWindow->IsOpen());
			}
			if (ImGui::MenuItem("OpenCV##Menu", nullptr, &m_OpenCVToolWindow->GetOpenRef()))
			{
				m_OpenCVToolWindow->SetOpen(m_OpenCVToolWindow->IsOpen());
			}
			if (ImGui::MenuItem("Plot##Menu", nullptr, &m_PlotToolWindow->GetOpenRef()))
			{
				m_PlotToolWindow->SetOpen(m_PlotToolWindow->IsOpen());
			}

			ImGui::EndMenu();
		}
	}

	// Settings
	{
		if (ImGui::BeginMenu("Settings##Menu"))
		{
			if (ImGui::BeginMenu("Widnow Mode##Menu"))
			{
				if (ImGui::RadioButtonEnum("Window Screen##Menu", &m_WindowMode, EWindowMode::Window) ||
					ImGui::RadioButtonEnum("Full Screen##Menu", &m_WindowMode, EWindowMode::FullScreen))
				{
					FTheme::SetWindowMode(static_cast<EWindowMode>(m_WindowMode));
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Theme##Menu"))
			{
				if (ImGui::RadioButtonEnum("Default Dark##Menu", &m_ThemeColor, EThemeColor::DefaultDark) ||
					ImGui::RadioButtonEnum("Default Light##Menu", &m_ThemeColor, EThemeColor::DefaultLight) ||
					ImGui::RadioButtonEnum("Classic##Menu", &m_ThemeColor, EThemeColor::Classic) ||
					ImGui::RadioButtonEnum("Full Dark##Menu", &m_ThemeColor, EThemeColor::Dark) ||
					ImGui::RadioButtonEnum("Darcula##Menu", &m_ThemeColor, EThemeColor::Darcula)
					)
				{
					FTheme::SetThemeMode(static_cast<EThemeColor>(m_ThemeColor));
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Font Style##Menu"))
			{
				if (ImGui::RadioButtonEnum("Consolas Origin##Menu", &m_FontStyle, EFontStyle::Consolas_Regular) ||
					ImGui::RadioButtonEnum("Consolas Bold##Menu", &m_FontStyle, EFontStyle::Consolas_Bold) ||
					ImGui::RadioButtonEnum("SF Pro Origin##Menu", &m_FontStyle, EFontStyle::SFPro_Regular) ||
					ImGui::RadioButtonEnum("SF Pro Bold##Menu", &m_FontStyle, EFontStyle::SFPro_Bold) ||
					ImGui::RadioButtonEnum("Source SansPro Regular##Menu", &m_FontStyle, EFontStyle::SourceSansPro_Regular) ||
					ImGui::RadioButtonEnum("Source SansPro Bold##Menu", &m_FontStyle, EFontStyle::SourceSansPro_Bold)
					)
				{
					FTheme::SetFontStyle(static_cast<EFontStyle>(m_FontStyle));
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}
	}

	// Hardware info
	{
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 3);
		ImGui::BulletText(std::format("fps:{:3.0f}", GApp::GetFPS()).c_str());
		ImGui::BulletText(std::format("cpu:{:3.0f}%%", GApp::GetCPUPercentage()).c_str());
		ImGui::BulletText(std::format("memory:{:3.0f}%%", GApp::GetMemoryPercentage()).c_str());

		ImGui::BulletText("thread pool:");
		if (GApp::IsThreadPoolRunning())
		{
			ImGui::TextColored(FColor::Yellow, "Running");
			ImGui::Spinner("Threading", 6.f, 3, FColor::Yellow);
		}
		else
		{
			ImGui::TextColored(FColor::Green, "Idle");
		}
	}

	// Exit button
	{
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 50.f);
		if (ImGui::Button("X", ImVec2(50, 0)))
		{
			ImGui::OpenPopup("Close App");
		}
		if (ImGui::BeginPopupModal("Close App", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Sure to exit?");

			ImGui::Separator();

			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				GApp::CloseApp();
			}
			//ImGui::SetItemDefaultFocus();
			ImGui::SameLine();

			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	ImGui::EndMainMenuBar();
}

void Application::RenderLoginWindow()
{
	if (!m_IsLogin)
	{
		ImGui::OpenPopup("Login");
	}
	if (ImGui::BeginPopupModal("Login", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static char account[32] = "Dev";
		static char password[32] = "Dev";

		ImGui::InputText("Account##Login", account, IM_ARRAYSIZE(account));
		ImGui::InputText("Password##Login", password, IM_ARRAYSIZE(password), ImGuiInputTextFlags_Password);
		ImGui::Separator();

		if (ImGui::Button("Login", m_LoginFuture._Is_ready(), ImVec2(150, 0)))
		{
			m_LoginFuture = GApp::ThreadPool.submit([this]() {
				Delay(2000);
				m_IsLogin = true;
				});
		}

		ImGui::SameLine();

		if (ImGui::Button("Exit", m_LoginFuture._Is_ready(), ImVec2(150, 0)))
		{
			GApp::CloseApp();
		}

		if (!m_LoginFuture._Is_ready())
		{
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.f - 5.f);
			ImGui::Spinner("##Logining", 6.f, 3.f, FColor::Blue);
		}
		else if (m_IsLogin)
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

Application& Application::GetInstance()
{
	static Application instance;
	return instance;
}

void Application::Init()
{
	LOG(Success, std::format("ImGui version: {}", ImGui::GetVersion()));
	LOG(Success, std::format("OpenCV version: {}", CV_VERSION));

	cv::ocl::setUseOpenCL(true);
	LOG(Success, cv::ocl::haveOpenCL() ? "OpenCL has been enable!" : "OpenCL is disable");

	LOG(Success, std::format("Available threads: {}", GApp::MaxThread));
	LOG(Success, std::format("Thread pool version: {}", BS_THREAD_POOL_VERSION));

	LoadConfig();
}

void Application::Render()
{
	RenderDockSpace();
	RenderLoginWindow();
	RenderMenuBar();

	m_ConsoleWindow->Render();
	m_HardwareUsageWindow->Render();
	m_DebugToolWindow->Render();

	m_OpenCVToolWindow->Render();
	m_PlotToolWindow->Render();
}

void Application::LoadConfig()
{
	FTimer timer("Load Config");

	FTheme::LoadPlotTheme();
	FTheme::LoadFont();

	GApp::XMLDocument.load_file(XML_UI_PATH);

	// Widnows
	bool open = false;
	open = GApp::XMLDocument.child(XML_UI).child(m_ConsoleWindow->GetTitle().c_str()).text().as_bool();
	m_ConsoleWindow->SetOpen(open);

	open = GApp::XMLDocument.child(XML_UI).child(m_HardwareUsageWindow->GetTitle().c_str()).text().as_bool();
	m_HardwareUsageWindow->SetOpen(open);

	open = GApp::XMLDocument.child(XML_UI).child(m_DebugToolWindow->GetTitle().c_str()).text().as_bool();
	m_DebugToolWindow->SetOpen(open);

	open = GApp::XMLDocument.child(XML_UI).child(m_OpenCVToolWindow->GetTitle().c_str()).text().as_bool();
	m_OpenCVToolWindow->SetOpen(open);

	open = GApp::XMLDocument.child(XML_UI).child(m_PlotToolWindow->GetTitle().c_str()).text().as_bool();
	m_PlotToolWindow->SetOpen(open);

	// Theme
	pugi::xml_node node;

	node = GApp::XMLDocument.child(XML_UI).child(XML_WINDOW_MODE);
	m_WindowMode = node.empty() ? m_WindowMode : node.text().as_int();
	FTheme::SetWindowMode(static_cast<EWindowMode>(m_WindowMode));

	node = GApp::XMLDocument.child(XML_UI).child(XML_THEME_MODE);
	m_ThemeColor = node.empty() ? m_ThemeColor : node.text().as_int();
	FTheme::SetThemeMode(static_cast<EThemeColor>(m_ThemeColor));

	// Font
	node = GApp::XMLDocument.child(XML_UI).child(XML_FONT_STYLE);
	m_FontStyle = node.empty() ? m_FontStyle : node.text().as_int();
	FTheme::SetFontStyle(static_cast<EFontStyle>(m_FontStyle));
}

void Application::SaveConfig()
{
	pugi::xml_document root;
	auto ui_root = root.append_child("UI");
	ui_root.append_child(m_ConsoleWindow->GetTitle().c_str()).text().set(std::format("{}", m_ConsoleWindow->IsOpen()).c_str());
	ui_root.append_child(m_HardwareUsageWindow->GetTitle().c_str()).text().set(std::format("{}", m_HardwareUsageWindow->IsOpen()).c_str());

	ui_root.append_child(m_DebugToolWindow->GetTitle().c_str()).text().set(std::format("{}", m_DebugToolWindow->IsOpen()).c_str());
	ui_root.append_child(m_OpenCVToolWindow->GetTitle().c_str()).text().set(std::format("{}", m_OpenCVToolWindow->IsOpen()).c_str());
	ui_root.append_child(m_PlotToolWindow->GetTitle().c_str()).text().set(std::format("{}", m_PlotToolWindow->IsOpen()).c_str());

	ui_root.append_child(XML_WINDOW_MODE).text().set(m_WindowMode);
	ui_root.append_child(XML_THEME_MODE).text().set(m_ThemeColor);
	ui_root.append_child(XML_FONT_STYLE).text().set(m_FontStyle);

	bool is_success = root.save_file(XML_UI_PATH, PUGIXML_TEXT("  "));
	assert(is_success);

	// Copy imgui.ini
	// CopyFileA(std::format("{}\\LYGui\\{}", PROJECT_DIR, IMGUI_INI).c_str(), std::format("{}\\{}", TARGET_DIR, IMGUI_INI).c_str(), 0);
}