#pragma once

#include <thread>
#include <list>
#include <mutex>

#include "WalletResultItem.h"
#include "WalletResult.h"
#include "WalletTask.h"

#include "../CryptoC/find-wallet-bin.h"
#include "../CryptoC/ecmult_gen_impl.h"
#include "../CryptoC/rmd160.h"
#include "../CryptoC/sha256.h"

#include "../CryptoOpenCL/wallet_find_opencl.h"

//业务处理子线程
//用于计算和验证一组私钥
class ThreadProcCryptoCL
{
public:
	ThreadProcCryptoCL()
	{
		//设置OpenCL文件
		pFileOpenCL12 = (char *)malloc( 512 );
		pFileOpenCL20 = (char *)malloc( 512 );

		//初始化OpenCL对象
		pWalletFind = new wallet_find_opencl();
		//初始化OpenCL平台
		pWalletFind->find_platform();

		//初始化椭圆曲线
		pContext = (secp256k1_ecmult_gen_context*)malloc(sizeof(secp256k1_ecmult_gen_context));
		secp256k1_ecmult_gen_context_build(pContext);

		//返回结果对象
		pWalletResult = new WalletResult();
		//计算任务对象
		pWalletTask = new WalletTask;
	};
	~ThreadProcCryptoCL()
	{
		free(pFileOpenCL12);
		free(pFileOpenCL20);

		//清除OpenCL平台
		pWalletFind->clean_buff();
		delete pWalletFind;

		free(pContext);

		//释放结果对象
		delete pWalletResult;
		//释放任务对象
		delete pWalletTask;
	};

	//启动线程
	void start()
	{
		//设置开始标志
		iFlagExit = 1;


		//启动处理线程
		oMainWorkThd = std::thread(&ThreadProcCryptoCL::run, this);
		oMainWorkThd.detach();
	};
	//停止线程
	void stop()
	{
		//设置退出标志
		iFlagExit = 0;

		//清除OpenCL平台
		pWalletFind->clean_buff();
		delete pWalletFind;
	};

	//设置OpenCL文件
	//参数：
	//  1、OpenCL文件，带有路径的文件名，整个长度小于500字节；
	void setFile(char *pOpenCL12, char *pOpenCL20)
	{
		if (pOpenCL12)
			strcpy(pFileOpenCL12, pOpenCL12);
		if (pOpenCL20)
			strcpy(pFileOpenCL20, pOpenCL20);

		//加载资源
		int iResult = pWalletFind->load_source(pFileOpenCL12, pFileOpenCL20);
	};
	//设置钱包数据
	//参数：
	//  1、pWalletData钱包数据；
	//  2、iWalletCount钱包数量
	void setWalletData(unsigned char *pWalletData, unsigned int iWalletCount)
	{
		this->pWalletData  = pWalletData;
		this->iWalletCount = iWalletCount;

		//设置钱包数据
		int iResult = pWalletFind->load_wallet(pWalletData, iWalletCount);
	};

	//添加业务线程的运行Data，添加任务
	//参数：
	//  1、JobID；
	//  2、任务ID；
	//  3、开始私钥；
	//  4、每线程的计算量；
	void addTaskData(unsigned int iJobID, unsigned int iTaskID, unsigned char *szPrivateKey, unsigned int iCalcCount)
	{
		WalletTaskItem * pTaskItem = new WalletTaskItem();
		pTaskItem->setJobID(iJobID);
		pTaskItem->setTaskID(iTaskID);
		pTaskItem->setPrivateStart(szPrivateKey);
		pTaskItem->setPrivateCount(iCalcCount);

		pWalletTask->pushItem(pTaskItem);
	};

	//判断线程是否在运行
	bool isRun()
	{
		return (1 == iFlagExit);
	};

