#using<System.dll>
#include "Laser.h"

using namespace System;
using namespace System::Threading;
using namespace System::Diagnostics;

Laser::Laser(SM_ThreadManagement^ SM_TM, SM_Laser^ SM_Laser)
{
	SM_Laser_ = SM_Laser;
	SM_TM_ = SM_TM;
	Watch = gcnew Stopwatch;
}

error_state Laser::connect(String^ hostName, int portNumber)
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

	SendData = gcnew array<unsigned char>(2048);
	ReadData = gcnew array<unsigned char>(2048);


	return SUCCESS;
}
error_state Laser::communicate()
{
	//trying a new code
	String^ Command = "sRN LMDscandata";
	try 
	{
		SendData = Encoding::ASCII->GetBytes(Command);
		Stream->WriteByte(0x02);
		Stream->Write(SendData, 0, SendData->Length);
		Stream->WriteByte(0x03);
		Thread::Sleep(20);
		Stream->Read(ReadData, 0, ReadData->Length);
	}
	catch (int error)
	{
		Console::WriteLine("Error: Failed to write request to Laser err_code=%d", error);
		shutdownModules();
		return ERR_WRITE;
	}
	Thread::Sleep(10);
	try
	{
		ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);
	}
	catch (int error)
	{
		Console::WriteLine("Error: Failed to read response from Laser err_code=%d", error);
		shutdownModules();
		return ERR_READ;
	}
	return SUCCESS;

}

error_state Laser::authenticate()
{
	//String^ Student_ID = ("5510198\n");
	Command = ("5510198\n");
	SendData = Encoding::ASCII->GetBytes(Command);
	Stream->Write(SendData, 0, SendData->Length);
	bytesRequired = false;
	Thread::Sleep(10);

	communicate();

	if (ResponseData->Contains("OK"))
	{
		Console::WriteLine("Laser: Authenticated");
		return SUCCESS;
	}
	else
	{
		Console::WriteLine("Laser: Authentication failed");
		return ERR_CONNECTION; //print error
	}
	return SUCCESS;

}

error_state Laser::processSharedMemory()
{
	int NumPoints;
	
	array<String^>^ Fragments;
	String^ ResponseData = Encoding::ASCII->GetString(ReadData);

	for (int i = 0; i < STANDARD_LASER_LENGTH; i++) {
		Console::WriteLine("x:{0, 0:F4} y:{1, 0:F4}", SM_Laser_->x[i], SM_Laser_->y[i]);
	}
	//check if total number of fields have been received
	
	if (ResponseData->Length > 360) {
		Fragments = ResponseData->Split(' ');

		

		//read data from LRF


		NumPoints = Convert::ToInt32(Fragments[25], 16);
		//Lock SM
		Monitor::Enter(SM_Laser_->lockObject);

		//Write to SM/


		for (int i = 0; i < NumPoints; i++)
		{
			SM_Laser_->x[i] = Convert::ToInt32(Fragments[26 + i], 16) * Math::Cos(i * 0.5 * Math::PI / 180.0);
			SM_Laser_->y[i] = Convert::ToInt32(Fragments[26 + i], 16) * Math::Sin(i * 0.5 * Math::PI / 180.0);
		}
		Console::WriteLine("{0:D5} {1:D5}", NumPoints, Fragments->Length);
	}
	else
		return ERR_INVALID_DATA;

	//unlock SM
	Monitor::Exit(SM_Laser_->lockObject);
			
	return SUCCESS;
}

void Laser::threadFunction()
{
	connect(WEEDER_ADDRESS, 23000/*2112*/);

	authenticate();

	Console::WriteLine("Laser		Thread is starting.");
	//setup the stopwatch
	Watch = gcnew Stopwatch;
	//barrier
	SM_TM_->ThreadBarrier->SignalAndWait();
	Watch->Start();
	while (!getShutdownFlag())
	{
		Console::WriteLine("Laser		Thread is running.");
		processHeartbeats();
		//readValues();
		if (communicate() == SUCCESS && checkData() == SUCCESS)
		{
			processSharedMemory();

			String^ print = "";
			for (int i = 0; i < SM_Laser_->x->Length; i++)
			{
				print = SM_Laser_->x[i] + "   " + SM_Laser_->y[i] + "\n";
			}
			Console::WriteLine(print);
		}
		Thread::Sleep(100);
	}
	Console::WriteLine("Laser		thread is terminating");

}

error_state Laser::processHeartbeats()
{
	if ((SM_TM_->heartbeat & bit_LASER) == 0)
	{
		SM_TM_->heartbeat |= bit_LASER;
		Watch->Restart();
	}
	else
	{
		if (Watch->ElapsedMilliseconds > CRASH_LIMIT)
		{
			shutdownModules();
			return ERR_LASER_FAILURE;
		}
	}
	//Heartbeat byte down?

	return SUCCESS;
}

void Laser::shutdownModules()
{
	SM_TM_->shutdown = 0xFF;
}

bool Laser::getShutdownFlag()
{
	return SM_TM_->shutdown & bit_LASER;
}

error_state Laser::checkData()
{
	return SUCCESS;
}


