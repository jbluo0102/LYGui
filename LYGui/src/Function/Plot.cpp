#include "pch.h"

#include "Plot.h"

TPlot::TPlot()
{
}

TPlot::~TPlot()
{
}

void TPlot::AddPoints(std::vector<std::vector<float>>& x_data, std::vector<std::vector<float>>& y_data, std::vector<float>& x, std::vector<float>& y)
{
	x_data.push_back(x);
	y_data.push_back(y);
}

void TPlot::AddPoints(std::vector<std::vector<float>>& x_data, std::vector<std::vector<float>>& y_data, std::vector<float>&& x, std::vector<float>&& y)
{
	x_data.push_back(std::move(x));
	y_data.push_back(std::move(y));
}