#pragma once

#include <mutex>
#include <list>

#include "WalletTaskItem.h"

class WalletTask
{
public:
	WalletTask()
	{
	};
	~WalletTask()
	{
	};

	//============================//
	//一组方法遍历任务项目(从后向前遍历)
	WalletTaskItem * getItemBegin()
	{
		m_lock.lock();
		if (lstItem.size() == 0)
		{
			m_lock.unlock();
			return NULL;
		}

		oIT = lstItem.begin();

		m_lock.unlock();
		return *oIT;
	};
	WalletTaskItem * getItemNext()
	{
		m_lock.lock();
		if (lstItem.size() == 0)
		{
			m_lock.unlock();
			return NULL;
		}
		if (lstItem.end() == oIT)
		{
			m_lock.unlock();
			return NULL;
		}

		oIT++;

		if (lstItem.end() == oIT)
		{
			m_lock.unlock();
			return NULL;
		}

		m_lock.unlock();
		return *oIT;
	};
	//获取第一个待处理的任务
	WalletTaskItem * getItemPending()
	{
		WalletTaskItem * pTaskOut = NULL;
		m_lock.lock();
		for (std::list<WalletTaskItem *>::iterator oITTmp = lstItem.begin(); oITTmp != lstItem.end(); ++oITTmp)
		{
			WalletTaskItem * pTask = *oITTmp;
			//判断当前任务是否属于待处理
			if (0 == pTask->getState())
			{
				pTaskOut = *oITTmp;
				break;
			}
		}
		m_lock.unlock();

		return pTaskOut;
	};

	//返回元素个数
	unsigned int getSize()
	{
		return (unsigned int)lstItem.size();
	};

	//插入一个元素
	void pushItem(WalletTaskItem * pItem)
	{
		m_lock.lock();
		lstItem.push_back(pItem);
		m_lock.unlock();
	};
	//删除一个元素，返回下一个任务
	void popItem(WalletTaskItem * pItem)
	{
		m_lock.lock();
		for (std::list<WalletTaskItem *>::iterator oITTmp = lstItem.begin(); oITTmp != lstItem.end(); )
		{
			WalletTaskItem * pTask = *oITTmp;
			if (pItem == *oITTmp)
			{
				oIT = lstItem.erase(oITTmp);

				m_lock.unlock();
				return;
			}
			++oITTmp;
		}
		m_lock.unlock();

		return;
	};

private:
	std::mutex m_lock;
	std::list<WalletTaskItem *> lstItem;
	std::list<WalletTaskItem *>::const_iterator oIT;
};
