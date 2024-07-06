#include "pch.h"
#include "PlotToolWindow.h"

UPlotToolWindow::UPlotToolWindow()
	:UImGuiToolWindow("Plot")
{
	m_Future = GApp::ThreadPool.submit([]() {});
}

void UPlotToolWindow::ClearCurveData()
{
	std::vector<std::vector<float>>().swap(m_DataX);
	std::vector<std::vector<float>>().swap(m_DataY);
	std::vector<std::vector<float>>().swap(m_PeakX);
	std::vector<std::vector<float>>().swap(m_PeakY);
	std::vector<std::string>().swap(m_Attributes);
}

void UPlotToolWindow::RenderTool()
{
	UImGuiToolWindow::RenderTool();

	if (ImGui::CollapsingHeader("AA", ImGuiTreeNodeFlags_DefaultOpen))
	{
		static int aa_type = 0;
		static const std::array<const char*, 2> aa_types = { "AA Curve", "AA Batch Run" };
		ImGui::Combo("Type##AA", &aa_type, aa_types.data(), static_cast<int>(aa_types.size()));

		if (ImGui::Button("Load Xml", m_Future._Is_ready()))
		{
			ImGuiFileDialog::Instance()->OpenDialog("ChooseXmlDlgKey", "Choose Xml", ".*", ".");
		}

		if (ImGuiFileDialog::Instance()->Display("ChooseXmlDlgKey"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				// Create xml
				m_PathName = ImGuiFileDialog::Instance()->GetFilePathName();
				m_FileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
				m_IsLoadFile = m_XmlDocument.load_file(m_PathName.c_str());
				if (m_IsLoadFile)
				{
					switch (static_cast<EAAData>(aa_type))
					{
					case EAAData::AACurve:
						m_Future = GApp::ThreadPool.submit([this]() { MakeAACurveData(m_XmlDocument); });
						break;
					case EAAData::AABatchRun:
						m_Future = GApp::ThreadPool.submit([this]() { MakeAABatchRunData(m_XmlDocument); });
						break;
					default:
						break;
					}
				}
			}

			ImGuiFileDialog::Instance()->Close();
		}

		ImGui::Separator();
	}
}

void UPlotToolWindow::RenderDetail()
{
	ImGui::SeparatorText("Info");
	ImGui::BulletText("Name: %s", m_FileName.c_str());
	ImGui::BulletText("Path: %s", m_PathName.c_str());
}

void UPlotToolWindow::RenderViewport()
{
	const int num_of_curve = m_DataX.size();
	float y = ImGui::GetWindowSize().y - 40;
	if (ImPlot::BeginPlot("", ImVec2(-1, y)))
	{
		ImPlot::EndPlot();
	}
}

void UPlotToolWindow::RenderAACurvePlot()
{
	if (!m_Future._Is_ready())
	{
		return;
	}

	const int num_of_curve = m_DataX.size();
	float y = ImGui::GetWindowSize().y - 40;
	if (ImPlot::BeginPlot(m_FileName.c_str(), ImVec2(-1, y)))
	{
		for (int idx = 0; idx < num_of_curve; idx++)
		{
			const ImColor color = FPlotColor::Get(idx);

			ImPlot::SetNextLineStyle(color);
			ImPlot::PlotLine(std::format("Line[{}]", idx).c_str(),
				m_DataX[idx].data(),
				m_DataY[idx].data(),
				m_DataX[idx].size(),
				ImPlotLineFlags_Shaded
			);

			ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
			ImPlot::SetNextLineStyle(color);
			ImPlot::PlotStems(
				std::format("Peak[{}]", idx).c_str(),
				m_PeakX[idx].data(),
				m_PeakY[idx].data(),
				m_PeakX[idx].size()
			);
		}
		ImPlot::EndPlot();
	}
}

void UPlotToolWindow::RenderAACurveDetail()
{
	if (!m_Future._Is_ready())
	{
		return;
	}

	ImGui::BulletText("Title: %s", m_AACurveData.Title.c_str());
	ImGui::BulletText("Step size: %.3f", m_AACurveData.StepSize);
	ImGui::BulletText("AATol: %.3f", m_AACurveData.AATol);
	ImGui::BulletText("Back light: %.3f", m_AACurveData.BackLight);
	ImGui::BulletText("ZDelay: %.3f", m_AACurveData.ZDelay);
	ImGui::BulletText("Min value: %.3f", m_AACurveData.MinValue);
	ImGui::BulletText("Max value: %.3f", m_AACurveData.MaxValue);

	ImGui::SeparatorText("Curves");

	const int num_of_curve = m_Attributes.size();
	ImGui::BulletText(std::format("Num of curve: {}", num_of_curve).c_str());
	for (int idx = 0; idx < num_of_curve; idx++)
	{
		if (ImGui::TreeNode(std::format("Curve[{}]", idx).c_str()))
		{
			ImGui::BulletText(std::format("Attribute: {}", m_Attributes[idx]).c_str());
			ImGui::BulletText(std::format("Peak: <{:.3f}, {:.3f}>", m_PeakX[idx].front(), m_PeakY[idx].front()).c_str());

			ImGui::Separator();
			ImGui::TreePop();
		}
	}
}

