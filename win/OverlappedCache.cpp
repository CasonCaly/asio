#include "OverlappedCache.h"

OverlappedCache::OverlappedCache()
{

}

OVERLAPPED_EX* OverlappedCache::add(AsynSocketChannel* channel, PendingTaskType type)
{
	m_lock.lock();
	OVERLAPPED_EX* overlapped = NULL;
	if (m_unusedCache.size() == 0)
	{
		overlapped = (OVERLAPPED_EX*)malloc(sizeof(OVERLAPPED_EX));
		m_cache.insert(overlapped);
	}
	else
	{
		overlapped = m_unusedCache.front();
		m_cache.insert(overlapped);
		m_unusedCache.pop_front();
	}

	memset(overlapped, 0, sizeof(OVERLAPPED_EX));
	overlapped->channel = channel;
	overlapped->taskType = type;
	m_lock.unlock();
	return overlapped;
}

void OverlappedCache::remove(OVERLAPPED_EX* overlapped)
{
	m_lock.lock();
	auto target = m_cache.find(overlapped);
	if (target != m_cache.end())
		m_cache.erase(target);

	m_unusedCache.push_back(overlapped);
	m_lock.unlock();
}