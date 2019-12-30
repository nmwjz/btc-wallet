#pragma once

#include "CL/cl.h"

class device_info
{
public:
	//设备类型常量
	static const unsigned int DEVICE_TYPE_CPU = 0;
	static const unsigned int DEVICE_TYPE_GPU = 1;
	static const unsigned int DEVICE_TYPE_OTHER = 2;

	//设备OpenCL版本号常量
	static const unsigned int PLATFORM_DEVICE_VER_12 = 0;
	static const unsigned int PLATFORM_DEVICE_VER_20 = 1;
	static const unsigned int PLATFORM_DEVICE_VER_OTHER = 2;

	//设备是支持大尾端还是小尾端
	static const unsigned int DEVICE_ENDIAN_LITTLE = 0;    //小尾端
	static const unsigned int DEVICE_ENDIAN_BIG    = 1;    //大尾端

	//设备的地址位
	static const unsigned int DEVICE_32 = 0;    //32位设备
	static const unsigned int DEVICE_64 = 1;    //64位设备

public:
	device_info() {};
	~device_info() {};

	//设置设备是大尾端还是小尾端
	void setEndian( unsigned int iEndian)
	{
		this->iEndian = iEndian;
	}
	unsigned int getEndian()
	{
		return iEndian;
	}

	//设备支持的OpenCL版本号
	void setOpenCLVer(unsigned int iVer)
	{
		iOpenCLVer = iVer;
	}
	unsigned int getOpenCLVer()
	{
		return iOpenCLVer;
	}

	//设备的最佳并行数量
	void setParallel(unsigned int iParallelX,unsigned int iParallelY, unsigned int iParallelZ)
	{
		this->iParallelX = iParallelX;
		this->iParallelY = iParallelY;
		this->iParallelZ = iParallelZ;
	}
	unsigned int getParallelX()
	{
		return iParallelX;
	}
	unsigned int getParallelY()
	{
		return iParallelY;
	}
	unsigned int getParallelZ()
	{
		return iParallelZ;
	}

	//设备最大时钟频率
	void setClockFreq( unsigned int iFreq)
	{
		iClockFreq = iFreq;
	}
	unsigned int getClockFreq()
	{
		return iClockFreq;
	}

	//设备的数据位宽
	void setBit(unsigned int iBit)
	{
		iDeviceBit = iBit;
	}
	unsigned int getBit()
	{
		return iDeviceBit;
	}

	//设备类型
	void setDeviceType(unsigned int iType)
	{
		iDeviceType = iType;
	}
	unsigned int getDeviceType()
	{
		return iDeviceType;
	}

	//设备ID
	void setDeviceID(cl_device_id iID)
	{
		iDeviceID = iID;
	}
	cl_device_id getDeviceID()
	{
		return iDeviceID;
	}

	//设备ID对应的上下文
	void setContext(cl_context oCon )
	{
		oContext = oCon;
	}
	cl_context getContext()
	{
		return oContext;
	}

	//oProgram;
	void setProgram(cl_program oPro)
	{
		oProgram = oPro;
	}
	cl_program getProgram()
	{
		return oProgram;
	}
	
	//kernel
	void setKernel(cl_kernel oKer)
	{
		oKernel = oKer;
	}
	cl_kernel getKernel()
	{
		return oKernel;
	}

	//CommandQueue
	void setCommand(cl_command_queue oCommand)
	{
		oCommandQueue = oCommand;
	}
	cl_command_queue getCommand()
	{
		return oCommandQueue;
	}
private:
	//设备是支持大尾端还是小尾端
	unsigned int iEndian     = DEVICE_ENDIAN_LITTLE;
	//设备支持的OpenCL版本号
	unsigned int iOpenCLVer  = PLATFORM_DEVICE_VER_20;
	//设备的最佳并行数量(在我们这种场景下)
	unsigned int iParallelX = 0;
	unsigned int iParallelY = 0;
	unsigned int iParallelZ = 0;
	//设备的最大时钟频率(MHz)
	unsigned int iClockFreq  = 0;
	//设备的地址位
	unsigned int iDeviceBit  = DEVICE_32;

	//设备类型
	unsigned int iDeviceType = DEVICE_TYPE_GPU;
	//设备ID
	cl_device_id iDeviceID;

	//设备Context
	cl_context oContext;

	cl_program oProgram;
	cl_kernel  oKernel;
	cl_command_queue oCommandQueue;
};
