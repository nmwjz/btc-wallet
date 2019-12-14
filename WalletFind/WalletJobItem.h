#pragma once
class WalletJobItem
{
public:
	WalletJobItem::WalletJobItem()
	{
	}
	WalletJobItem::~WalletJobItem()
	{
	}

	void setID(unsigned int iID)
	{
		iJobID = iID;
	};
	unsigned int getID()
	{
		return iJobID;
	};

	void setTaskCount(unsigned int iCount)
	{
		iTaskCount = iCount;
	};
	unsigned int getTaskCount()
	{
		return iTaskCount;
	};

	void setFinishCount(unsigned int iCount)
	{
		iFinishCount = iCount;
	};
	unsigned int getFinishCount()
	{
		return iFinishCount;
	};

private:
	unsigned int iJobID       = 0;
	unsigned int iTaskCount   = 0;
	unsigned int iFinishCount = 0;
};

