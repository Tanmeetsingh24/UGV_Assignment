#include "TMM.h"
#include "Display.h"
#include "Laser.h"
#include <conio.h>
#include "GNSS.h"
#include "VC.h"

using namespace System;
using namespace System::Threading;

int main()
{
	

	ThreadManagement^ myTMM = gcnew ThreadManagement();
	

	//SM_ThreadManagement^ TMM = gcnew SM_ThreadManagement();

	
	//SM_Laser^ SM_LMS151 = gcnew SM_Laser();
	//Laser^ LMS151 = gcnew Laser(TMM, SM_LMS151);
	//LMS151->connect(WEEDER_ADDRESS, 23000/*2112*/);


	//SM_GNSS^ SM_GPSModule = gcnew SM_GNSS();

	//GNSS^ GPSModule = gcnew GNSS(TMM, SM_GPSModule);
	//GPSModule->connect(WEEDER_ADDRESS, 24000);

	//SM_VehicleControl^ SM_VControl = gcnew SM_VehicleControl();

	//VC^ VControl = gcnew VC(TMM, SM_VControl);
	//VControl->connect(WEEDER_ADDRESS, 25000);

	//Display^ MatlabGraph = gcnew Display(TMM, SM_LMS151, SM_GPSModule);
	//MatlabGraph->connect(DISPLAY_ADDRESS, 28000);
	

	
	

	/*Display^ MatlabGraph = gcnew Display(nullptr, SM_LMS151, nullptr);*/
		
	while (!_kbhit())
	{
		//LMS151->communicate();
		//LMS151->processSharedMemory();
		//MatlabGraph->FakeLaser();
		//MatlabGraph->communicate();
		myTMM->setupSharedMemory();
		myTMM->threadFunction();
		Thread::Sleep(50);
	}
	
	Console::ReadKey();
	Console::ReadKey();
	return 0;
}