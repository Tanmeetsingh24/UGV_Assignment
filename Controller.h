#pragma once
#using <System.dll>
#include "SMObjects.h"
#include "NetworkedModule.h"

using namespace System;
using namespace System::Threading;
using namespace System::Diagnostics;
using namespace System::Net::Sockets;
using namespace System::Text;

ref class Controller : public UGVModule
{
public:

    Controller(SM_ThreadManagement^ SM_TM, SM_Controller^ SM_Controller);


    error_state processSharedMemory() override;
    bool getShutdownFlag() override;
    void shutdownModules() override;
    void threadFunction() override;
    error_state processHeartbeats();
   

    ~Controller() {};

private:

    SM_Controller^ SM_Controller_;
    Stopwatch^ Watch;
    array<unsigned char>^ SendData;

};

