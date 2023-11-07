//#pragma once
//#using <System.dll>
//#include "SMObjects.h"
//#include "NetworkedModule.h"
//#include "ControllerInterface.h"
//
//using namespace System;
//using namespace System::Threading;
//using namespace System::Diagnostics;
//
//ref class Controller : public UGVModule
//{
//public:
//
//    Controller(SM_ThreadManagement^ SM_TM, SM_Controller^ SM_Controller);
//
//    error_state setupSharedMemory();
//    void threadFunction() override;
//    error_state processHeartbeats();
//    void shutdownModules() override;
//    bool getShutdownFlag() override;
//    error_state communicate() override;
//    error_state checkData();
//    error_state processSharedMemory() override;
//    error_state connect(String^ hostName, int portNumber) override;
//
//
//    ~Controller() {};
//
//private:
//    SM_ThreadManagement^ SM_TM_;
//    SM_VehicleControl^ SM_VehicleControl_;
//    Stopwatch^ Watch;
//};
//
