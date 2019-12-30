#pragma once

#include "device_info.h"
#include "wallet_find_opencl.h"

//解码CL文件
void static codec(unsigned char *pCLFile, unsigned int iLen)
{
	const unsigned int MASK_LEN = 8;
	unsigned char szMask[MASK_LEN] = { 0xff, 0xfc, 0xf0, 0xc0, 0x00, 0x03, 0x0f, 0x3f };    //1111 1111/1111 1100/1111 0000/1100 0000/0000 0000/0000 0011/0000 1111/0011 1111//

	//处理每一个字符
	for (unsigned int i = 0; i < iLen; i++)
	{
		*pCLFile = (*pCLFile) ^ szMask[i%MASK_LEN];

		pCLFile++;
	}
}

wallet_find_opencl::wallet_find_opencl()
{
	pPlatformID = NULL;
}
wallet_find_opencl::~wallet_find_opencl()
{
	//存放所有的钱包信息
	if (pAllWalletContext)
	{
		free(pAllWalletContext);
		pAllWalletContext = NULL;
	}
	//存放cl脚本文件的串
	if (pSourceOpenCL12)
	{
		free(pSourceOpenCL12);
		pSourceOpenCL12 = NULL;
	}
	//存放cl脚本文件的串
	if (pSourceOpenCL20)
	{
		free(pSourceOpenCL20);
		pSourceOpenCL20 = NULL;
	}
}


//调用顺序0
//功能：设置钱包数据信息
//参数；
//	1、pWalletData钱包数据
//	2、iWalletCount钱包数量
//成功返回0，失败返回对应错误码
unsigned int wallet_find_opencl::load_wallet(unsigned char *pWalletData, unsigned int iCount)
{
	pAllWalletContext = pWalletData;
	iWalletCount = iCount;

	return 0;
}

//调用顺序0
//功能：加载CL文件到内存
//参数：
//  1、pFileOpenCL12 OpenCL1.2的源文件
//  2、pFileOpenCL20 OpenCL2.0的源文件
//成功返回0，失败返回对应错误码
unsigned int wallet_find_opencl::load_source(char *pFileOpenCL12, char *pFileOpenCL20)
{
	//====================读取CL1.2脚本=开始==============//
	//打开文件
	FILE * fp12 = fopen(pFileOpenCL12, "rb");

	//获取文件总长度
	fseek(fp12, 0, SEEK_END);
	unsigned int iFileLen12 = ftell(fp12);

	//分配文件内容空间
	pSourceOpenCL12 = (char *)malloc(iFileLen12+1);
	memset(pSourceOpenCL12, 0, iFileLen12+1);

	//读取文件内容
	readFile(fp12, (unsigned char *)pSourceOpenCL12, iFileLen12);

	//关闭文件
	fclose(fp12);

	//编解码内容
	codec((unsigned char *)pSourceOpenCL12, iFileLen12);
	//====================读取CL1.2脚本=结束==============//


	//====================读取CL2.0脚本=开始==============//
	//打开文件
	FILE * fp20 = fopen(pFileOpenCL20, "rb");

	//获取文件总长度
	fseek(fp20, 0, SEEK_END);
	unsigned int iFileLen20 = ftell(fp20);

	//分配文件内容空间
	pSourceOpenCL20 = (char *)malloc(iFileLen20+1);
	memset(pSourceOpenCL20, 0, iFileLen20+1);

	//读取文件内容
	readFile(fp20, (unsigned char *)pSourceOpenCL20, iFileLen20);

	//关闭文件
	fclose(fp20);

	//编解码内容
	codec((unsigned char *)pSourceOpenCL20, iFileLen20);
	//====================读取CL2.0脚本=结束==============//

	return 0;
}

