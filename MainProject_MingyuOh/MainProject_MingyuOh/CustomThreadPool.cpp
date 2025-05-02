#include "pch.h"
#include "CustomThreadPool.h"

void CustomThreadPool::InitThread(int numOfThreads)
{
	StopThreads();

	m_nThreadCount = numOfThreads;
}

void CustomThreadPool::StartThreads()
{
	if (m_Threads.empty() == false)
	{
		return;
	}

	m_bStop = false;
	for (int i = 0; i < m_nThreadCount; i++)
	{
		m_Threads.emplace_back([this]
		{
			while (true)
			{
				std::function<void()> task;
				{
					std::unique_lock<std::mutex> lock(m_Mutex);
					m_CV.wait(lock, [this] { return m_bStop == true || m_Tasks.empty() == false; });
					if (m_bStop == true)
					{
						return;
					}
					task = std::move(m_Tasks.front());
					m_Tasks.pop();
				}
				task();
			}
		});
	}
}

void CustomThreadPool::StopThreads()
{
	m_bStop = true;
	for (std::thread& thread : m_Threads)
	{
		if (thread.joinable())
		{
			std::unique_lock<std::mutex> lock(m_Mutex);
			m_CV.notify_all();
		}

		if (thread.joinable())
		{
			thread.join();
		}
	}
	m_Threads.clear();
}

void CustomThreadPool::EnqueueTask(const std::function<void()>& task)
{
	if (m_bStop == true)
	{
		return;
	}

	std::lock_guard<std::mutex> lock(m_Mutex);
	m_Tasks.push(task);

	m_CV.notify_one();
}

CustomThreadPool::CustomThreadPool() : m_nThreadCount(0), m_bStop(false)
{
}

CustomThreadPool::~CustomThreadPool()
{
}