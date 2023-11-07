#using <System.dll>

#include "TMM.h"
#include "Laser.h"
#include "GNSS.h"
#include "SMObjects.h"
#include "VC.h"


// Create shared memory objects
error_state ThreadManagement::setupSharedMemory()
{
	SM_TM_ = gcnew SM_ThreadManagement;
	SM_Laser_ = gcnew SM_Laser;
	SM_GNSS_ = gcnew SM_GNSS;

	return SUCCESS;
}

void ThreadManagement::threadFunction()
{
	Console::WriteLine("TMM		Thread is starting. ");
	//make a list of thread properties
	ThreadPropertiesList = gcnew array<ThreadProperties^>
	{	gcnew ThreadProperties(gcnew ThreadStart(gcnew VC(SM_TM_, SM_VehicleControl_), &VC::threadFunction), true, bit_VC, "Vehicle Control Thread"),
		gcnew ThreadProperties(gcnew ThreadStart(gcnew Laser(SM_TM_, SM_Laser_), &Laser::threadFunction), true, bit_LASER, "Laser Thread"),
		gcnew ThreadProperties(gcnew ThreadStart(gcnew GNSS(SM_TM_, SM_GNSS_), &GNSS::threadFunction), true, bit_GPS, "GNSS Thread")};
	//make a list of threads
	ThreadList = gcnew array<Thread^>(ThreadPropertiesList->Length);
	//allocate space for the stop watch list
	StopwatchList = gcnew array<Stopwatch^>(ThreadPropertiesList->Length);
	//make thread barrier
	SM_TM_->ThreadBarrier = gcnew Barrier(ThreadPropertiesList->Length + 1);
	//start all the threads
	for (int i = 0; i < ThreadPropertiesList->Length; i++)
	{
		StopwatchList[i] = gcnew Stopwatch;
		ThreadList[i] = gcnew Thread(ThreadPropertiesList[i]->ThreadStart_);
		ThreadList[i]->Start();
	}
	//wait at the thread barrier
	SM_TM_->ThreadBarrier->SignalAndWait();
	//start all the stop watches
	for (int i = 0; i < ThreadList->Length; i++)
	{
		StopwatchList[i]->Start();
	}
	//start the thread loop
	while (!Console::KeyAvailable && !getShutdownFlag())
	{
		Console::WriteLine("TMM		Thread is running ");
		processHeartbeats();
		//keep checking the heartbeats
		Thread::Sleep(50);
	}

	//end of thread loop
	//shutdown all threads
	shutdownThreads();

	//join all threads (used so that tmm waits for the gnss and laser threads to terminate properly first)
	for (int i = 0; i < ThreadPropertiesList->Length; i++)
	{
		ThreadList[i]->Join();
	}
	Console::WriteLine("TMM		thread is terminating..");
}
// Send/Recieve data from shared memory structures
error_state ThreadManagement::processHeartbeats()
{
	for (int i = 0; i < ThreadList->Length; i++)
	{
		//check the heartbeat flag of ith thread (is it high?)
		if (SM_TM_->heartbeat & ThreadPropertiesList[i]->BitID)
		{//if high, put the ith bit(flag) down
			SM_TM_->heartbeat ^= ThreadPropertiesList[i]->BitID;
			//Reset the stopwatch
			StopwatchList[i]->Restart();
		}//else
		else
		{
			//check the stopwatch. Has the time exceeded the crash limit?
			if (StopwatchList[i]->ElapsedMilliseconds > CRASH_LIMIT)
			{
				//is ith process a critical process?
				if (ThreadPropertiesList[i]->Critical)
				{
					//shutdown all threads
					Console::WriteLine(ThreadPropertiesList[i]->ThreadName + " failure. Shutting down all threads.");
					shutdownThreads();
					return ERR_CRITICAL_PROCESS_FAILURE;
				}
				else
				{
					Console::WriteLine(ThreadPropertiesList[i]->ThreadName + " failed. Attempting to restart.");
					//else
					ThreadList[i]->Abort();
					//try to restart
					ThreadList[i] = gcnew Thread(ThreadPropertiesList[i]->ThreadStart_);
					SM_TM_->ThreadBarrier = gcnew Barrier(1);
					ThreadList[i]->Start();
				}
			}



		}

	}
	return SUCCESS;
}

void ThreadManagement::shutdownThreads()
{
	SM_TM_->shutdown = 0xFF;  //Ob11111111
}

// Get Shutdown signal for module, from Thread Management SM
bool ThreadManagement::getShutdownFlag()
{
	return (SM_TM_->shutdown & bit_PM);
	return true;
}

void ThreadManagement::shutdownModules()
{

}

error_state ThreadManagement::processSharedMemory()
{
	return SUCCESS;
}

// Thread function for PMM