//调用顺序0
//功能：获取设备ID，并且创建设备上下文
//无参数；
//成功返回0，失败返回对应错误码
unsigned int wallet_find_opencl::find_platform()
{
	cl_int iResult = 0;

	//获取平台数目
	iResult = clGetPlatformIDs(0, NULL, &iPlatformCount);
	if (CL_SUCCESS != iResult)
	{
		printf("获取平台数目失败\r\n");
		return 0;
	}
	//没有OpenCL平台
	if (0 >= iPlatformCount)
	{
		printf("平台数目:0\r\n");
		return 0;
	}
	printf("平台数目:%d\r\n", iPlatformCount);

	//为平台ID分配空间
	pPlatformID = (cl_platform_id*)malloc(iPlatformCount * sizeof(cl_platform_id));

	//获取所有平台的ID
	iResult = clGetPlatformIDs(iPlatformCount, pPlatformID, NULL);
	if (CL_SUCCESS != iResult)
	{
		printf("获取所有平台ID\r\n");
		return 1;
	}

	//unsigned int iVer = device_info::PLATFORM_DEVICE_VER_20;
	//char * pInfo = (char *)malloc(1024);
	//下面获取每个平台的所有设备（还需要继续处理）
	for (unsigned int i = 0; i < iPlatformCount; i++)
	{
		//获取平台名称
		//memset(pInfo, 0, 1024);
		//clGetPlatformInfo(pPlatformID[i], CL_PLATFORM_NAME, 1024, pInfo, NULL);

		//获取平台OpenCL版本
		//memset(pInfo, 0, 1024);
		//clGetPlatformInfo(pPlatformID[i], CL_PLATFORM_VERSION, 1024, pInfo, NULL);

		//判断是不是OpenCL1.2
		//if (strstr(pInfo, "1.2"))
		//	iVer = device_info::PLATFORM_DEVICE_VER_12;

		//获取平台下所有设备
		find_device(pPlatformID[i]);
	}

	return 0;
}

//调用顺序1，在方法load_wallet，load_source，find_platform之后
//功能：分配应用执行的Buff，创建应用执行的参数
//参数：
//	无
//成功返回0，失败返回对应错误码
unsigned int wallet_find_opencl::create_buffer()
{
	for (std::list<device_info*>::iterator it = lstDeviceInfo.begin(); it != lstDeviceInfo.end(); it++)
	{
		device_info * pDevice = *it;
		create_buffer_inner(pDevice->getContext(), pDevice->getKernel());
	}

	return 0;
}

//调用顺序2，在方法load_wallet，load_source，find_platform，create_buffer之后
//功能：将Host数据加载到Kernel
//	1、pPublicKey公钥数组，存放每一个开始的公钥
//	2、pPrivateKey私钥数组，存放每一个开始的私钥
//	3、iCalcCount每个线程的计算量
//成功返回0，失败返回对应错误码
unsigned int wallet_find_opencl::load_buffer(unsigned char * pPublicKey, unsigned char * pPrivateKey, unsigned int iCalcCount)
{
	for (std::list<device_info*>::iterator it = lstDeviceInfo.begin(); it != lstDeviceInfo.end(); it++)
	{
		device_info * pDevice = *it;
		load_buffer_inner(pDevice->getCommand(), pPublicKey, pPrivateKey, iCalcCount);
	}

	return 0;
}

//调用顺序3，在方法load_wallet，load_source，find_platform，create_buffer，load_buffer之后
//功能：计算目标加密数据
//参数：
//	1、pOutput输出数据
//	2、iOutputCount输出数据的数量
//成功返回0，失败返回对应错误码
unsigned int wallet_find_opencl::crypt(unsigned char * pOutput, unsigned int &iOutputCount)
{
	for (std::list<device_info*>::iterator it = lstDeviceInfo.begin(); it != lstDeviceInfo.end(); it++)
	{
		device_info * pDevice = *it;
		crypt_inner(pDevice->getCommand(), pDevice->getKernel(), pOutput, iOutputCount);
	}

	return 0;
}

