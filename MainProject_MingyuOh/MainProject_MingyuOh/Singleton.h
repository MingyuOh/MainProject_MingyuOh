#pragma once

#include <memory>
#include <mutex>

template<typename T>
class Singleton
{
private:
	static std::shared_ptr<T> m_pInstance;
	static std::once_flag m_onceFlag;
private:
	struct Deleter
	{
		void operator()(T* instance)
		{
			delete instance;
		}
	};
protected:
	Singleton() = default;
	virtual ~Singleton() = default;
public:
	Singleton(const Singleton&) = delete;
	Singleton(Singleton&&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	Singleton& operator=(Singleton&&) = delete;
public:
	static std::shared_ptr<T>& GetInstance()
	{
		std::call_once(m_onceFlag, []()
			{
				Singleton<T>::m_pInstance.reset(new T);
			});

		return m_pInstance;
	}
};

template<typename T>
std::shared_ptr<T>
Singleton<T>::m_pInstance = std::shared_ptr<T>(nullptr);

template<typename T>
std::once_flag Singleton<T>::m_onceFlag;
