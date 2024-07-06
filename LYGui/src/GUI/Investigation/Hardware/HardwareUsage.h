#pragma once

class UHardwareUsage
{
	UHardwareUsage();
	UHardwareUsage(const UHardwareUsage& hardware) = delete;
	UHardwareUsage(const UHardwareUsage&& hardware) = delete;

public:
	static UHardwareUsage& GetInstance();
	double GetCPUPercentage();
	double GetMemoryPercentage();
	void Render();
};
