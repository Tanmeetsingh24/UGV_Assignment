#using<System.dll>
#include "VC.h"

using namespace System;
using namespace System::Threading;
using namespace System::Diagnostics;

VC::VC(SM_ThreadManagement^ SM_TM, SM_VehicleControl^ SM_VC)
{
	SM_VehicleControl_ = SM_VC;
	SM_TM_ = SM_TM;
	Watch = gcnew Stopwatch;
	//controllerInterface_ = gcnew ControllerInterface();
}

error_state VC::connect(String^ hostName, int portNumber)
{
	try
	{
		Client = gcnew TcpClient(hostName, portNumber);
	}
	catch (int error)
	{
		Console::WriteLine("Error: Failed to establish connection with Vehicle Control Module err_code=%d", error);
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
	return SUCCESS;
}

error_state VC::communicate()
{
	String^ ResponseData = Encoding::ASCII->GetString(ReadData);
	return SUCCESS;
}

error_state VC::authenticate()
{
	String^ Command = ("5510198\n");
	SendData = Encoding::ASCII->GetBytes(Command);
	Stream->Write(SendData, 0, SendData->Length);
	Thread::Sleep(10);
	
	communicate();

	//if (ResponseData->Contains("OK"))
	//{
	//	Console::WriteLine("VC: Authenticated");
	//	return SUCCESS;
	//}
	//else
	//{
	//	Console::WriteLine("VC: Authentication failed");
	//	return ERR_CONNECTION; //print error
	//}
	return SUCCESS;

}

void VC::threadFunction()
{	
	connect(WEEDER_ADDRESS, 25000);

	authenticate();

	Console::WriteLine("VC		Thread is starting.");
	//setup the stopwatch
	Watch = gcnew Stopwatch;
	//barrier
	SM_TM_->ThreadBarrier->SignalAndWait();
	Watch->Start();
	while (!getShutdownFlag())
	{
		Console::WriteLine("VC		Thread is running.");
		processHeartbeats();
	
		if (communicate() == SUCCESS && checkData() == SUCCESS)
		{
			processSharedMemory();
		}
		Thread::Sleep(20);
	}
	Console::WriteLine("VC		Thread is terminating");

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
			return ERR_VC_FAILURE;
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

error_state VC::checkData()
{
	return SUCCESS;
}

error_state VC::processSharedMemory()
{
	return SUCCESS;
}

VC::~VC()
{
	Stream->Close();
	Client->Close();
}

static float processSpeed(float leftTrigger, float rightTrigger) {
	const double MAX_SPEED = 10;
	return (rightTrigger - leftTrigger) * MAX_SPEED;
}

static float processSteering(double rightThumbX) {
	const double MAX_STEERING_ANGLE = 40;
	return rightThumbX * MAX_STEERING_ANGLE;
}
/////////////////////////////////////////////////////////////////////////////
//float processSpeed(float leftTrigger, float rightTrigger) {
//	const float MAX_SPEED = 10.0f;
//	return (leftTrigger - rightTrigger) * MAX_SPEED;
//}
//
//
//float processSteering(float joystickX) {
//	const float MAX_STEERING_ANGLE = 45.0f;
//	return joystickX * MAX_STEERING_ANGLE;
//}
//
//void sendUGVCommand(float speed, float steeringAngle) {
//	static int flag = 0; // Alternating flag
//	std::string command = "#" + std::to_string(steeringAngle) + " " +
//		std::to_string(speed) + " " + std::to_string(flag) + "#";
//	// Send this command to the UGV
//	ugvInterface.sendCommand(command);
//	flag = 1 - flag; // Alternate flag
//}
//void VehicleControl::threadFunction()
//{
//	// Other initialization, like setting up communication with UGV
//
//	while (!getShutdownFlag()) {
//		// Process controller data for UGV commands
//		float speed = processSpeed(controllerData->leftTrigger, controllerData->rightTrigger);
//		float steeringAngle = processSteering(controllerData->joystickX);
//
//		// Send commands to UGV
//		// This should be adapted to your UGV's specific command interface
//		sendUGVCommand(speed, steeringAngle);
//
//		int i = 0;
//		Console::WriteLine("vc starting");
//		Watch = gcnew Stopwatch;
//		SM_TM_->ThreadBarrier->SignalAndWait();
//		Watch->Start();
//		while (!Console::KeyAvailable && !getShutdownFlag())
//		{
//			Console::WriteLine("vc running");
//			processHeartbeats();
//			if (communicate() == SUCCESS && checkData() == SUCCESS)
//			{
//				processSharedMemory();
//			}
//			Thread::Sleep(20);
//			if (i++ > 100) break;
//		}
//		Console::WriteLine("vc terminated");
//	}//vc ques bracket
//}