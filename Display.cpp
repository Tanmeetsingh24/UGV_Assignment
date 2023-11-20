#using<System.dll>
#include "Display.h"
#include "NetworkedModule.h"

using namespace System;
using namespace System::Threading;
using namespace System::Diagnostics;

Display::Display(SM_ThreadManagement^ SM_TM, SM_Laser^ SM_Laser, SM_GNSS^ SM_GNSS)
{

	SM_Laser_ = SM_Laser;
	SM_GNSS_ = SM_GNSS;
	SM_TM_ = SM_TM;
	Watch = gcnew Stopwatch;
}

void Display::threadFunction()
{
	connect(DISPLAY_ADDRESS, 28000);

	Console::WriteLine("Display		Thread is starting.");
	//setup the stopwatch
	Watch = gcnew Stopwatch;
	//barrier
	SM_TM_->ThreadBarrier->SignalAndWait();
	Watch->Start();
	while ( !getShutdownFlag())
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
			return ERR_DISPLAY_FAILURE;
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

error_state Display::connect(String^ hostName, int portNumber)
{
	try
	{
		Client = gcnew TcpClient(hostName, portNumber);
	}
	catch (int error)
	{
		Console::WriteLine("Error: Failed to establish connection with Laser err_code=%d", error);
		shutdownModules();
		return ERR_CONNECTION;
	}
	
	Client->NoDelay = true;
	Client->ReceiveTimeout = 2500;
	Client->SendTimeout = 2500;
	Client->ReceiveBufferSize = 1024;
	Client->SendBufferSize = 1024;
	Stream = Client->GetStream();

	SendData = gcnew array<unsigned char>(64);
	ReadData = gcnew array<unsigned char>(64);
	return SUCCESS;
}
error_state Display::communicate()
{
	sendDisplayData();
	return SUCCESS;
}

void Display::sendDisplayData()
{
	// Serialize the data arrays to a byte array
	//(format required for sending)

	Monitor::Enter(SM_Laser_->lockObject); //Mutex Start

	array<Byte>^ dataX = gcnew array<Byte>(SM_Laser_->x->Length * sizeof(double));
	Buffer::BlockCopy(SM_Laser_->x, 0, dataX, 0, dataX->Length);
	array<Byte>^ dataY = gcnew array<Byte>(SM_Laser_->y->Length * sizeof(double));
	Buffer::BlockCopy(SM_Laser_->y, 0, dataY, 0, dataY->Length);

	Monitor::Exit(SM_Laser_->lockObject);			//Mutex End

	// Send byte data over connection
	Stream->Write(dataX, 0, dataX->Length);
	Thread::Sleep(10);
	Stream->Write(dataY, 0, dataY->Length);
}

error_state Display::processSharedMemory()
{
	return SUCCESS;
}

error_state Display::checkData()
{
	return SUCCESS;
}


//void Display::FakeLaser() // SM_Laser_ should give us the access to real laer data. This is just to test the data being displayed on the Matlab module
//{
//	SM_Laser_ = gcnew SM_Laser();
//
//	for (int i = 0; i < 361; i++)
//	{
//		SM_Laser_->y[i] = 5000.0 * Math::Sin(i * 0.5 * Math::PI / 180.0); //Radius = 5000.0, X is Sin function according to the Laser Module, i*0.5 is gonna take us to 180 degrees, pi/180 is conversion from degree to radians
//		SM_Laser_->x[i] = 5000.0 * Math::Cos(i * 0.5 * Math::PI / 180.0);
//	}
//}
