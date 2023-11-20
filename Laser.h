#pragma once
#using <System.dll>
#include "SMObjects.h"
#include "NetworkedModule.h"

using namespace System;
using namespace System::Threading;
using namespace System::Diagnostics;
using namespace System::Net::Sockets;
using namespace System::Text;

ref class Laser : public NetworkedModule
{
public:
    
    Laser(SM_ThreadManagement^ SM_TM, SM_Laser^ SM_Laser);

    void threadFunction() override;
    error_state processHeartbeats();
    void shutdownModules() override;
    bool getShutdownFlag() override;
    error_state communicate() override;
    error_state checkData();
    error_state processSharedMemory() override;
    error_state connect(String^ hostName, int portNumber) override;
    error_state Laser::authenticate();
       
    ~Laser() {};

private:
    
    SM_Laser^ SM_Laser_;
    Stopwatch^ Watch;
    array<unsigned char>^ SendData;
    String^ ResponseData;

    
    bool bytesRequired;
    int StartByte = 0x02;
    int EndByte = 0x03;
    String^ Command; 

};