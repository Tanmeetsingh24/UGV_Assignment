#pragma once
#using <System.dll>
#include "SMObjects.h"
#include "NetworkedModule.h"

using namespace System;
using namespace System::Threading;
using namespace System::Diagnostics;

ref class VC : public NetworkedModule
{
public:
   
    VC (SM_ThreadManagement^ SM_TM, SM_VehicleControl^ SM_VC_);

    error_state setupSharedMemory();
    void threadFunction() override;
    error_state processHeartbeats();
    void shutdownModules() override;
    bool getShutdownFlag() override;
    error_state communicate() override;
    error_state checkData();
    error_state processSharedMemory() override;
    error_state connect(String^ hostName, int portNumber) override;
    

    ~VC() {};

private:
    SM_ThreadManagement^ SM_TM_;
    SM_VehicleControl^ SM_VehicleControl_;
    Stopwatch^ Watch;
};

