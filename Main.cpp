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
	
	//VControl->connect(WEEDER_ADDRESS, 25000);

	//Display^ MatlabGraph = gcnew Display(TMM, SM_LMS151, SM_GPSModule);
	//MatlabGraph->connect(DISPLAY_ADDRESS, 28000);

	while (!_kbhit())
	{
		myTMM->setupSharedMemory();
		myTMM->threadFunction();
		Thread::Sleep(50);
	}
	
	Console::ReadKey();
	Console::ReadKey();
	return 0;
}