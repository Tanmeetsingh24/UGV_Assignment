#using<System.dll>
#include "GNSS.h"
#include "UGVModule.h"

using namespace System;
using namespace System::Threading;
using namespace System::Diagnostics;

GNSS::GNSS(SM_ThreadManagement^ SM_TM, SM_GNSS^ SM_GNSS)
{
	SM_GNSS_ = SM_GNSS;
	SM_TM_ = SM_TM;
	Watch = gcnew Stopwatch;
}

error_state GNSS::connect(String^ hostName, int portNumber)
{
	try
	{
		Client = gcnew TcpClient(hostName, portNumber);
	}
	catch (int error)
	{
		Console::WriteLine("Error: Failed to establish connection with GNSS module err_code=%d", error);
		shutdownModules();
		return ERR_CONNECTION;
	}
	Client->NoDelay = true;
	Client->ReceiveTimeout = 2500;
	Client->SendTimeout = 2500;
	Client->ReceiveBufferSize = 1024;
	Client->SendBufferSize = 1024;
	Stream = Client->GetStream();

	SendData = gcnew array<unsigned char>(2048);
	ReadData = gcnew array<unsigned char>(2048);

	communicate();

	return SUCCESS;
}

//GNSS::~GNSS()
//{
//	Stream->Close();
//	Client->Close();
//}

void GNSS::threadFunction()
{
	connect(WEEDER_ADDRESS, 24000);

	Console::WriteLine("GNSS		Thread is starting.");
	
	Watch = gcnew Stopwatch;      //setup the stopwatch
	
	SM_TM_->ThreadBarrier->SignalAndWait();   //barrier
	Watch->Start();
	while (!getShutdownFlag())
	{
		Console::WriteLine("GNSS		Thread is running.");
		processHeartbeats();
		if (communicate() == SUCCESS && checkData() == SUCCESS)
		{
			processSharedMemory();
		}
		Thread::Sleep(20);
	}
	Console::WriteLine("GNSS		thread is terminating");

}

error_state GNSS::processHeartbeats()
{
	if ((SM_TM_->heartbeat & bit_GNSS) == 0)
	{
		SM_TM_->heartbeat |= bit_GNSS;
		Watch->Restart();
	}
	else
	{
		if (Watch->ElapsedMilliseconds > CRASH_LIMIT)
		{
			//shutdownModules();
			return ERR_GNSS_FAILURE;
		}
	}
	return SUCCESS;
}


error_state GNSS::processSharedMemory()
{
	return SUCCESS;
}


error_state GNSS::communicate()
{
	unsigned int Header = 0;
	Byte GData;
	 
	do{
		GData = Stream->ReadByte();
		Header = (Header << 8) | GData;
	} while (Header != 0xaa44121c);

	for (int i = 0; i < 108; i++)
	{
		ReadData[i] = Stream->ReadByte();
	}

	Northing = BitConverter::ToDouble(ReadData, 40);

	Easting = BitConverter::ToDouble(ReadData, 48);

	Height = BitConverter::ToDouble(ReadData, 56);

	CRC = BitConverter::ToUInt32(ReadData, 104);

	Console::WriteLine("Northing:{0:F3} Easting:{1:F3} Height:{2:F3} CRC:{3:X8} ", Northing, Easting, Height, CRC);
	return SUCCESS;
}

error_state GNSS::checkData()
{
	return SUCCESS;
}

void GNSS::shutdownModules()
{
	SM_TM_->shutdown = 0xFF;
}


bool GNSS::getShutdownFlag()
{
	return SM_TM_->shutdown & bit_GNSS;
}

//unsigned long CRC32Value(int i)
//{
//	int j;
//	unsigned long ulCRC;
//	ulCRC = i;
//	for (j = 8; j > 0; j--)
//	{
//		if (ulCRC & 1)
//			ulCRC = (ulCRC >> 1) ^ CRC32_POLYNOMIAL;
//		else
//			ulCRC >>= 1;
//	}
//	return ulCRC;
//}
//unsigned long CalculateBlockCRC32(unsigned long ulCount, /* Number of bytes in the data block */unsigned char* ucBuffer) /* Data block */
//{
//	unsigned long ulTemp1;
//	unsigned long ulTemp2;
//	unsigned long ulCRC = 0;
//	while (ulCount-- != 0)
//	{
//		ulTemp1 = (ulCRC >> 8) & 0x00FFFFFFL;
//		ulTemp2 = CRC32Value(((int)ulCRC ^ *ucBuffer++) & 0xff);
//		ulCRC = ulTemp1 ^ ulTemp2;
//	}
//	return(ulCRC);
//}
