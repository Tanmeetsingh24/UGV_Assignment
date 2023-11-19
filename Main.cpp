#include "TMM.h"
#include "Display.h"
#include "Laser.h"
#include <conio.h>

using namespace System;
using namespace System::Threading;

int main()
{
	//Display^ MatlabGraph = gcnew Display(SM_TM->SM_TM_, SM_Laser_,SM_GNSS_);
	ThreadManagement^ myTMM = gcnew ThreadManagement();

	myTMM->setupSharedMemory();
	myTMM->threadFunction();
	SM_Laser^ SM_LMS151 = gcnew SM_Laser();
	Laser^ LMS151 = gcnew Laser(nullptr, SM_LMS151);
	LMS151->connect("192.168.5.15", 2112);

	Display^ MatlabGraph = gcnew Display(nullptr, SM_LMS151, nullptr);
	MatlabGraph->connect("127.0.0.1", 28000);

	

	while (!_kbhit())
	{
		LMS151->communicate();
		LMS151->processSharedMemory();
		MatlabGraph->communicate();
		Thread::Sleep(50);
	}
	
	Console::ReadKey();
	Console::ReadKey();
	return 0;
}