//调用顺序2，在方法find_platform之后即可
//返回Work_Item的数量，即并发线程的数量
unsigned int wallet_find_opencl::get_thread()
{
	unsigned int iCount = 0;

	//循环获取每个设备的并发数
	for (std::list<device_info*>::iterator it = lstDeviceInfo.begin(); it != lstDeviceInfo.end(); it++)
	{
		device_info * pDevice = *it;

		iCount = iCount + pDevice->getParallelX();
	}

	return iCount;
}

//调用顺序4，在方法create_buffer，crypt之后
//功能：清除战场
void wallet_find_opencl::clean_buff()
{
	if (0 != buffer_public_key)
		clReleaseMemObject(buffer_public_key);
	if (0 != buffer_private_key)
		clReleaseMemObject(buffer_private_key);
	if (0 != buffer_calc_count)
		clReleaseMemObject(buffer_calc_count);
	if (0 != buffer_wallet_data)
		clReleaseMemObject(buffer_wallet_data);
	if (0 != buffer_wallet_count)
		clReleaseMemObject(buffer_wallet_count);
	if (0 != buffer_output)
		clReleaseMemObject(buffer_output);
	if (0 != buffer_output_count)
		clReleaseMemObject(buffer_output_count);
}

//调用顺序5，在方法create_buffer，crypt，clean_buff之后
//功能：清除战场，清除所有OpenCL对象
void wallet_find_opencl::clean_context()
{
	for (std::list<device_info*>::iterator it = lstDeviceInfo.begin(); it != lstDeviceInfo.end(); it++)
	{
		device_info * pDevice = *it;

		cl_device_id iDeviceIDTmp = pDevice->getDeviceID();

		//设备Context
		cl_context oContextTmp = pDevice->getContext();
		cl_program oProgramTmp = pDevice->getProgram();
		cl_kernel  oKernelTmp = pDevice->getKernel();
		cl_command_queue oCommandQueueTmp = pDevice->getCommand();

		//释放设备上的处理对象
		clReleaseCommandQueue(oCommandQueueTmp);
		clReleaseKernel(oKernelTmp);
		clReleaseProgram(oProgramTmp);
		clReleaseContext(oContextTmp);

		//释放设备
		clReleaseDevice(iDeviceIDTmp);
	}
}

//功能：读取OpenCL文件
//参数：
//  1、fp被fopen打开的文件句柄
//  2、pSourceContext存放文件的目标Buff
//  3、iLen文件长度或Buff长度
//成功返回0，失败返回对应错误码
unsigned int wallet_find_opencl::readFile(FILE *fp, unsigned char * pSourceContext, unsigned int iLen)
{
	//移动到文件开头
	fseek(fp, 0, SEEK_SET);

	//总的读取数量
	unsigned int iSum = 0;

	unsigned char * pBuff = pSourceContext;
	for (unsigned int i = 0; i < iLen; i++)
	{
		//一个字节一个字节读取
		if (fread(pBuff, 1, 1, fp) <= 0)
			break;

		pBuff++;

		iSum++;
	}

	return iSum;
}

