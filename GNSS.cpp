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


#pragma pack(push,4)
struct GNSS_Data		//segregating bytes for processing GNSS data of 112 bytes
{
	unsigned int Header;
	unsigned char Discards1[40];
	double Northing;
	double Easting;
	double Height;
	unsigned char Discards2[40];
	unsigned int CRC;
};
#pragma pack(pop,4)

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
	Client->ReceiveTimeout = 500;
	Client->SendTimeout = 500;
	Client->ReceiveBufferSize = 1024;
	Client->SendBufferSize = 1024;
	Stream = Client->GetStream();

	SendData = gcnew array<unsigned char>(64);
	ReadData = gcnew array<unsigned char>(256);
	return SUCCESS;
}

//GNSS::~GNSS()
//{
//	Stream->Close();
//	Client->Close();
//}

void GNSS::threadFunction()
{

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
//	// Start reading from header. From there we fill GPS_struct
//	// Trapping the Header
//	bool hasHeader = false;
//	unsigned int Header = 0;
//	int i = 0;
//	int Start; //Start of data
//	unsigned char Data;
//	do
//	{
//		Data = ReadData[i++];
//		Header = ((Header << 8) | Data);
//		if (Header == 0xaa44121c) {
//			hasHeader = true;
//		}
//	} while (Header != 0xaa44121c && i < ReadData->Length);
//	Start = i - 4;
//	if (hasHeader == false) {
//		return;
//	}
//	// Filling data
//	unsigned char* BytePtr = (unsigned char*)&GNSS_Data;
//	for (int i = Start; i < Start + sizeof(GNSS_Data); i++)
//	{
//		*(BytePtr + i) = ReadData[i];
//	}
////////////////////////////////////////////////////////////////////////////////////////////////////
//	Thread::Sleep(10);
//	// Get incoming data
//	if (Stream->DataAvailable) {
//		Stream->Read(ReadData, 0, ReadData->Length);
//	}
//	this->readFromHeader();
//	// Compare CRC before setting attributes
//	unsigned char* bytePtr = (unsigned char*)&GPS_struct;
//	unsigned int GeneratedCRC = CalculateBlockCRC32(112 - 4, bytePtr);
//	this->CRC = GPS_struct.Checksum;
//	Console::WriteLine("CalcCRC: {0}, ServerCRC: {1}, Equal {2}]", GeneratedCRC, CRC, GeneratedCRC == CRC);
//	/*Setting attributes*/
//	if (GeneratedCRC == this->CRC) {
//		this->easting = GPS_struct.Easting;
//		this->northing = GPS_struct.Northing;
//		this->height = GPS_struct.Height;
//	}
//	else {
//		Console::WriteLine("GPS checksum mismatched");
//	}
//	Thread::Sleep(100);

	return SUCCESS;
}



error_state GNSS::communicate()
{
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
