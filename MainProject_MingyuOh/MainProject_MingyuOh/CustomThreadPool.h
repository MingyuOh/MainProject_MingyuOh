#pragma once

#include <thread>
#include <queue>
#include <vector>
#include <condition_variable>
#include <functional>
#include <atomic>

class CustomThreadPool
{
private:
	std::vector<std::thread> m_Threads;
	std::queue<std::function<void()>> m_Tasks;
	std::mutex m_Mutex;
	std::condition_variable m_CV;
	std::atomic<bool> m_bStop;

	int m_nThreadCount;
public:
	void InitThread(int numOfThreads);
	void StartThreads();
	void StopThreads();

	void EnqueueTask(const std::function<void()>& task);
public:
	CustomThreadPool();
	~CustomThreadPool();
};