//功能：获取平台下所有设备，并且在每个设备ID的基础上创建上下文
//参数：
//  1、iPlatformID平台ID
//成功返回0，失败返回对应错误码
unsigned int wallet_find_opencl::find_device(cl_platform_id iPlatformID)
{
	cl_uint iDevicesNum = 0;

	//获取平台下所有GPU设备的数量
	cl_int iResult = clGetDeviceIDs(iPlatformID, CL_DEVICE_TYPE_GPU, 0, NULL, &iDevicesNum);
	if (CL_SUCCESS != iResult)
	{
		printf("获取设备数量失败\r\n");
		return 1;
	}

	//设备数量=0
	if (0 >= iDevicesNum)
	{
		printf("设备数量=0\r\n");
		return 2;
	}

	//分配设备ID的存储空间
	cl_device_id *pDeviceID = (cl_device_id*)malloc(iDevicesNum * sizeof(cl_device_id));

	//获取指定平台下所有的设备ID
	iResult = clGetDeviceIDs(iPlatformID, CL_DEVICE_TYPE_GPU, iDevicesNum, pDeviceID, NULL);
	if (CL_SUCCESS != iResult)
	{
		printf("获取设备ID失败\r\n");
		return 3;
	}

	//设备ID
	//printf("DeviceID:%d\r\n", (unsigned int)*pDeviceID);

	//循环处理所有设备
	for (unsigned int i = 0; i < iDevicesNum; i++)
	{
		unsigned int iEndian = device_info::DEVICE_ENDIAN_LITTLE;
		unsigned int iVer = device_info::PLATFORM_DEVICE_VER_20;
		unsigned int iDeviceBit = device_info::DEVICE_64;
		unsigned int iClockFreq = 0;

		char *szBuffTmp = (char *)malloc(1024);
		size_t iLength = 0;

		memset(szBuffTmp, 0, 1024);
		//获取设备支持的OpenCL版本 
		iResult = clGetDeviceInfo(pDeviceID[i], CL_DEVICE_VERSION, 1024, szBuffTmp , &iLength);
		if (strstr(szBuffTmp, "1.2"))
			iVer = device_info::PLATFORM_DEVICE_VER_12;
		//打印设备版本
		printf( "DeviceVersion:%s\r\n" , szBuffTmp);

		memset(szBuffTmp, 0, 1024);
		//获取设备支持的大小尾端
		iResult = clGetDeviceInfo(pDeviceID[i], CL_DEVICE_ENDIAN_LITTLE, 1024, szBuffTmp, &iLength);
		if(CL_TRUE == szBuffTmp[0])
			iEndian = device_info::DEVICE_ENDIAN_LITTLE;
		else
			iEndian = device_info::DEVICE_ENDIAN_BIG;
		//打印设备大小尾端
		printf("DeviceEndian:%s\r\n", szBuffTmp);

		memset(szBuffTmp, 0, 1024);
		//获取最佳并行计算数量
		iResult = clGetDeviceInfo(pDeviceID[i], CL_DEVICE_MAX_WORK_ITEM_SIZES, 1024, szBuffTmp, &iLength);
		size_t *iTmp = (size_t *)szBuffTmp;
		unsigned __int64 iParallelX = *iTmp; iTmp++;
		unsigned __int64 iParallelY = *iTmp; iTmp++;
		unsigned __int64 iParallelZ = *iTmp; iTmp++;
		unsigned __int64 iParallelM = *iTmp; iTmp++;
		//打印设备各维度上的Work_Item数量
		printf("Work_Item[0]:%d\r\n", (unsigned int)iParallelX);
		printf("Work_Item[1]:%d\r\n", (unsigned int)iParallelY);
		printf("Work_Item[2]:%d\r\n", (unsigned int)iParallelZ);
		printf("Work_Item[3]:%d\r\n", (unsigned int)iParallelM);

		memset(szBuffTmp, 0, 1024);
		//获取设备最大的时钟频率
		iResult = clGetDeviceInfo(pDeviceID[i], CL_DEVICE_MAX_CLOCK_FREQUENCY, 1024, szBuffTmp, &iLength);
		iClockFreq = (cl_uint)*szBuffTmp;
		//打印设备ClockFreq
		printf("ClockFreq:%x\r\n", iClockFreq);

		memset(szBuffTmp, 0, 1024);
		//获取设备支持的数据位数
		iResult = clGetDeviceInfo(pDeviceID[i], CL_DEVICE_ADDRESS_BITS, 1024, szBuffTmp, &iLength);
		cl_uint iBit = (cl_uint)*szBuffTmp;
		if (32 == iBit)
			iDeviceBit = device_info::DEVICE_32;
		else
			iDeviceBit = device_info::DEVICE_64;
		free(szBuffTmp);

		//在对应的设备ID上创建上下文
		cl_context oContext = clCreateContext(NULL, 1, &pDeviceID[i], NULL, NULL, &iResult);
		if (CL_SUCCESS != iResult)
		{
			printf("创建Context失败\r\n");
			return 3;
		}

		char *pSource = NULL;
		if (device_info::PLATFORM_DEVICE_VER_12 == iVer)
			pSource = pSourceOpenCL12;
		else
			pSource = pSourceOpenCL20;

		//printf("========SRC========\r\n%s\r\n===================\r\n", pSource);
		//加载内核程序
		size_t iSourceLength = strlen(pSource) + 1;
		cl_program oProgram = clCreateProgramWithSource(oContext, 1, (const char **)&pSource, (const size_t *)&iSourceLength, &iResult);
		if (CL_SUCCESS != iResult)
		{
			printf("加载程序失败\r\n");
			return 1;
		}
		//clCreateProgramWithBinary
		//clGetProgramInfo
		//编译内核
		iResult = clBuildProgram(oProgram, 1, &pDeviceID[i], NULL, NULL, NULL);
		if (CL_SUCCESS != iResult)
		{
			printf("编译程序失败\r\n");
			return 2;
		}

		//创建命令队列
		cl_command_queue oCommandQueue = clCreateCommandQueue(oContext, pDeviceID[i], 0, &iResult);
		if (CL_SUCCESS != iResult)
		{
			printf("创建CommandQueue失败\r\n");
			return 4;
		}

		//Kernel入口函数
		cl_kernel oKernel = clCreateKernel(oProgram, "main", &iResult);
		if (CL_SUCCESS != iResult)
		{
			printf("创建Kernel失败\r\n");
			return 3;
		}

		//构建一个新的设备信息
		device_info *pDeviceInfo = new device_info();
		pDeviceInfo->setDeviceID(pDeviceID[i]);
		pDeviceInfo->setDeviceType(CL_DEVICE_TYPE_GPU);
		pDeviceInfo->setOpenCLVer(iVer);
		pDeviceInfo->setEndian(iEndian);
		pDeviceInfo->setParallel((unsigned int)iParallelX, (unsigned int)iParallelY, (unsigned int)iParallelZ);

		pDeviceInfo->setContext(oContext);
		pDeviceInfo->setProgram(oProgram);
		pDeviceInfo->setCommand(oCommandQueue);
		pDeviceInfo->setKernel(oKernel);

		//将新增设备信息插入列表
		lstDeviceInfo.push_back(pDeviceInfo);
	}

	//释放分配的空间
	free(pDeviceID);

	return 0;
}

