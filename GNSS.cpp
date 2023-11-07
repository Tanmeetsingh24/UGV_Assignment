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

GNSS::GNSS(String^ ipAddress, int port)
{
	IPAddress = ipAddress;
	Port = port;
	UGV = gcnew TcpClient(IPAddress, Port);
	UGVStream = UGV->GetStream();
	UGV->NoDelay = true;
	UGV->ReceiveTimeout = 500;
	UGV->SendTimeout = 500;
	UGV->ReceiveBufferSize = 1024;
	UGV->SendBufferSize = 1024;

	SendData = gcnew array<unsigned char>(64);
	RecvData = gcnew array<unsigned char>(64);
}

error_state GNSS::setupSharedMemory()
{
	return SUCCESS;
}

error_state GNSS::connect(String^ hostName, int portNumber)
{
	return SUCCESS;
}

void GNSS::threadFunction()
{

	Console::WriteLine("GNSS		Thread is starting.");
	//setup the stopwatch
	Watch = gcnew Stopwatch;
	//barrier
	SM_TM_->ThreadBarrier->SignalAndWait();
	Watch->Start();
	while (/*!Console::KeyAvailable && */ !getShutdownFlag())
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
			shutdownModules();
			return ERR_TMM_FAILURE;
		}
	}
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

error_state GNSS::communicate()
{
	return SUCCESS;
}

error_state GNSS::checkData()
{
	return SUCCESS;
}

error_state GNSS::processSharedMemory()
{
	return SUCCESS;
}

//GNSS::~GNSS()
//{
//	UGV->Close();
//}