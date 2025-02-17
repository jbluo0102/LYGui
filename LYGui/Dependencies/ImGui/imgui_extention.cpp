#include <imgui_extension.h>

void ImGui::BulletTextColored(const ImVec4& color, const char* label)
{
	ImGui::PushStyleColor(ImGuiCol_Text, color);
	ImGui::BulletText(label);
	ImGui::PopStyleColor();
}

bool ImGui::TreeNodeColored(const ImVec4& color, const char* label)
{
	ImGui::PushStyleColor(ImGuiCol_Text, color);
	const bool res = ImGui::TreeNode(label);
	ImGui::PopStyleColor();

	return res;
}

bool ImGui::TreeNodeExColored(const ImVec4& color, const char* label, ImGuiTreeNodeFlags flag)
{
	ImGui::PushStyleColor(ImGuiCol_Text, color);
	const bool res = ImGui::TreeNodeEx(label, flag);
	ImGui::PopStyleColor();

	return res;
}

void ImGui::Loading(const char* label, const ImVec4& color)
{
	ImGui::TextColored(color, "%s %c", label, "|/-\\"[(int)(ImGui::GetTime() / 0.05f) & 3]);
}

bool ImGui::BufferingBar(const char* label, float value, const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size = size_arg;
	size.x -= style.FramePadding.x * 2;

	const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
	ItemSize(bb, style.FramePadding.y);
	if (!ItemAdd(bb, id))
		return false;

	// Render
	const float circleStart = size.x * 0.7f;
	const float circleEnd = size.x;
	const float circleWidth = circleEnd - circleStart;

	window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart, bb.Max.y), bg_col);
	window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart * value, bb.Max.y), fg_col);

	const float t = g.Time;
	const float r = size.y / 2;
	const float speed = 1.5f;

	const float a = speed * 0;
	const float b = speed * 0.333f;
	const float c = speed * 0.666f;

	const float o1 = (circleWidth + r) * (t + a - speed * (int)((t + a) / speed)) / speed;
	const float o2 = (circleWidth + r) * (t + b - speed * (int)((t + b) / speed)) / speed;
	const float o3 = (circleWidth + r) * (t + c - speed * (int)((t + c) / speed)) / speed;

	window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o1, bb.Min.y + r), r, bg_col);
	window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o2, bb.Min.y + r), r, bg_col);
	window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o3, bb.Min.y + r), r, bg_col);
}

bool ImGui::Spinner(const char* label, float radius, int thickness, const ImU32& color)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);

	const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
	ItemSize(bb, style.FramePadding.y);
	if (!ItemAdd(bb, id))
		return false;

	// Render
	window->DrawList->PathClear();

	int num_segments = 30;
	int start = abs(ImSin(g.Time * 1.8f) * (num_segments - 5));

	const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;
	const float a_max = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

	const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

	for (int i = 0; i < num_segments; i++) {
		const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
		window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a + g.Time * 8) * radius,
			centre.y + ImSin(a + g.Time * 8) * radius));
	}

	window->DrawList->PathStroke(color, false, thickness);
	return true;
}

bool ImGui::Spinner(const char* label, float radius, int thickness, const ImVec4& color)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);

	const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
	ItemSize(bb, style.FramePadding.y);
	if (!ItemAdd(bb, id))
		return false;

	// Render
	window->DrawList->PathClear();

	int num_segments = 30;
	int start = abs(ImSin(g.Time * 1.8f) * (num_segments - 5));

	const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;
	const float a_max = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

	const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

	for (int i = 0; i < num_segments; i++) {
		const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
		window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a + g.Time * 8) * radius,
			centre.y + ImSin(a + g.Time * 8) * radius));
	}

	window->DrawList->PathStroke(ImGui::GetColorU32(color), false, thickness);
	return true;
}

bool ImGui::Button(const char* label, bool is_valid, const ImVec2& size_arg)
{
	if (!is_valid)
	{
		auto& colors = ImGui::GetStyle().Colors;
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, FColor::Red);
		ImGui::Button(label, size_arg);
		ImGui::PopStyleColor(1);
		return false;
	}

	return ImGui::Button(label, size_arg);
}

ImVec4 ImGui::ImColor2ImVec4(const ImColor& color)
{
	return static_cast<ImVec4>(color);
}