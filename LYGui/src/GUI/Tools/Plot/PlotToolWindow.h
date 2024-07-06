#pragma once
#include <GUI/Tools/ImGuiToolWindow.h>

#include <Function/Plot.h>
#include <PugiXml/pugixml.hpp>
#include <future>

class UPlotToolWindow :
	public UImGuiToolWindow
{
	std::future<void> m_Future;

	pugi::xml_document m_XmlDocument;

	FAACurve m_AACurveData;
	std::string m_FileName;
	std::string m_PathName;
	std::vector<std::vector<float>> m_DataX;
	std::vector<std::vector<float>> m_DataY;
	std::vector<std::vector<float>> m_PeakX;
	std::vector<std::vector<float>> m_PeakY;
	std::vector<std::string> m_Attributes;
	pugi::xml_parse_result m_IsLoadFile;

	/** Clear vector data **/
	void ClearCurveData();
public:
	UPlotToolWindow();
protected:
	/** Override **/
	virtual void RenderTool() override;
	virtual void RenderDetail() override;
	virtual void RenderViewport() override;

	/** Render data **/
	void RenderAACurvePlot();
	void RenderAACurveDetail();

	void RenderAABatchRunPlot();
	void RenderAABatchRunDetail();

	/** Data function **/
	void MakeAACurveData(const pugi::xml_document& Xml);
	void MakeAABatchRunData(const pugi::xml_document& Xml);
};
