#include "pch.h"
#include "OpenCV.h"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/ocl.hpp>

#include <imgui.h>
#include <imgui_extension.h>

std::mutex TOpenCV::s_TextureMutex;

bool TOpenCV::IsValidMat(const cv::Mat& mat)
{
	return mat.rows != 0 && mat.cols != 0;
}

void TOpenCV::Blur(const cv::Mat& in, cv::Mat& out, const int size)
{
	// cv::blur(in, out, cv::Size(size, size));
	cv::medianBlur(in, out, size % 2 ? size : size + 1);
}

void TOpenCV::Threshold(const cv::Mat& in, cv::Mat& out, const int thresh, const int max_value, const int type)
{
	cv::threshold(in, out, thresh, max_value, type);
}

void TOpenCV::NoiseCancel(const cv::Mat& in, cv::Mat& out, const int size, const int iterate)
{
	const cv::Mat& kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Point(size, size));
	cv::morphologyEx(in, out, cv::MORPH_HITMISS, kernel, cv::Point(-1, -1), iterate);
}

void TOpenCV::Dilate(const cv::Mat& in, cv::Mat& out, const int size, const int iterate)
{
	const cv::Mat& kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Point(size, size));
	cv::dilate(in, out, kernel, cv::Point(-1, -1), iterate);
}

std::vector<std::vector<cv::Point>> TOpenCV::FindContours(const cv::Mat& mat)
{
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(mat, contours, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
	return contours;
}

void TOpenCV::DrawContour(
	cv::Mat& mat,
	const std::vector<std::vector<cv::Point>>& contours,
	int contour_idx, const cv::Scalar& color, const int thickness)
{
	cv::drawContours(mat, contours, contour_idx, color, thickness);
}

float TOpenCV::CalculateFlare(const cv::Mat& mat, const cv::Point2f& flare_center, const float flare_size, const float offset)
{
	const std::vector<cv::Point2f> flare_centers = {
		{ flare_center.x, flare_center.y }, // mid
		{ flare_center.x - offset, flare_center.y }, // left
		{ flare_center.x + offset, flare_center.y }, // right
		{ flare_center.x, flare_center.y - offset }, // top
		{ flare_center.x, flare_center.y + offset } // bottom
	};

	std::vector<float> mean_values;
	mean_values.reserve(4);

	const int row = mat.rows;
	const int col = mat.cols;
	for (const auto& point : flare_centers)
	{
		cv::Rect2f flare_roi(point.x - flare_size / 2.f, point.y - flare_size / 2.f, flare_size, flare_size);

		flare_roi.x = std::clamp(static_cast<int>(flare_roi.x), 0, col - 1);
		flare_roi.y = std::clamp(static_cast<int>(flare_roi.y), 0, row - 1);
		flare_roi.width = std::clamp(flare_roi.width, 0.f, mat.cols - flare_roi.x);
		flare_roi.height = std::clamp(flare_roi.height, 0.f, mat.rows - flare_roi.y);

		mean_values.emplace_back(cv::mean(mat(flare_roi))[0]);
	}

	return mean_values[0] * 4.f / std::accumulate(mean_values.begin() + 1, mean_values.end(), 0.f);
}

std::pair<cv::Point2f, float> TOpenCV::FindContourCenter(const std::vector<cv::Point>& contour)
{
	cv::Point2f center;
	float radius;
	cv::minEnclosingCircle(contour, center, radius);

	return { center, radius };
}

cv::Point2f TOpenCV::FindNearestCenter(const cv::Point2f& roi_center, const std::vector<cv::Point2f>& centers)
{
	cv::Point2f nearest_center;
	double min_distance = FLT_MAX;
	for (const auto& center : centers)
	{
		double distance = cv::norm(roi_center - center);
		if (distance < min_distance)
		{
			min_distance = distance;
			nearest_center = center;
		}
	}
	return nearest_center;
}

void TOpenCV::EqualizeHist(const cv::Mat& in, cv::Mat& out)
{
	cv::equalizeHist(in, out);
}

void TOpenCV::AdjustBrightnessWithMask(const cv::Mat& in, cv::Mat& out, const float RadiusX, const float RadiusY, const float Thresh, const int type)
{
	if (in.empty())
	{
		return;
	}

	cv::Mat mask = cv::Mat::zeros(in.size(), in.type());

	cv::ellipse(mask, cv::Point(in.cols / 2, in.rows / 2), cv::Size(RadiusX, RadiusY), 0, 0, 360, cv::Scalar(255), -1);
	cv::threshold(in, out, Thresh, 255, type);
	in.copyTo(out, mask);
}

void TOpenCV::UpdateImage(const cv::Mat& mat, GLuint texture)
{
	std::lock_guard lock(s_TextureMutex);

	/*glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
		mat.cols, mat.rows,
		0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
		mat.ptr());*/

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mat.cols, mat.rows, GL_RED, GL_UNSIGNED_BYTE, mat.data);
	glBindTexture(GL_TEXTURE_2D, 0);
}