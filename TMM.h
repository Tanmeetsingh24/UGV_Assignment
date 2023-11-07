#pragma once

#using <System.dll>
#include "SMObjects.h"
#include "UGVModule.h"

using namespace System;
using namespace System::Threading;
using namespace System::Diagnostics;

ref struct ThreadProperties
{
    ThreadStart^ ThreadStart_;
    bool Critical;
    String^ ThreadName;
    uint8_t BitID;

    ThreadProperties(ThreadStart^ start, bool crit, uint8_t bit_id, String^ threadName)
    {
        ThreadStart_ = start;
        Critical = crit;
        ThreadName = threadName;
        BitID = bit_id;

    }
};


ref class ThreadManagement : public UGVModule
{
public:
    // Create shared memory objects
    error_state setupSharedMemory();
    void threadFunction() override;               // Send/Recieve data from shared memory structures
    error_state processHeartbeats();             //error_state processSharedMemory();
    void shutdownThreads();
    bool getShutdownFlag() override;
    error_state processSharedMemory() override;   
    void shutdownModules() override;             //Defining this to shutdown the modules
    

private:
    // Add any additional data members or helper functions here

    SM_ThreadManagement^ SM_TM_;
    SM_Laser^ SM_Laser_;
    SM_GNSS^ SM_GNSS_;
    SM_VehicleControl^ SM_VehicleControl_;
    SM_Controller_^ SM_Controller_;
    array<Stopwatch^>^ StopwatchList;
    array<Thread^>^ ThreadList;
    array<ThreadProperties^>^ ThreadPropertiesList;
};


/*
Setup shared memory
Thread Function
process heartbeats
shutdown modules
getshutdownflag
*/


