#pragma once

#include <string>
#include <vector>

enum class EAAData {
	AACurve,
	AABatchRun
};

struct FAACurve
{
	std::string Title;
	float StepSize;
	float AATol;
	float BackLight;
	float ZDelay;
	float MinValue;
	float MaxValue;
};

class TPlot
{
public:
	TPlot();
	~TPlot();

	static void AddPoints(
		std::vector<std::vector<float>>& x_data,
		std::vector<std::vector<float>>& y_data,
		std::vector<float>& x,
		std::vector<float>& y);

	static void AddPoints(
		std::vector<std::vector<float>>& x_data,
		std::vector<std::vector<float>>& y_data,
		std::vector<float>&& x,
		std::vector<float>&& y);
};
