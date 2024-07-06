#pragma once

#include <mutex>
#include <list>
#include <string>
#include <imgui.h>

enum
{
	Normal,
	Success,
	Warning,
	Error
};

class ULogger
{
	ULogger();
	~ULogger() = default;

	/** Log's mutex **/
	std::mutex m_LogMutex;

	/** Log's history **/
	std::list<std::pair<ImColor, std::string>> s_Histories;

public:
	/** Return self **/
	static ULogger& GetInstance();

	/** Returns log history **/
	const std::list<std::pair<ImColor, std::string>>& GetHistories();

	/** Pushing message into history **/
	void Log(const unsigned int status, std::string_view message);
};

#define LOG(status, message) ULogger::GetInstance().Log(status, message)