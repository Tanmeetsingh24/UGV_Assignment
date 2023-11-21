#include "ControllerInterface.h"
#using<System.dll>
#include "Controller.h"
#include "UGVModule.h"

///////////////////////////////////////////////////////
#include "SMObjects.h"
#include "Display.h"
#include "Laser.h"
#include "TMM.h"
#include "VC.h" 
#include "GNSS.h"
#include <iostream>
#include <math.h>
#include <windows.h>
#include <XInput.h>
#pragma comment(lib, "XInput.lib")
//////////////////////////////////////////////////////
using namespace System;
using namespace System::Threading;
using namespace System::Diagnostics;

ControllerInterface Xboxcontroller;
//
//ControllerInterface::ControllerInterface(DWORD playerNum, int input_type)
//{
//	playerNum = 1; //keyboard
//	//playerNum = 0; //Controller
//}


Controller::Controller(SM_ThreadManagement^ SM_TM, SM_VehicleControl^ SM_VehicleControl)
{
	SM_VehicleControl_ = SM_VehicleControl;
	SM_TM_ = SM_TM;
	Watch = gcnew Stopwatch;
	
}

void Controller::threadFunction()
{

	

	Console::WriteLine("Controller	Thread is starting.");
	//setup the stopwatch
	Watch = gcnew Stopwatch;
	//barrier
	SM_TM_->ThreadBarrier->SignalAndWait();
	Watch->Start();
	while (!getShutdownFlag())
	{

		//Console::WriteLine("Controller	Thread is running.");
		processHeartbeats();
		
				
		Thread::Sleep(20);
	}
	Console::WriteLine("Controller	Thread is terminating");

	
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

error_state Controller::processSharedMemory()
{
	Monitor::Enter(SM_VehicleControl_->lockObject);

	controllerState^ val = Xboxcontroller.GetState();

	if (val->isConnected) {
		SM_VehicleControl_->Steering = -val->rightThumbX * 40;

		SM_VehicleControl_->Speed = val->rightTrigger - val->leftTrigger;

	}
	else {
		SM_VehicleControl_->Steering = 0;
		SM_VehicleControl_->Speed = 0;
	}
	Monitor::Exit(SM_VehicleControl_->lockObject);

	Monitor::Enter(SM_TM_->lockObject);

	if (val->buttonX) {
		SM_TM_->shutdown = bit_ALL;
	}
	Monitor::Exit(SM_TM_->lockObject);

	return SUCCESS;
}

void Controller::shutdownModules()
{
	SM_TM_->shutdown = 0xFF;
}

bool Controller::getShutdownFlag()
{
	return SM_TM_->shutdown & bit_CONTROLLER;
}
error_state Controller::checkData()
{
	return SUCCESS;
}
Controller::~Controller()
{

}


