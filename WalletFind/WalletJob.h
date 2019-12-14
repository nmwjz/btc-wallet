#pragma once

#include <mutex>
#include <list>

#include "WalletJobItem.h"

class WalletJob
{
public:
	WalletJob::WalletJob()
	{
	};

	WalletJob::~WalletJob()
	{
	};

	//查找一个指定的Job
	WalletJobItem * getItem(unsigned int iID)
	{
		m_lock.lock();
		for (std::list<WalletJobItem *>::iterator oITTmp = lstItem.begin(); oITTmp != lstItem.end(); )
		{
			WalletJobItem * pTask = *oITTmp;
			if (pTask->getID() == iID)
			{
				m_lock.unlock();
				return pTask;
			}
			++oITTmp;
		}
		m_lock.unlock();
		return NULL;
	};
	//插入一个元素
	void pushItem(WalletJobItem * pItem)
	{
		m_lock.lock();
		lstItem.push_back(pItem);
		m_lock.unlock();
	};
	//删除一个元素
	void popItem(unsigned int iID)
	{
		m_lock.lock();
		for (std::list<WalletJobItem *>::iterator oITTmp = lstItem.begin(); oITTmp != lstItem.end(); )
		{
			WalletJobItem * pTask = *oITTmp;
			if ( pTask->getID() == iID)
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
	std::list<WalletJobItem *> lstItem;
	std::list<WalletJobItem *>::const_iterator oIT;
};

