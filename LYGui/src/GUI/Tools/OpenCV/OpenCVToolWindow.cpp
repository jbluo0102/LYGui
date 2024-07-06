#include "pch.h"
#include "OpenCVToolWindow.h"

#include "Function/OpenCV.h"

UOpenCVToolWindow::UOpenCVToolWindow()
	: UImGuiToolWindow("OpenCV"), m_IsLoadImage(false)
{
	m_Future = GApp::ThreadPool.submit([]() {});

	m_NeedUpdate = false;

	glGenTextures(1, &m_Texture);
	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
		m_SaveMat.cols, m_SaveMat.rows,
		0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
		m_SaveMat.ptr());
}

void UOpenCVToolWindow::RenderTool()
{
	UImGuiToolWindow::RenderTool();

	// Load image
	if (ImGui::CollapsingHeader("Operation", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::TreeNodeExColored(FColor::Red, "Important", ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf))
		{
			ImGui::BulletTextColored(FColor::Red, "Image will convert to gray scale");
			ImGui::BulletTextColored(FColor::Red, "Need to save image after each step");
			ImGui::Separator();
			ImGui::TreePop();
		}

		if (ImGui::Button("Load Image", m_Future._Is_ready()))
		{
			ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose Image", ".*", ".");
		}

		if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				m_Future = GApp::ThreadPool.submit([this]() {
					FTimer timer("Load Image");
					m_FilePathName = ImGuiFileDialog::Instance()->GetFilePathName();
					m_FilePath = ImGuiFileDialog::Instance()->GetCurrentPath();
					m_FileName = m_FilePathName.substr(m_FilePathName.rfind('\\') + 1);

					m_IsLoadImage = LoadImage(m_FilePathName);
					if (m_IsLoadImage)
					{
						OnUpdateImageStart();
						LOG(Success, std::format("File[{}] load success", m_FileName).c_str());
					}
					else
					{
						LOG(Error, std::format("File[{}] load fail", m_FileName).c_str());
					}
					});
			}

			ImGuiFileDialog::Instance()->Close();
		}

		// Image operation
		{
			// Save
			{
				if (ImGui::Button("Save Current Image", m_Future._Is_ready()))
				{
					try
					{
						FTimer timer("Save Image");
						m_SaveMat = m_ProcessMat.clone();
					}
					catch (const std::exception& e)
					{
						LOG(Error, std::format("Save Image: {}", e.what()));
					}
				}
			}

			// Undo
			ImGui::SameLine();
			{
				if (ImGui::Button("Undo Image", m_Future._Is_ready()))
				{
					try
					{
						FTimer timer("Undo Image");
						UpdateTexture(m_SaveMat, m_Texture);
					}
					catch (const std::exception& e)
					{
						LOG(Error, std::format("Undo Image: {}", e.what()));
					}
				}
			}

			// Reset
			ImGui::SameLine();
			{
				if (ImGui::Button("Reset Image", m_Future._Is_ready()))
				{
					try
					{
						FTimer timer("Reset Image");
						m_SaveMat = m_GrayMat.clone();
						UpdateTexture(m_SaveMat, m_Texture);
					}
					catch (const std::exception& e)
					{
						LOG(Error, std::format("Reset Image: {}", e.what()));
					}
				}
			}
		}

		ImGui::Separator();
	}

	// Image processing
	if (ImGui::CollapsingHeader("Image Processing", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// Mask filter
		{
			ImGui::SeparatorText("Mask");

			static int type = 1;
			static const std::array<const char*, 2> item = { "BINARY", "BINARY_INV" };

			ImGui::Combo("Type##Mask", &type, item.data(), static_cast<int>(item.size()));

			static int radius;
			static int radius_x;
			static int radius_y;
			static int thresh;
			bool is_modify = false;

			if (is_modify |= ImGui::DragInt("Radius##Mask", &radius, 1.f, 0, 5000))
			{
				radius_x = radius;
				radius_y = radius;
			}

			is_modify |= ImGui::DragInt("Radius X##Mask", &radius_x, 1.f, 0, 5000);
			is_modify |= ImGui::DragInt("Radius Y##Mask", &radius_y, 1.f, 0, 5000);
			is_modify |= ImGui::DragInt("Thresh##Mask", &thresh, 1.f, 0, 255);
			if (is_modify && m_Future._Is_ready())
			{
				try
				{
					FTimer timer("Mask");
					TOpenCV::AdjustBrightnessWithMask(m_SaveMat, m_ProcessMat, radius_x, radius_y, thresh, type);
					UpdateTexture(m_ProcessMat, m_Texture);
				}
				catch (const std::exception& e)
				{
					LOG(Error, std::format("Thresold: {}", e.what()));
				}
			}
		}

		// Threshold
		{
			static int type = 1;
			static const std::array<const char*, 2> item = { "BINARY", "BINARY_INV" };

			ImGui::SeparatorText("Threshold");
			ImGui::Combo("Type##Threshold", &type, item.data(), static_cast<int>(item.size()));

			static int thresh;
			if (ImGui::DragInt("Thresh##Threshold", &thresh, 1.f, 0, 255) && m_Future._Is_ready())
			{
				try
				{
					FTimer timer("Threshold");
					cv::threshold(m_SaveMat, m_ProcessMat, thresh, 255, type);
					UpdateTexture(m_ProcessMat, m_Texture);
				}
				catch (const std::exception& e)
				{
					LOG(Error, std::format("Thresold: {}", e.what()));
				}
			}
		}

		// EqualizeHist
		{
			if (ImGui::Button("Equalize Hist", m_Future._Is_ready()))
			{
				try
				{
					FTimer timer("Equalize Hist");
					TOpenCV::EqualizeHist(m_SaveMat, m_ProcessMat);
					UpdateTexture(m_ProcessMat, m_Texture);
				}
				catch (const std::exception& e)
				{
					LOG(Error, std::format("Equalize Hist", e.what()));
				}
			}
		}

		// Blur
		{
			ImGui::SeparatorText("Blur");

			static int blur;
			ImGui::PushID("Blur");
			if (ImGui::DragInt("Iterate##Blur", &blur, 1.f, 0, 64) && m_Future._Is_ready())
			{
				try
				{
					FTimer timer("Blur");
					TOpenCV::Blur(m_SaveMat, m_ProcessMat, blur);
					UpdateTexture(m_ProcessMat, m_Texture);
				}
				catch (const std::exception& e)
				{
					LOG(Error, std::format("Blur: {}", e.what()));
				}
			}
			ImGui::PopID();
		}

		// Noise cancel
		{
			static int iterate;

			ImGui::SeparatorText("Noise Cancel");
			if (ImGui::DragInt("Iterate##Noise Cancel", &iterate, 1.f, 0, 64) && m_Future._Is_ready())
			{
				try
				{
					FTimer timer("Noise Cancel");
					TOpenCV::NoiseCancel(m_SaveMat, m_ProcessMat, 2, iterate);
					UpdateTexture(m_ProcessMat, m_Texture);
				}
				catch (const std::exception& e)
				{
					LOG(Error, std::format("Noise Cancel: {}", e.what()));
				}
			}
		}

		// Dilate
		{
			static int iterate;

			ImGui::SeparatorText("Dilate");
			if (ImGui::DragInt("Iterate##Dilate", &iterate, 1.f, 0, 64) && m_Future._Is_ready())
			{
				try
				{
					FTimer timer("Dilate");
					TOpenCV::Dilate(m_SaveMat, m_ProcessMat, 2, iterate);
					UpdateTexture(m_ProcessMat, m_Texture);
				}
				catch (const std::exception& e)
				{
					LOG(Error, std::format("Dilate: {}", e.what()));
				}
			}
		}

		// Contour
		if (ImGui::CollapsingHeader("Contours", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::BulletTextColored(FColor::Yellow, "Notice: Need to do pre-processing, like threshold etc.");
			ImGui::Separator();

			if (ImGui::Button("Find Contours", m_Future._Is_ready()))
			{
				m_Future = GApp::ThreadPool.submit([this]() {
					try
					{
						FTimer timer("Find Contours");

						cv::Mat contours_mat;
						std::vector<std::vector<cv::Point>> contours;
						contours_mat = cv::Mat(m_GrayMat.size(), CV_8UC1, CV_SCALAR_DARK);
						contours = TOpenCV::FindContours(m_SaveMat);

						const int contour_num = static_cast<int>(contours.size());
						for (int idx = 0; idx < contour_num; idx++)
						{
							TOpenCV::DrawContour(contours_mat, contours, idx);
						}

						m_DetailWindow->AddRenderFunc("Contours", [contour_num]() {
							if (ImGui::CollapsingHeader("Contours", ImGuiTreeNodeFlags_DefaultOpen))
							{
								ImGui::BulletText("Num: %d", contour_num);
								for (int idx = 0; idx < contour_num; idx++)
								{
									ImGui::BulletText("contour[%d]", idx);
								}

								ImGui::Separator();
							}
							});

						m_SaveMat = contours_mat.clone();
						OnUpdateImageStart();
					}
					catch (const std::exception& e)
					{
						LOG(Error, std::format("Find Contours: {}", e.what()));
					}
					});

				ImGui::Separator();
			}

			ImGui::Separator();
		}

		// Flare
		if (ImGui::CollapsingHeader("Flare"))
		{
			static std::vector<float> flare_values;
			static std::vector<std::array<int, 2>> rois = { { 160, 60 }, { 1650, 50 }, { 1550, 800 }, { 50, 800 }, { 900, 450 } };
			static bool is_calculate = false;
			static int roi_size = 200;
			static int flare_size = 50;
			static int offset = 100;

			ImGui::BulletTextColored(FColor::Yellow, "Notice: Need to do pre-processing, like threshold etc.");
			ImGui::Separator();

			if (ImGui::Button("Calculate Flare", m_Future._Is_ready()))
			{
				cv::Mat flare_mat = m_GrayMat.clone();
				std::vector<std::vector<cv::Point>> contours;
				std::vector<cv::Point2f> contour_centers;
				std::vector<cv::Point2f> roi_centers;
				std::vector<cv::Point2f> flare_centers;

				flare_centers.resize(rois.size());
				flare_values.reserve(rois.size());
				flare_values.clear();
				flare_values.shrink_to_fit();

				try
				{
					// Pre-process
					FTimer timer("Calculate Flare");

					// Contour and it's centers
					contours = TOpenCV::FindContours(m_SaveMat);
					contour_centers.reserve(contours.size());
					for (const auto& contour : contours)
					{
						contour_centers.emplace_back(TOpenCV::FindContourCenter(contour).first);
					}

					// Find nearest center between roi_center and contour_center
					for (const auto& roi : rois)
					{
						const cv::Point2f roi_center(roi[0] + roi_size / 2.f, roi[1] + roi_size / 2.f);
						flare_centers.emplace_back(TOpenCV::FindNearestCenter(roi_center, contour_centers));

						cv::Rect rect(roi[0], roi[1], roi_size, roi_size);
						cv::rectangle(flare_mat, rect, CV_SCALAR_WHITE, 2);
					}

					// Calculate flare
					for (const auto& flare_center : flare_centers)
					{
						flare_values.emplace_back(TOpenCV::CalculateFlare(m_GrayMat, flare_center, flare_size, offset));

						const std::vector<cv::Point2f> flare_rois = {
							{ flare_center.x, flare_center.y }, // mid
							{ flare_center.x - offset, flare_center.y }, // left
							{ flare_center.x + offset, flare_center.y }, // right
							{ flare_center.x, flare_center.y - offset }, // top
							{ flare_center.x, flare_center.y + offset } // bottom
						};

						for (const auto& flare_roi : flare_rois)
						{
							const float radius = flare_size / 2.f;
							cv::Rect2f roi(flare_roi.x - radius, flare_roi.y - radius, flare_size, flare_size);
							cv::rectangle(flare_mat, roi, CV_SCALAR_WHITE, 2);
						}
					}

					is_calculate = true;

					m_ProcessMat = flare_mat.clone();
					UpdateTexture(m_ProcessMat, m_Texture);
				}
				catch (const std::exception& e)
				{
					is_calculate = false;
					LOG(Error, std::format("Calculate Flare: {}", e.what()));
				}
			}

			// Clone the new mat if modify the roi
			ImGui::SeparatorText("ROI Parameters");
			ImGui::DragInt("ROI Size", &roi_size, 1.f, 1, 500);
			ImGui::DragInt("Flare Size", &flare_size, 1.f, 1, 500);
			ImGui::DragInt("Offset", &offset, 1.f, 0, 500);

			ImGui::SeparatorText("ROI Location");
			if (ImGui::Button("Add ROI"))
			{
				rois.push_back({ 0, 0 });
			}

			ImGui::SameLine();

			if (ImGui::Button("Delete All ROI"))
			{
				rois.clear();
				rois.shrink_to_fit();
			}

			const int roi_num = rois.size();
			for (int idx = 0; idx < roi_num; idx++)
			{
				ImGui::DragInt2(std::format("ROI[{}]", idx).c_str(), rois[idx].data(), 1.f, 0, 5000);

				ImGui::SameLine();

				if (ImGui::Button(std::format("Delete ##{}", idx).c_str()))
				{
					rois.erase(rois.begin() + idx);
					break;
				}
			}

			if (is_calculate)
			{
				m_DetailWindow->AddRenderFunc("Flare", []() {
					if (ImGui::CollapsingHeader("Flare Values", ImGuiTreeNodeFlags_DefaultOpen))
					{
						int idx = 0;
						for (const auto& value : flare_values)
						{
							ImGui::BulletTextColored(value > 1.f ? FColor::Red : FColor::Normal,
								std::format("flare[{}]: {:.4f}", idx++, value).c_str());
						}
						ImGui::Separator();
					}
					});
			}

			ImGui::Separator();
		}
	}
}
void UOpenCVToolWindow::RenderDetail()
{
	if (ImGui::CollapsingHeader("Basic Info", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::BulletText(std::format("Name: {}", m_FileName).c_str());
		ImGui::BulletText(std::format("Path: {}", m_FilePath).c_str());
		ImGui::BulletText(std::format("Size: {} KB", m_OriginMat.total() * m_OriginMat.elemSize() / 1000).c_str());
		ImGui::BulletText(std::format("Dimensions: {} x {}", m_OriginMat.cols, m_OriginMat.rows).c_str());

		static int depth = m_OriginMat.depth() == CV_8U ? 8 : m_OriginMat.depth() == CV_16U ? 16 : m_OriginMat.depth() == CV_32F ? 32 : 0;
		static int bits = m_OriginMat.channels() * depth;
		ImGui::BulletText(std::format("Bit depth: {}", bits).c_str());
		ImGui::Separator();
	}
}

void UOpenCVToolWindow::RenderViewport()
{
	static ImVec2 window_size;
	window_size = ImGui::GetWindowSize() - ImVec2(25.f, 50.f);
	ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(m_Texture)), window_size);

	if (m_Future._Is_ready())
	{
		if (m_NeedUpdate)
		{
			UpdateTexture(m_SaveMat, m_Texture);
			OnUpdateImageEnd();
		}
	}
	else
	{
		ImGui::SetCursorPos(ImGui::GetWindowSize() / 2.f);
		ImGui::Spinner("##Image Processing", 10.f, 6, FColor::Blue);
	}
}

bool UOpenCVToolWindow::LoadImage(const std::string& FileName)
{
	try
	{
		m_OriginMat = cv::imread(FileName, cv::IMREAD_UNCHANGED);
	}
	catch (const std::exception& e)
	{
		LOG(Error, std::format("{}", e.what()));
		return false;
	}

	if (!TOpenCV::IsValidMat(m_OriginMat))
	{
		return false;
	}

	m_GrayMat = cv::imread(FileName, cv::IMREAD_GRAYSCALE);
	if (!TOpenCV::IsValidMat(m_GrayMat))
	{
		return false;
	}

	m_SaveMat = m_GrayMat.clone();
	m_ProcessMat = m_GrayMat.clone();

	return true;
}

void UOpenCVToolWindow::UpdateTexture(const cv::Mat& Mat, GLuint TextrueID)
{
	glBindTexture(GL_TEXTURE_2D, TextrueID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
		Mat.cols, Mat.rows,
		0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
		Mat.ptr());
}

void UOpenCVToolWindow::OnUpdateImageStart()
{
	m_NeedUpdate = true;
}

void UOpenCVToolWindow::OnUpdateImageEnd()
{
	m_NeedUpdate = false;
}