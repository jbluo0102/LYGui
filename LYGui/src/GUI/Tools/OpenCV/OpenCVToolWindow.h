#pragma once

#include <GUI/Tools/ImGuiToolWindow.h>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include <mutex>
#include <future>

class UOpenCVToolWindow
	: public UImGuiToolWindow
{
public:
	UOpenCVToolWindow();
protected:
	/** Override **/
	virtual void RenderTool() override;
	virtual void RenderDetail() override;
	virtual void RenderViewport() override;

	std::mutex m_TextureMutex;
	GLuint m_Texture;

	std::future<void> m_Future;
	std::atomic_bool m_NeedUpdate;

	std::string m_FilePathName;
	std::string m_FilePath;
	std::string m_FileName;

	cv::Mat m_OriginMat;
	cv::Mat m_GrayMat;
	cv::Mat m_SaveMat;
	cv::Mat m_ProcessMat;
	bool m_IsLoadImage;

	bool LoadImage(const std::string& FileName);
	void UpdateTexture(const cv::Mat& Mat, GLuint TextrueID);
	void OnUpdateImageStart();
	void OnUpdateImageEnd();
};
