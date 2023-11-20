#using<System.dll>
#include "ControllerInterface.h"
#include "Controller.h"
#include "UGVModule.h"

using namespace System;
using namespace System::Threading;
using namespace System::Diagnostics;

Controller::Controller(SM_ThreadManagement^ SM_TM, SM_Controller^ SM_Controller)
{
	SM_Controller_ = SM_Controller;
	SM_TM_ = SM_TM;
	Watch = gcnew Stopwatch;
}

void Controller::threadFunction()
{
	Console::WriteLine("Controller		Thread is starting.");
	//setup the stopwatch
	Watch = gcnew Stopwatch;
	//barrier
	SM_TM_->ThreadBarrier->SignalAndWait();
	Watch->Start();
	while (/*!Console::KeyAvailable && */ !getShutdownFlag())
	{
		Console::WriteLine("Controller		Thread is running.");
		processHeartbeats();
		if (communicate() == SUCCESS && checkData() == SUCCESS)
		{
			processSharedMemory();
		}
		Thread::Sleep(20);
	}
	Console::WriteLine("Controller		thread is terminating");

}

error_state Controller::processHeartbeats()
{
	if ((SM_TM_->heartbeat & bit_CONTROLLER) == 0)
	{
		SM_TM_->heartbeat |= bit_CONTROLLER;
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

error_state processSharedMemory()
{

}


void Controller::shutdownModules()
{
	SM_TM_->shutdown = 0xFF;
}

bool Controller::getShutdownFlag()
{
	return SM_TM_->shutdown & bit_CONTROLLER;
}