void UPlotToolWindow::RenderAABatchRunPlot()
{
	if (!m_Future._Is_ready())
	{
		return;
	}

	const int num_of_curve = m_DataX.size();
	float y = ImGui::GetWindowSize().y - 40;
	if (ImPlot::BeginPlot(m_FileName.c_str(), ImVec2(-1, y)))
	{
		for (int idx = 0; idx < num_of_curve; idx++)
		{
			const ImColor color = FPlotColor::Get(idx);

			ImPlot::SetNextLineStyle(color);
			ImPlot::PlotLine(std::format("ROI {}", m_Attributes[idx]).c_str(),
				m_DataX[idx].data(),
				m_DataY[idx].data(),
				m_DataX[idx].size(),
				ImPlotLineFlags_Shaded
			);

			ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
			ImPlot::SetNextLineStyle(color);
			ImPlot::PlotStems(
				std::format("Peak{}", m_Attributes[idx]).c_str(),
				m_PeakX[idx].data(),
				m_PeakY[idx].data(),
				m_PeakX[idx].size()
			);
		}
		ImPlot::EndPlot();
	}
}

void UPlotToolWindow::RenderAABatchRunDetail()
{
	if (!m_Future._Is_ready())
	{
		return;
	}

	const int num_of_curve = m_Attributes.size();
	ImGui::BulletText(std::format("Num of curve: {}", num_of_curve).c_str());
	for (int idx = 0; idx < num_of_curve; idx++)
	{
		if (ImGui::TreeNode(std::format("ROI{}", m_Attributes[idx]).c_str()))
		{
			ImGui::BulletText(std::format("Peak: <{:.3f}, {:.3f}>", m_PeakX[idx].front(), m_PeakY[idx].front()).c_str());

			ImGui::Separator();
			ImGui::TreePop();
		}
	}
}

void UPlotToolWindow::MakeAACurveData(const pugi::xml_document& Xml)
{
	ClearCurveData();

	// Get root and try get tag
	const pugi::xml_node& root = Xml.document_element();
	m_AACurveData.Title = root.child("Title").text().as_string();
	m_AACurveData.StepSize = root.child("stepsize").text().as_float();
	m_AACurveData.AATol = root.child("AA_Tol").text().as_float();
	m_AACurveData.BackLight = root.child("backlight").text().as_float();
	m_AACurveData.ZDelay = root.child("ZDelay").text().as_float();
	m_AACurveData.MinValue = root.child("Max").child("x").text().as_float();
	m_AACurveData.MaxValue = root.child("Min").child("x").text().as_float();

	// Curves
	const auto& Curves = root.children("Detail_Curve");
	for (const auto& curve : Curves)
	{
		// Push each points to vector and push to res 2D vector
		std::vector<float> x_data, y_data;
		const auto& points = curve.children("value");
		for (const auto& point : points)
		{
			x_data.push_back(point.child("x").text().as_float());
			y_data.push_back(point.child("y").text().as_float());
		}

		// Push points
		TPlot::AddPoints(m_DataX, m_DataY, std::move(x_data), std::move(y_data));

		// Push peak
		const auto& Peak = curve.child("Peak");
		float x = Peak.child("x").text().as_float();
		float y = Peak.child("y").text().as_float();
		TPlot::AddPoints(m_PeakX, m_PeakY, { x }, { y });

		// Push attribute
		const auto& Attribute = curve.child("Attribute");
		m_Attributes.push_back(Attribute.text().as_string());
	}

	m_ViewportWindow->SetRenderFunc([this]() { RenderAACurvePlot(); });
	m_DetailWindow->AddRenderFunc("Details", [this]() { RenderAACurveDetail(); });
}

void UPlotToolWindow::MakeAABatchRunData(const pugi::xml_document& Xml)
{
	ClearCurveData();

	auto root = Xml.child("AATestlistBatchRun");
	if (root.empty())
	{
		return;
	}

	// DAC
	{
	}

	// MTF
	{
		const auto& curves = root.child("CTestItemActiveAlign").child("MTF").children("MTFValueMap");
		for (const auto& curve : curves)
		{
			m_Attributes.push_back(curve.attribute("ROI").value());

			auto peak = curve.child("Peak");
			float z = 0, score = 0;
			z = peak.attribute("Z").as_float();
			score = peak.attribute("Score").as_float();
			TPlot::AddPoints(m_PeakX, m_PeakY, { z }, { score });

			std::vector<float> x_data, y_data;
			auto points = curve.children("value");
			for (const auto& point : points)
			{
				z = point.attribute("Z").as_float();
				score = point.attribute("Score").as_float();
				x_data.push_back(z);
				y_data.push_back(score);
			}
			TPlot::AddPoints(m_DataX, m_DataY, x_data, y_data);
		}
	}

	m_ViewportWindow->SetRenderFunc([this]() { RenderAABatchRunPlot(); });
	m_DetailWindow->AddRenderFunc("Details", [this]() { RenderAABatchRunDetail(); });
}