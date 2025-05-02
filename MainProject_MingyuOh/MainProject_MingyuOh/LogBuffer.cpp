#include "pch.h"
#include "LogBuffer.h"

void LogBuffer::Init()
{
	m_pGeneralFunctions = GeneralFunctions::GetInstance();
}

void LogBuffer::Run()
{
	m_qBuffer.resize(LOG_BUFFER_SIZE);
	for (auto& buffer : m_qBuffer)
	{
		buffer.first.store(false);
	}

	m_WriteIndex = 0;
	m_ReadIndex = 0;
	m_bExit = false;

	m_ConsumerThread = std::thread(&LogBuffer::Consumer, this);
}

void LogBuffer::StopThreads()
{
	m_bExit = true;

	if (m_ConsumerThread.joinable())
	{
		m_ConsumerThread.join();
	}
}

void LogBuffer::Producer(const std::string& fileName, const std::string& data, const eLogFileExtension& logFileExtension)
{
	if (fileName.empty() || data.empty())
	{
		return;
	}

	int index = m_WriteIndex;
	int nextIndex = (index + 1) % LOG_BUFFER_SIZE;

	LogData temp(fileName, data, logFileExtension);
	std::swap(m_qBuffer[index].second, temp);
	m_qBuffer[index].first.store(true);

	m_WriteIndex = nextIndex;
}

void LogBuffer::Consumer()
{
	while (m_bExit == false) 
	{
		int index = m_ReadIndex;

		if (m_qBuffer[index].first.load() == false)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			continue;
		}

		LogData writeData = m_qBuffer[index].second;
		m_pGeneralFunctions->AddLog(writeData.fileName, writeData.data, writeData.logFileExtension);

		m_qBuffer[index].first.store(false);
		m_qBuffer[index].second.Clear();
		m_ReadIndex = (index + 1) % LOG_BUFFER_SIZE;

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

LogBuffer::LogBuffer() : m_bExit(false), m_ReadIndex(0), m_WriteIndex(0)
{
}

LogBuffer::~LogBuffer()
{
}