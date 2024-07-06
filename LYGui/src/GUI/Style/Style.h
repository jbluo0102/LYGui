#pragma once

#include <imgui.h>
#include <imgui_internal.h>

#include <vector>

#define XML_UI "UI"
#define XML_WINDOW_MODE "WindowMode"
#define XML_THEME_MODE "ThemeColor"
#define XML_FONT_STYLE "FontStyle"

struct FColor
{
	inline static const ImVec4 Normal = (ImVec4)ImColor(255, 255, 255, 255);
	inline static const ImVec4 Green = (ImVec4)ImColor(46, 204, 113, 255);
	inline static const ImVec4 Red = (ImVec4)ImColor(157, 34, 53, 255);
	inline static const ImVec4 Yellow = (ImVec4)ImColor(189, 155, 25, 255);
	inline static const ImVec4 Purple = (ImVec4)ImColor(188, 121, 184, 255);
	inline static const ImVec4 Blue = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
};

enum class EFontStyle
{
	Consolas_Regular,
	Consolas_Bold,
	SFPro_Regular,
	SFPro_Bold,
	SourceSansPro_Regular,
	SourceSansPro_Bold
};

struct FFontStyle
{
	/** Load Fonts
	* If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	* AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	* If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	* The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	* Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
	* Read 'docs/FONTS.md' for more instructions and details.
	* Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	* Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
	* io.Fonts->AddFontDefault();
	* io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
	* io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	* io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	* io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	* ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
	* IM_ASSERT(font != nullptr);
	**/
	static ImFont* Consolas_Regular;
	static ImFont* Consolas_Bold;
	static ImFont* SFPro_Regular;
	static ImFont* SFPro_Bold;
	static ImFont* SourceSansPro_Regular;
	static ImFont* SourceSansPro_Bold;
};

enum class EThemeColor
{
	DefaultDark,
	DefaultLight,
	Classic,
	Dark,
	Darcula
};

enum class EWindowMode
{
	Window,
	FullScreen
};

enum class EVSyncMode
{
	Disable,
	Enable
};

struct FTheme
{
	static void LoadDefaultDarkTheme();
	static void LoadDefaultLightTheme();
	static void LoadClassicTheme();
	static void LoadDarkTheme();
	static void LoadDarculaTheme();
	static void LoadPlotTheme();
	static void LoadFont();

	static void SetWindowMode(EWindowMode Mode);
	static void SetThemeMode(EThemeColor Mode);
	static void SetFontStyle(EFontStyle Style);
};

struct FPlotColor
{
	static const std::vector<ImColor> m_PlotColors;
	static const int m_NumOfColor;

	/** Get the color[idx % num] **/
	static ImColor Get(int idx);
};
