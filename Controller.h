#pragma once
#using <System.dll>
#include "SMObjects.h"
#include "NetworkedModule.h"

using namespace System;
using namespace System::Threading;
using namespace System::Diagnostics;
using namespace System::Net::Sockets;
using namespace System::Text;


struct ControllerData {
    float leftTrigger;  // For propulsion speed
    float rightTrigger; // For propulsion speed
    float joystickX;    // For steering
};

ref class Controller : public UGVModule
{
public:

    Controller(SM_ThreadManagement^ SM_TM, SM_Controller^ SM_Controller);

    //Controller(SM_ThreadManagement^ SM_TM, SM_VehicleControl^ SM_VC); Implement this or the other constructor

    error_state processSharedMemory() override;
    bool getShutdownFlag() override;
    void shutdownModules() override;
    void threadFunction() override;
    error_state processHeartbeats();
   
    error_state checkData();

    ~Controller();

private:

    SM_Controller^ SM_Controller_;
    Stopwatch^ Watch;
    array<unsigned char>^ SendData;

};

