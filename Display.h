#pragma once
#using <System.dll>
#include "SMObjects.h"
#include "NetworkedModule.h"

using namespace System;
using namespace System::Threading;
using namespace System::Diagnostics;

ref class Display : public NetworkedModule
{
public:

    Display(SM_ThreadManagement^ SM_TM, SM_Laser^ SM_Laser, SM_GNSS^ SM_GNSS);

    void threadFunction() override;
    error_state processHeartbeats();
    void shutdownModules() override;
    bool getShutdownFlag() override;
    error_state communicate() override;
    error_state processSharedMemory() override;
    error_state connect(String^ hostName, int portNumber) override;
    error_state checkData();
    //void FakeLaser();   //Comment Out after Testing the Matlab Display

    void sendDisplayData();

    ~Display() {};

private:
    //List of inherited Data Members
    //SM_ThreadManagement^ SM_TM_;  Already Initialized in UGVModule
    //TcpClient^ Client;					// Handle for TCP connection
    //NetworkStream^ Stream;				// Handle for TCP data stream
    //array<unsigned char>^ ReadData;		// Array to store sensor Data (only used for sensor modules)
    
    array<unsigned char>^ SendData;   //Array to store Sensor Data
    SM_Laser^ SM_Laser_;
    SM_GNSS^ SM_GNSS_;    
    Stopwatch^ Watch;
 
};

