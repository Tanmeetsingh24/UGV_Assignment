//#using<System.dll>
//#include "ControllerInterface.h"
//#include "Controller.h"
//#include "UGVModule.h"
//
//using namespace System;
//using namespace System::Threading;
//using namespace System::Diagnostics;
//
//Controller::Controller(SM_ThreadManagement^ SM_TM, SM_Controller^ SM_Controller)
//{
//	SM_TM_ = SM_TM;
//	Watch = gcnew Stopwatch;
//}
//
//
//
//
//error_state Controller::connect(String^ hostName, int portNumber)
//{
//	return SUCCESS;
//}
//
//void Controller::threadFunction()
//{
//	Console::WriteLine("Controller		Thread is starting.");
//	//setup the stopwatch
//	Watch = gcnew Stopwatch;
//	//barrier
//	SM_TM_->ThreadBarrier->SignalAndWait();
//	Watch->Start();
//	while (/*!Console::KeyAvailable && */ !getShutdownFlag())
//	{
//		Console::WriteLine("Controller		Thread is running.");
//		processHeartbeats();
//		if (communicate() == SUCCESS && checkData() == SUCCESS)
//		{
//			processSharedMemory();
//		}
//		Thread::Sleep(20);
//	}
//	Console::WriteLine("Controller		thread is terminating");
//
//}
//
//error_state Controller::processHeartbeats()
//{
//	if ((SM_TM_->heartbeat & bit_CONTROLLER) == 0)
//	{
//		SM_TM_->heartbeat |= bit_CONTROLLER;
//		Watch->Restart();
//	}
//	else
//	{
//		if (Watch->ElapsedMilliseconds > CRASH_LIMIT)
//		{
//			shutdownModules();
//			return ERR_TMM_FAILURE;
//		}
//	}
//	//Heartbeat byte down?
//
//	return SUCCESS;
//}
//
//void Controller::shutdownModules()
//{
//	SM_TM_->shutdown = 0xFF;
//}
//
//bool Controller::getShutdownFlag()
//{
//	return SM_TM_->shutdown & bit_CONTROLLER;
//}
//
//ControllerInterface() {
//
//}
//
///**
//* Constructs a class setting the controller number and input type as specified
//* Definition of controller type for use when running softare to be controllable
//* by keyboard and xbox controller independently.
//* Use keyboard for testing at home if you do not have a controller
//* @param playerNum number of the controller being communicated with
//* (specified by the controller) as an integer in the range 1-4
//* @param input 0 for X-Box controller and 1 for keyboard
//*/
//ControllerInterface(DWORD playerNum, int input_type);
//
///**
//* Gets the state of all inputs from the controller
//* @returns struct as the state of all controller inputs.
//* Return type is as defined in the struct above.
//*/
//controllerState GetState();
//
///**
//* Check whether there is a controller connected currently based on the class instantiation.
//* If keyboard mode is used, this will always return true
//* @returns true if controller is currently connected
//*/
//bool IsConnected();
//
///**
//* Helpful function for printing out the state of the whole controller (based on what you
//* would have got from the GetState function).
//* @param state current state of the controller (as returned by ControllerInterface::GetState()
//*/
//static void printControllerState(controllerState state);
//