//功能：分配应用执行的Buff，创建应用执行的参数
//参数：
//  1、oContext设备的上下文
//  2、oKernel设备上创建的Kernel
//成功返回0，失败返回对应错误码
unsigned int wallet_find_opencl::create_buffer_inner(cl_context oContext, cl_kernel oKernel)
{
	cl_int iResult = 0;
	//Work_Item的数量
	unsigned int iThreadCount = get_thread();

	//这三组数据时实时变动的，所以这里只需要创建空间，不需要填充数据
	//私钥数组
	buffer_private_key = clCreateBuffer(oContext, CL_MEM_READ_WRITE, 32 * iThreadCount, NULL, &iResult);  //CL_MEM_USE_HOST_PTR
	if (CL_SUCCESS != iResult)
	{
		printf("创建buffer_private_key失败\r\n");
		return 1;
	}
	//公钥数组
	buffer_public_key = clCreateBuffer(oContext, CL_MEM_READ_WRITE, 66 * iThreadCount, NULL, &iResult);
	if (CL_SUCCESS != iResult)
	{
		printf("创建buffer_public_key失败\r\n");
		return 2;
	}
	//每个线程的计算量
	buffer_calc_count = clCreateBuffer(oContext, CL_MEM_READ_WRITE, 8, NULL, &iResult);
	if (CL_SUCCESS != iResult)
	{
		printf("创建buffer_thread_count失败\r\n");
		return 3;
	}

	//这两组数据不随时间变动，所以这里完成注入后，不再变动
	//所有钱包的数据
	buffer_wallet_data = clCreateBuffer(oContext, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 20 * iWalletCount, pAllWalletContext, &iResult);
	if (CL_SUCCESS != iResult)
	{
		printf("创建buffer_wallet_data失败\r\n");
		return 4;
	}
	//所有钱包数据的长度
	buffer_wallet_count = clCreateBuffer(oContext, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 4, &iWalletCount, &iResult);
	if (CL_SUCCESS != iResult)
	{
		printf("创建buffer_wallet_count失败\r\n");
		return 5;
	}

	//下面两组数据是返回数据
	//输出结果数组,最多存放128个结果
	//输出结果(每个Work_Item最多输出16个结果，每个结果32字节)
	buffer_output = clCreateBuffer(oContext, CL_MEM_READ_WRITE, get_thread() * 32 * 16, NULL, &iResult);
	if (CL_SUCCESS != iResult)
	{
		printf("创建buffer_output失败\r\n");
		return 6;
	}
	//输出结果数量
	buffer_output_count = clCreateBuffer(oContext, CL_MEM_READ_WRITE, 4, NULL, &iResult);
	if (CL_SUCCESS != iResult)
	{
		printf("创建buffer_output_count失败\r\n");
		return 7;
	}

	//设置Kernel参数-公钥
	iResult = clSetKernelArg(oKernel, 0, sizeof(buffer_public_key), (void *)&buffer_public_key);
	if (CL_SUCCESS != iResult)
	{
		printf("设置KernelArg参数-公钥，失败\r\n");
		return 8;
	}
	//设置Kernel参数-私钥
	iResult = clSetKernelArg(oKernel, 1, sizeof(buffer_private_key), (void *)&buffer_private_key);
	if (CL_SUCCESS != iResult)
	{
		printf("设置KernelArg参数-私钥，失败失败\r\n");
		return 9;
	}
	//设置Kernel参数-线程的计算量
	iResult = clSetKernelArg(oKernel, 2, sizeof(buffer_calc_count), (void *)&buffer_calc_count);
	if (CL_SUCCESS != iResult)
	{
		printf("设置KernelArg参数-线程的计算量，失败\r\n");
		return 10;
	}
	//设置Kernel参数-所有钱包数据
	iResult = clSetKernelArg(oKernel, 3, sizeof(buffer_wallet_data), (void *)&buffer_wallet_data);
	if (CL_SUCCESS != iResult)
	{
		printf("设置KernelArg参数-所有钱包数据，失败\r\n");
		return 11;
	}
	//设置Kernel参数-所有钱包数据的长度
	iResult = clSetKernelArg(oKernel, 4, sizeof(buffer_wallet_count), (void *)&buffer_wallet_count);
	if (CL_SUCCESS != iResult)
	{
		printf("设置KernelArg参数-所有钱包数据的长度，失败\r\n");
		return 12;
	}
	//设置Kernel参数-输出数据
	iResult = clSetKernelArg(oKernel, 5, sizeof(buffer_output), (void *)&buffer_output);
	if (CL_SUCCESS != iResult)
	{
		printf("设置KernelArg参数-输出数据，失败\r\n");
		return 13;
	}
	//设置Kernel参数-输出数据的数量
	iResult = clSetKernelArg(oKernel, 6, sizeof(buffer_output_count), (void *)&buffer_output_count);
	if (CL_SUCCESS != iResult)
	{
		printf("设置KernelArg参数-输出数据的数量，失败\r\n");
		return 14;
	}

	return 0;
}

