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

    Display(SM_ThreadManagement^ SM_TM, SM_Display^ SM_Display_);

    error_state setupSharedMemory();
    void threadFunction() override;
    error_state processHeartbeats();
    void shutdownModules() override;
    bool getShutdownFlag() override;
    error_state communicate() override;
    error_state checkData();
    error_state processSharedMemory() override;
    error_state connect(String^ hostName, int portNumber) override;


    ~Display() {};

private:
    SM_ThreadManagement^ SM_TM_;
    SM_Display^ SM_Display_;
    Stopwatch^ Watch;
};

