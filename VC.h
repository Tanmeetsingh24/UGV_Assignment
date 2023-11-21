#pragma once
#using <System.dll>
#include "SMObjects.h"
#include "NetworkedModule.h"

using namespace System;
using namespace System::Threading;
using namespace System::Diagnostics;

//constexpr double L = 0.80; // UGV length (wheel base) in meters
//constexpr double W = 0.60; // UGV width (wheel track) in meters
//constexpr double r = 0.21; // UGV wheel radius in meters
//constexpr int CWheel = 8192; // counts per revolution of a wheel
//constexpr int CSteering = 4096; // counts per revolution of the steering motor

ref class VC : public NetworkedModule
{
  /*  TcpListener^ Server;
    String^ ServerIPAddress;
    int ServerPort;
    NetworkStream^ ServerStream;
    TcpClient^ Client;
    String^ GalilIPAddress;
    int GalilPort;
    NetworkStream^ ClientStream;
    TcpClient^ IncomingClient;
    Stopwatch^ Watchdog;
    String^ Response;
    double SetSpeed;
    double SetSteering;
    int Flag;*/


public:

 /*   VC() {};
    VC(String^ serverIPAddress, int serverPort, String^ clientIPAddress, int clientPort);
       
    error_state ConnectAsServer();
    error_state StartServer();
    error_state ConnectToGalil();
    error_state CommunicateWithGalil();
    int ReadEncoder();
    error_state CommunicateAsServer();
    bool WatchdogOK();
    bool IncomingClientConnected();
    void IncomingClientClose();*/
    


    VC(SM_ThreadManagement^ SM_TM, SM_VehicleControl^ SM_VC_);

    void threadFunction() override;
    error_state processHeartbeats();
    void shutdownModules() override;
    bool getShutdownFlag() override;
    error_state communicate() override;
    error_state checkData();
    error_state processSharedMemory() override;
    error_state connect(String^ hostName, int portNumber) override;
    
    error_state VC::authenticate();

    ~VC();

private:
    SM_ThreadManagement^ SM_TM_;
    SM_VehicleControl^ SM_VehicleControl_;
    Stopwatch^ Watch;
    array<unsigned char>^ SendData;
    String^ ResponseData;
    int flag;

    double Steering;
    double Speed;

};