//功能：将Host数据加载到Queue
//	1、oQueue命令队列
//	2、pPublicKey公钥数组，存放每一个开始的公钥
//	3、pPrivateKey私钥数组，存放每一个开始的私钥
//  4、iCalcCount每个线程的计算量
//成功返回0，失败返回对应错误码
unsigned int wallet_find_opencl::load_buffer_inner(cl_command_queue oQueue, unsigned char * pPublicKey, unsigned char * pPrivateKey, unsigned int iCalcCount)
{
	cl_int iResult = 0;

	//加载Kernel参数-公钥
	iResult = clEnqueueWriteBuffer(oQueue, buffer_public_key, CL_TRUE, 0, get_thread() * 66, pPublicKey, 0, NULL, NULL);
	if (CL_SUCCESS != iResult)
	{
		printf("加载数据public_key，失败\r\n");
		return 1;
	}
	//加载Kernel参数-私钥
	iResult = clEnqueueWriteBuffer(oQueue, buffer_private_key, CL_TRUE, 0, get_thread() * 32, pPrivateKey, 0, NULL, NULL);
	if (CL_SUCCESS != iResult)
	{
		printf("加载数据private_key，失败失败\r\n");
		return 2;
	}
	//加载Kernel参数-线程的计算量
	iResult = clEnqueueWriteBuffer(oQueue, buffer_calc_count, CL_TRUE, 0, sizeof(buffer_calc_count), (void *)&iCalcCount, 0, NULL, NULL);
	if (CL_SUCCESS != iResult)
	{
		printf("加载数据CalcCount，失败\r\n");
		return 3;
	}

	return 0;
}

