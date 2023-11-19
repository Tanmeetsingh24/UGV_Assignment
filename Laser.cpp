#using<System.dll>
#include "Laser.h"
#include "UGVModule.h"
#include "TMM.h"

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
	Client = gcnew TcpClient(hostName, portNumber);
	Stream = Client->GetStream();
	Client->NoDelay = true;
	Client->ReceiveTimeout = 500;
	Client->SendTimeout = 500;
	Client->ReceiveBufferSize = 1024;
	Client->SendBufferSize = 1024;

	SendData = gcnew array<unsigned char>(64);
	ReadData = gcnew array<unsigned char>(2048);
	return SUCCESS;
}

void Laser::threadFunction()
{
	Console::WriteLine("Laser		Thread is starting.");
	//setup the stopwatch
	Watch = gcnew Stopwatch;
	//barrier
	SM_TM_->ThreadBarrier->SignalAndWait();
	Watch->Start();
	while (/*!Console::KeyAvailable && */ !getShutdownFlag())
	{
		Console::WriteLine("Laser		Thread is running.");
		processHeartbeats();
		if (communicate() == SUCCESS && checkData() == SUCCESS)
		{
			processSharedMemory();
		}
		Thread::Sleep(20);
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
			return ERR_TMM_FAILURE;
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

error_state Laser::communicate()
{
	// sRN LMDscandata
	// send STX byte 0x02
	// send sRN LMDscandata
	// send ETX byte 0x03
		
	//String^ Command = "sRN LMDscandata";
	//
	//SendData = Encoding::ASCII->GetBytes(Command);
	//Stream->WriteByte(0x02); //sending STX
	//Stream->Write(SendData, 0, SendData->Length);
	//Stream->WriteByte(0x03); //sending ETX
	//// wait for some time or use DataAvailable
	//Thread::Sleep(10);
	//Stream->Read(ReadData, 0, ReadData->Length);
	return SUCCESS;
		
}

error_state Laser::checkData()
{
	return SUCCESS;
}

error_state Laser::processSharedMemory()
{	
	//int NumPoints;
	//array<String^>^ Fragments;
	//String^ Response = Encoding::ASCII->GetString(ReadData);
	////Console::WriteLine(Response);
	////check if total number of fields have been received
	//Fragments = Response->Split(' ');
	//
	////Console::WriteLine(Fragments->Length);

	////read data from LRF
	//NumPoints = Convert::ToInt32(Fragments[25], 16);

	////Lock SM
	//Monitor::Enter(SM_Laser_->lockObject);

	////Write to SM
	//for (int i = 0; i < NumPoints; i++)
	//{
	//	SM_Laser_->x[i] = Convert::ToInt32(Fragments[26 + i], 16) * Math::Cos(i * 0.05 * Math::PI / 180.0);
	//	SM_Laser_->y[i] = Convert::ToInt32(Fragments[26 + i], 16) * Math::Sin(i * 0.05 * Math::PI / 180.0);
	//}

	////unlock SM
	//Monitor::Exit(SM_Laser_->lockObject);
	//if (Fragments->Length == 400)
	//	return SUCCESS;
	//else
	//	return ERR_INVALID_DATA;

	//Disect the data string, extract range data and store in shared memory
	
	 
	

	return SUCCESS;
}

//Laser::~Laser()
//{
//	UGV->Close();
//}