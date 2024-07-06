#include "pch.h"
#include "Logger.h"

ULogger::ULogger()
{
}

ULogger& ULogger::GetInstance()
{
	static ULogger instance;
	return instance;
}

const std::list<std::pair<ImColor, std::string>>& ULogger::GetHistories()
{
	return s_Histories;
}

void ULogger::Log(const unsigned int status, std::string_view message)
{
	std::lock_guard lock(m_LogMutex);

	const auto& local_time = std::chrono::get_tzdb().current_zone()->to_local(std::chrono::system_clock::now());
	std::string local_time_str = std::format("{}", local_time).substr(11, 8);

	if (s_Histories.size() > 1024)
	{
		s_Histories.pop_front();
	}

	ImColor color;
	switch (static_cast<unsigned int>(status))
	{
	case Normal:
		color = FColor::Normal;
		break;
	case Success:
		color = FColor::Green;
		break;
	case Warning:
		color = FColor::Yellow;
		break;
	case Error:
		color = FColor::Red;
		break;
	default:
		break;
	}

	s_Histories.emplace_back(color, std::format("[{}]{}", local_time_str, message));
}