//功能：计算目标加密数据
//参数：
//  1、oCommandQueue命令队列
//  2、oKernel内核
//	3、pOutput输出数据
//	4、iOutputCount输出数据的数量
//成功返回0，失败返回对应错误码
unsigned int wallet_find_opencl::crypt_inner(cl_command_queue oCommandQueue, cl_kernel oKernel, unsigned char * pOutput, unsigned int &iOutputCount)
{
	cl_int iResult = 0;

	//size_t *global_work_size = (size_t *)malloc(sizeof(size_t) * 3);
	//*global_work_size = get_thread(); global_work_size++;
	//*global_work_size = get_thread(); global_work_size++;
	//*global_work_size = get_thread();

	//size_t *local_work_size = (size_t *)malloc(sizeof(size_t) * 3);
	//*local_work_size = 1; local_work_size++;
	//*local_work_size = 0; local_work_size++;
	//*local_work_size = 0;

	size_t global_work_size = get_thread();
	size_t local_work_size  = 1;

	// printf("%s\n",buffer_keys);
	iResult = clEnqueueNDRangeKernel(oCommandQueue, oKernel, 1, NULL, &global_work_size, &local_work_size, 0, NULL, NULL);
	if (CL_SUCCESS != iResult)
	{
		printf("Call clEnqueueNDRangeKernel，失败:%d\r\n", iResult);
		return 1;
	}
	iResult = clFinish(oCommandQueue);
	if (CL_SUCCESS != iResult)
	{
		printf("Call clFinish，失败\r\n");
		return 2;
	}

	//读取输出结果(每个Work_Item最多输出16个结果，每个结果32字节)
	iResult = clEnqueueReadBuffer(oCommandQueue, buffer_output, CL_TRUE, 0, get_thread() * 32 * 16, pOutput, 0, NULL, NULL);
	if (CL_SUCCESS != iResult)
	{
		printf("Call clEnqueueReadBuffer，失败\r\n");
		return 3;
	}
	//读取输出结果的数量
	iResult = clEnqueueReadBuffer(oCommandQueue, buffer_output_count, CL_TRUE, 0, 4, &iOutputCount, 0, NULL, NULL);
	if (CL_SUCCESS != iResult)
	{
		printf("Call clEnqueueReadBuffer，失败\r\n");
		return 4;
	}

	return iResult;
}
