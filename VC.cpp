#using<System.dll>
#include "VC.h"
#include "UGVModule.h"

using namespace System;
using namespace System::Threading;
using namespace System::Diagnostics;

VC::VC(SM_ThreadManagement^ SM_TM, SM_VehicleControl^ SM_VC)
{
	SM_VehicleControl_ = SM_VC;
	SM_TM_ = SM_TM;
	Watch = gcnew Stopwatch;
}

error_state VC::setupSharedMemory()
{
	return SUCCESS;
}


error_state VC::connect(String^ hostName, int portNumber)
{
	return SUCCESS;
}

void VC::threadFunction()
{
	Console::WriteLine("VC		Thread is starting.");
	//setup the stopwatch
	Watch = gcnew Stopwatch;
	//barrier
	SM_TM_->ThreadBarrier->SignalAndWait();
	Watch->Start();
	while (/*!Console::KeyAvailable && */ !getShutdownFlag())
	{
		Console::WriteLine("VC		Thread is running.");
		processHeartbeats();
		if (communicate() == SUCCESS && checkData() == SUCCESS)
		{
			processSharedMemory();
		}
		Thread::Sleep(20);
	}
	Console::WriteLine("VC		thread is terminating");

}

error_state VC::processHeartbeats()
{
	if ((SM_TM_->heartbeat & bit_VC) == 0)
	{
		SM_TM_->heartbeat |= bit_VC;
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
	//Heartbeat byte down?

	return SUCCESS;
}

void VC::shutdownModules()
{
	SM_TM_->shutdown = 0xFF;
}

bool VC::getShutdownFlag()
{
	return SM_TM_->shutdown & bit_VC;
}

error_state VC::communicate()
{
	return SUCCESS;
}

error_state VC::checkData()
{
	return SUCCESS;
}

error_state VC::processSharedMemory()
{
	return SUCCESS;
}