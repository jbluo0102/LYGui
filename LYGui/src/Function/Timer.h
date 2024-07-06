#pragma once

#include <chrono>
#include <Function/Logger.h>

/**
* For Analyzing code performance
* Timer start if construct the object
* Call stop() to get the duration from start time
* If want to covert std::string to CString -> _T("%s", timer.name.c_str())
* Return type: long long (ms)
**/
class FTimer
{
	using ClockType = std::chrono::steady_clock;

	/** Funciton name **/
	std::string Name;

	/** Set start time if construct **/
	const ClockType::time_point StartTime;

public:
	/** Constructor/Destructor **/
	explicit FTimer(std::string name, const bool show_debug = true) :
		Name(std::move(name)), StartTime(ClockType::now())
	{
	}
	~FTimer()
	{
		const auto end = ClockType::now();
		const auto duration = end - StartTime;
		const auto total_time = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

		LOG(Warning, std::format("{} cost {} ms", Name, total_time));
	}

	FTimer(const FTimer& obj) = delete;
	FTimer(const FTimer&& obj) = delete;

	/** Operator **/
	FTimer& operator =(const FTimer& obj) = delete;
	FTimer& operator =(const FTimer&& obj) = delete;
};