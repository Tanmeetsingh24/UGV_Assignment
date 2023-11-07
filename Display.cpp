#using<System.dll>
#include "Display.h"
#include "NetworkedModule.h"

using namespace System;
using namespace System::Threading;
using namespace System::Diagnostics;

Display::Display(SM_ThreadManagement^ SM_TM, SM_Display^ SM_Display)
{
	SM_Display_ = SM_Display;
	SM_TM_ = SM_TM;
	Watch = gcnew Stopwatch;
}

error_state Display::setupSharedMemory()
{
	return SUCCESS;
}


error_state Display::connect(String^ hostName, int portNumber)
{
	return SUCCESS;
}

void Display::threadFunction()
{
	Console::WriteLine("Display		Thread is starting.");
	//setup the stopwatch
	Watch = gcnew Stopwatch;
	//barrier
	SM_TM_->ThreadBarrier->SignalAndWait();
	Watch->Start();
	while (/*!Console::KeyAvailable && */ !getShutdownFlag())
	{
		Console::WriteLine("Display		Thread is running.");
		processHeartbeats();
		if (communicate() == SUCCESS && checkData() == SUCCESS)
		{
			processSharedMemory();
		}
		Thread::Sleep(20);
	}
	Console::WriteLine("Display		thread is terminating");

}

error_state Display::processHeartbeats()
{
	if ((SM_TM_->heartbeat & bit_DISPLAY) == 0)
	{
		SM_TM_->heartbeat |= bit_DISPLAY;
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

void Display::shutdownModules()
{
	SM_TM_->shutdown = 0xFF;
}

bool Display::getShutdownFlag()
{
	return SM_TM_->shutdown & bit_DISPLAY;
}

error_state Display::communicate()
{
	return SUCCESS;
}

error_state Display::checkData()
{
	return SUCCESS;
}

error_state Display::processSharedMemory()
{
	return SUCCESS;
}