#pragma once

#include <mutex>
#include <list>

#include "WalletResultItem.h"

class WalletResult
{
public:
	WalletResult()
	{
	};
	~WalletResult()
	{
	};

	//插入一个元素
	void pushItem(WalletResultItem * pItem)
	{
		m_lock.lock();
		lstItem.push_back(pItem);
		m_lock.unlock();
	};

	//=====================================
	WalletResultItem * getItemBegin()
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
	WalletResultItem * getItemNext()
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
	//获取第一个未处理的任务
	WalletResultItem * getItemPending()
	{
		WalletResultItem * pTaskOut = NULL;
		m_lock.lock();
		for (std::list<WalletResultItem *>::iterator oITTmp = lstItem.begin(); oITTmp != lstItem.end(); ++oITTmp)
		{
			WalletResultItem * pTask = *oITTmp;
			//判断当前任务是否属于未处理
			if (0 == pTask->getState())
			{
				pTaskOut = *oITTmp;
				break;
			}
		}
		m_lock.unlock();

		return pTaskOut;
	};

private:
	std::mutex m_lock;
	std::list<WalletResultItem *> lstItem;
	std::list<WalletResultItem *>::const_iterator oIT;
};

