#pragma once

#include <vector>

#include "GeneralFunctions.h"

enum
{
	LOG_BUFFER_SIZE = 2048
};

class LogBuffer : public Singleton<LogBuffer>
{
protected:
	std::vector<std::pair<AtomicBool, LogData>> m_qBuffer;
	int m_WriteIndex;
	int m_ReadIndex;
	bool m_bExit;

	std::thread m_ConsumerThread;
public:
	std::shared_ptr<GeneralFunctions> m_pGeneralFunctions;

public:
	void Init();

	void Run();
	void StopThreads();

	void Producer(const std::string& fileName, const std::string& data, const eLogFileExtension& logFileExtension = eLogFileExtension::LOG);

private:
	void Consumer();

public:
	LogBuffer();
	~LogBuffer();
};