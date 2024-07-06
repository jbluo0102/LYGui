#pragma once

#include <imgui_internal.h>
#include <ImPlot/implot.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>
#include <GUI/Style/Style.h>

namespace ImGui
{
	/** utility structure for realtime plot **/
	struct ScrollingBuffer {
		int MaxSize;
		int Offset;
		ImVector<ImVec2> Data;
		ScrollingBuffer(int max_size = 2000) {
			MaxSize = max_size;
			Offset = 0;
			Data.reserve(MaxSize);
		}
		void AddPoint(float x, float y) {
			if (Data.size() < MaxSize)
				Data.push_back(ImVec2(x, y));
			else {
				Data[Offset] = ImVec2(x, y);
				Offset = (Offset + 1) % MaxSize;
			}
		}
		void Erase() {
			if (Data.size() > 0) {
				Data.shrink(0);
				Offset = 0;
			}
		}
	};

	/** utility structure for realtime plot **/
	struct RollingBuffer {
		float Span;
		ImVector<ImVec2> Data;
		RollingBuffer() {
			Span = 10.0f;
			Data.reserve(2000);
		}
		void AddPoint(float x, float y) {
			float xmod = fmodf(x, Span);
			if (!Data.empty() && xmod < Data.back().x)
				Data.shrink(0);
			Data.push_back(ImVec2(xmod, y));
		}
	};

	ImVec4 ImColor2ImVec4(const ImColor& color);

	void BulletTextColored(const ImVec4& color, const char* label);

	bool TreeNodeColored(const ImVec4& color, const char* label);

	bool TreeNodeExColored(const ImVec4& color, const char* label, ImGuiTreeNodeFlags flag = 0);

	void Loading(const char* label, const ImVec4& color = FColor::Normal);

	bool BufferingBar(const char* label, float value, const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col);

	bool Spinner(const char* label, float radius = 6.f, int thickness = 3.f, const ImU32& color = ImGui::GetColorU32(FColor::Blue));
	bool Spinner(const char* label, float radius = 6.f, int thickness = 3.f, const ImVec4& color = FColor::Blue);

	bool Button(const char* label, bool is_valid, const ImVec2& size_arg = ImVec2(0, 0));

	template <typename T>
	bool RadioButtonEnum(const char* label, int* v, T v_button)
	{
		return ImGui::RadioButton(label, v, static_cast<int>(v_button));
	}
}