	//获取结果列表
	WalletResult * getResult()
	{
		return pWalletResult;
	};
	//获取任务
	WalletTask * getTask()
	{
		return pWalletTask;
	};
private:
	//产生开头的公钥
	unsigned int genPublicKeyStart(unsigned char *pPrivateKey, unsigned char *pPublicKey, unsigned int iThreadCount, unsigned int iCalcCount, unsigned char *pPrivateStart)
	{
		//使用私钥，计算公钥为每个GPU线程计算公钥
		for (unsigned int i = 0; i < iThreadCount; i++)
		{
			//目标私钥
			ProcPrivateAddInt(pPrivateKey, pPrivateStart, iCalcCount * i);

			//目标私钥对应的公钥
			secp256k1_ecmult_gen(pContext, pPublicKey, pPrivateKey);

			//目标私钥和公钥的存储位置
			pPrivateKey = pPrivateKey + 32;
			pPublicKey  = pPublicKey  + 66;
		}
		return 0;
	};
	//产生其他的公钥
	unsigned int genPublicKeyOther(unsigned char *pPrivateKey, unsigned char *pPublicKey, unsigned int iThreadCount, unsigned int iCalcCount, unsigned char * pOutput, unsigned int &iResultCount, unsigned int iJob, unsigned int iTask )
	{
		//unsigned char pPublicKey222[66] = { 0x04,0x4d,0xd2,0x58,0xcc,0x3e,0x05,0x0b,0x57,0x02,0x99,0xef,0x45,0xde,0x5d,0x96,0xe5,0x24,0x05,0x10,0x96,0xa2,0xa9,0xae,0x52,0xd2,0x2b,0xa8,0x92,0x7b,0x16,0x7f,0xce,0xf2,0x97,0xf3,0x5a,0x0d,0xe8,0xb7,0xc5,0x78,0x92,0x64,0xd2,0xde,0x85,0x8d,0xc8,0x58,0x2c,0x39,0x36,0x8c,0x39,0x9f,0xd9,0x1d,0xc5,0xa9,0x2c,0x33,0xd8,0x5a,0xa1,0x0 };

		//中间返回值
		int iResult = 0;
		//创建Buff
		iResult = pWalletFind->create_buffer();
		if (0 != iResult)
			return iResult;
		//加载数据
		iResult = pWalletFind->load_buffer(pPublicKey, pPrivateKey, iCalcCount);
		if (0 != iResult)
			return iResult;
		//计算结果
		iResult = pWalletFind->crypt(pOutput, iResultCount);
		if (0 != iResult)
			return iResult;
		return iResult;
	};
	//计算私钥 和 32位整数相加
	int ProcPrivateAddInt(unsigned char *pPrivateDest, unsigned char * szPrivateKey, unsigned int iNum)
	{
		unsigned char iFirst  = iNum % 0x100; iNum = iNum / 0x100;
		unsigned char iSecond = iNum % 0x100; iNum = iNum / 0x100;
		unsigned char iThird  = iNum % 0x100; iNum = iNum / 0x100;
		unsigned char iFourth = iNum % 0x100; iNum = iNum / 0x100;

		//先把全部数据放到目标空间
		memcpy(pPrivateDest, szPrivateKey, 32);

		//前28位原样不动
		int iTmp = szPrivateKey[31] + iFirst; pPrivateDest[31] = szPrivateKey[31] + iFirst;
		if (iTmp >= 0x100)
		{
			iTmp = szPrivateKey[30] + iSecond + 1; pPrivateDest[30] = szPrivateKey[30] + iSecond + 1;
		}
		else
		{
			iTmp = szPrivateKey[30] + iSecond; pPrivateDest[30] = szPrivateKey[30] + iSecond;
		}

		if (iTmp >= 0x100)
		{
			iTmp = szPrivateKey[29] + iThird + 1; pPrivateDest[29] = szPrivateKey[29] + iThird + 1;
		}
		else
		{
			iTmp = szPrivateKey[29] + iThird; pPrivateDest[29] = szPrivateKey[29] + iThird;
		}

		if (iTmp >= 0x100)
		{
			iTmp = szPrivateKey[28] + iFourth + 1; pPrivateDest[28] = szPrivateKey[28] + iFourth + 1;
		}
		else
		{
			iTmp = szPrivateKey[28] + iFourth; pPrivateDest[28] = szPrivateKey[28] + iFourth;
		}

		if (iTmp >= 0x100)
		{
			iTmp = szPrivateKey[27] + 1; szPrivateKey[27] = szPrivateKey[27] + 1;
		}

		if (iTmp >= 0x100)
		{
			iTmp = szPrivateKey[26] + 1; szPrivateKey[26] = szPrivateKey[26] + 1;
		}
		return iTmp;
	};

private:
	//释放每一个已经设为删除的任务
	static void ReleaseFinish(WalletTask * pWalletTask)
	{
		//任务对象
		WalletTaskItem * pTaskItem = NULL;
		//获取第一个Item
		pTaskItem = pWalletTask->getItemBegin();

		//循环释放每一个已经设为删除的任务
		while (true)
		{
			//如果找不到，就跳出去
			if (NULL == pTaskItem)
			{
				break;
			}

			//如果已经被设为删除状态，就删除掉
			if (3 == pTaskItem->getState())
			{
				//从任务清单删除
				pWalletTask->popItem(pTaskItem);
				//释放空间
				delete pTaskItem;
				pTaskItem = NULL;
			}

			//获取下一个任务
			pTaskItem = pWalletTask->getItemNext();
		}
	}
	//线程主函数
	static void run(ThreadProcCryptoCL * pThread)
	{
		if (NULL == pThread)
			return;

		//任务对象
		WalletTask * pWalletTask = pThread->pWalletTask;
		WalletTaskItem * pTaskItem = NULL;

		while (pThread->iFlagExit)
		{
			//查找和释放已经删除的任务
			ReleaseFinish(pWalletTask);

			//寻找未处理的任务Item
			pTaskItem = pWalletTask->getItemPending();
			//如果找不到，就等一下再找
			if (!pTaskItem)
			{
				Sleep(100);
				continue;
			}

			//设置为正在处理状态
			pTaskItem->setState(1);

			//将任务交给空闲任务处理器
			pThread->proc_task(pTaskItem);

			//设置为完成状态
			pTaskItem->setState(2);
		}
		//退出状态
		pThread->iFlagExit = 0;
	};
	void proc_task(WalletTaskItem * pTaskItem)
	{
		//开始私钥
		unsigned char szPrivateStart[33] = { 0 };
		memcpy(szPrivateStart, pTaskItem->getPrivateStart(), 32);
		//JobID
		unsigned int iJobID = pTaskItem->getJobID();
		//TaskID
		unsigned int iTaskID = pTaskItem->getTaskID();
		//私钥数量
		unsigned int iPrivateCount = pTaskItem->getPrivateCount();

		//处理每个阶段返回的结果
		unsigned int iResult = 0;

		//这里的线程数不是上层所说的线程，此处指GPU的并行计算数量		//获取GPU的最佳并行数量
		unsigned int iThreadCount = pWalletFind->get_thread();
		//每个线程的计算量
		unsigned int iCalcCount = iPrivateCount / iThreadCount + 1;

		//计算私钥和公钥的空间
		unsigned char * pPrivateKey = (unsigned char *)malloc(32 * iThreadCount);
		unsigned char * pPublicKey  = (unsigned char *)malloc(66 * iThreadCount);

		//返回结果以及返回结果数量，每个线程最大128个返回
		unsigned int iResultCount = 0;
		unsigned char * pOutput = (unsigned char *)malloc(32 * iThreadCount * 128);

		//计算开始公钥数组
		iResult = genPublicKeyStart(pPrivateKey, pPublicKey, iThreadCount, iCalcCount, szPrivateStart);

		//结果数量为0
		iResultCount = 0;

		//调用GPU处理剩余的计算任务
		iResult = genPublicKeyOther(pPrivateKey, pPublicKey, iThreadCount, iCalcCount, pOutput, iResultCount, iJobID, iTaskID);

		//对于OpenCL，运行到这里，意味着整个任务完成，设置界面的显示进度
		unsigned int * iTmp = pTaskItem->getCurrent();
		*iTmp = iPrivateCount;

		//读取每个返回结果
		for (unsigned int i = 0; i < iResultCount; i++)
		{
			WalletResultItem *pItem = new WalletResultItem();
			pItem->setPrivateKey(pOutput + i * 32);
			pItem->setJobID(iJobID);
			pItem->setTaskID(iTaskID);
			pWalletResult->pushItem(pItem);
		}

		//释放空间
		free(pPrivateKey);
		free(pPublicKey);
		free(pOutput);
	};

private:
	//线程运行和退出标志: 0,退出；1,运行
	unsigned int iFlagExit   = 0;

	//主线程句柄
	std::thread oMainWorkThd;
	//计算结果
	WalletResult * pWalletResult = NULL;
	//计算任务
	WalletTask * pWalletTask = NULL;

	//设置OpenCL文件，包含路径
	char *pFileOpenCL12 = NULL;
	char *pFileOpenCL20 = NULL;

	//钱包文件内容
	unsigned char *pWalletData = NULL;
	//钱包数据中钱包的数量
	unsigned int iWalletCount  = 0;

	//处理GPU
	wallet_find_opencl * pWalletFind = NULL;

	//椭圆曲线计算对象
	secp256k1_ecmult_gen_context * pContext = NULL;
};
