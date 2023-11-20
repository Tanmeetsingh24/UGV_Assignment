#using<System.dll>
#include "ControllerInterface.h"
#include "Controller.h"
#include "UGVModule.h"

///////////////////////////////////////////////////////
#include "Controller.h"
#include "SMObjects.h"
#include "ControllerInterface.h"
#include "Display.h"
#include "Laser.h"
#include "TMM.h"
#include "VC.h" 
#include "GNSS.h"
#using <System.dll>
#include <UGVModule.h>
#include <iostream>
#include <math.h>
#include <windows.h>
#include <XInput.h>
#pragma comment(lib, "XInput.lib")
//////////////////////////////////////////////////////
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
	Console::WriteLine("Controller	Thread is starting.");
	//setup the stopwatch
	Watch = gcnew Stopwatch;
	//barrier
	SM_TM_->ThreadBarrier->SignalAndWait();
	Watch->Start();
	while (/*!Console::KeyAvailable && */ !getShutdownFlag())
	{
		Console::WriteLine("Controller	Thread is running.");
		processHeartbeats();
		/*if (communicate() == SUCCESS && checkData() == SUCCESS)
		{
			processSharedMemory();
		}*/
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

//////////////////////////////////////////////////////////////////
//void Controller::threadFunction() {
//	Joystick joystick;
//	joystick.initialize();
//
//	while (!getShutdownFlag())
//	{ //xbox button a shutdown
//		if (isAButtonPressed())
//		{
//			shutdownThreads();
//			break;
//		}
//
//		JoystickState state = joystick.readState();
//		float leftTriggerValue = state.leftTrigger;
//		float rightTriggerValue = state.rightTrigger;
//		float joystickXValue = state.joystickX;
//		controllerData->leftTrigger = leftTriggerValue;
//		controllerData->rightTrigger = rightTriggerValue;
//		controllerData->joystickX = joystickXValue;
//
//		Console::WriteLine("controller running");
//		processHeartbeats();
//		if (communicate() == SUCCESS && checkData() == SUCCESS)
//		{
//			processSharedMemory();
//		}
//
//		Thread::Sleep(20);
//	}
//
//	Console::WriteLine("controller terminated");
//}
//
//bool isAButtonPressed() {
//	XINPUT_STATE state;
//	ZeroMemory(&state, sizeof(XINPUT_STATE));
//
//	DWORD result = XInputGetState(0, &state);
//
//	if (result == ERROR_SUCCESS) {
//		return (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0;
//	}
//
//	return false;
//
