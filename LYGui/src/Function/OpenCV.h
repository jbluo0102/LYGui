#pragma once

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>

#define CV_SCALAR_WHITE cv::Scalar(255, 255, 255)
#define CV_SCALAR_BLACK cv::Scalar(0, 0, 0)
#define CV_SCALAR_DARK cv::Scalar(20, 20, 20)

class TOpenCV
{
	static std::mutex s_TextureMutex;

public:

	static void UpdateImage(const cv::Mat& mat, GLuint texture);

	/** Return true if mat is not empty vector **/
	static bool IsValidMat(const cv::Mat& mat);

	/** OpenCV function **/
	static void Blur(const cv::Mat& in, cv::Mat& out, const int size = 1);
	static void Threshold(const cv::Mat& in, cv::Mat& out, const int thresh, const int max_value, const int type);
	static void NoiseCancel(const cv::Mat& in, cv::Mat& out, const int size, const int iterate);
	static void Dilate(const cv::Mat& in, cv::Mat& out, const int size, const int iterate);
	static void EqualizeHist(const cv::Mat& in, cv::Mat& out);
	static void AdjustBrightnessWithMask(const cv::Mat& in, cv::Mat& out, const float RadiusX, const float RadiusY, const float Thresh, const int type);

	static std::vector<std::vector<cv::Point>> FindContours(const cv::Mat& mat);

	static void DrawContour(
		cv::Mat& mat,
		const std::vector<std::vector<cv::Point>>& contours,
		int contour_idx,
		const cv::Scalar& color = CV_SCALAR_WHITE,
		const int thickness = 1);

	static float CalculateFlare(const cv::Mat& mat, const cv::Point2f& flare_center, const float flare_size, const float offset);

	static std::pair<cv::Point2f, float> FindContourCenter(const std::vector<cv::Point>& contour);

	static cv::Point2f FindNearestCenter(const cv::Point2f& roi_center, const std::vector<cv::Point2f>& centers);
};
