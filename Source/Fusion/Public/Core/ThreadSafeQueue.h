#ifndef __FUSION_PUBLIC_CORE_THREADSAFEQUEUE_H__
#define __FUSION_PUBLIC_CORE_THREADSAFEQUEUE_H__

#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <deque>

namespace fu {
namespace fusion {

template <typename T>
class ThreadSafeQueue
{
public:
	using iterator					= typename std::deque<T>::iterator;
	using const_iterator			= typename std::deque<T>::const_iterator;
	using reverse_iterator			= typename std::deque<T>::reverse_iterator;
	using const_reverse_iterator	= typename std::deque<T>::const_reverse_iterator;

	/// Construction
	ThreadSafeQueue() { }
	ThreadSafeQueue(const ThreadSafeQueue& other)
	{
		std::lock_guard<std::mutex> lock(other.m_Mutex);
		m_DataQueue = other.m_DataQueue;
	}

	void Push(T newValue)
	{
		std::lock_guard<std::mutex>	lock(m_Mutex);
		m_DataQueue.push_back(newValue);
		m_DataCondition.notify_one();
	}

	void WaitForPop(T& value)
	{
		std::unique_lock<std::mutex> lock(m_Mutex);
		m_DataCondition.wait(lock, [this]{ return !m_DataQueue.empty(); });
		value = m_DataQueue.front();
		m_DataQueue.pop();
	}

	T WaitAndPop()
	{
		std::unique_lock<std::mutex> lock(m_Mutex);
		m_DataCondition.wait(lock, [this]{ return !m_DataQueue.empty(); });
		T res = m_DataQueue.front();
		m_DataQueue.pop();
		return res;
	}

	bool TryPop(T& value)
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		if (m_DataQueue.empty())
			return false;
		value = m_DataQueue.front();
		m_DataQueue.pop();
		return true;
	}

	T TryPop()
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		if (m_DataQueue.empty())
			return T();
		T res = m_DataQueue.front();
		m_DataQueue.pop_front();
		return res;
	}

	bool Empty() const
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		return m_DataQueue.empty();
	}

	size_t Size()
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		return m_DataQueue.size();
	}

	void Clear()
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		std::deque<T> empty;
		std::swap(m_DataQueue, empty);
	}

	iterator Begin()
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		return m_DataQueue.begin();
	}

	iterator End()
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		return m_DataQueue.end();
	}

	const_iterator Begin() const
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		return m_DataQueue.begin();
	}

	const_iterator End() const
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		return m_DataQueue.end();
	}

	reverse_iterator RBegin()
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		return m_DataQueue.rbegin();
	}

	const_reverse_iterator RBegin() const
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		return m_DataQueue.rbegin();
	}

	reverse_iterator REnd()
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		return m_DataQueue.rend();
	}

	const_reverse_iterator REnd() const
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		return m_DataQueue.rend();
	}
private:
	mutable std::mutex 		m_Mutex;
	std::deque<T>			m_DataQueue;
	std::condition_variable	m_DataCondition;
};	///	!class ThreadSafeQueue
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_CORE_THREADSAFEQUEUE_H__