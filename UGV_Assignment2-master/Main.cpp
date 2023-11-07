#include "TMM.h"

int main()
{
	ThreadManagement^ myTMM = gcnew ThreadManagement();

	myTMM->setupSharedMemory();
	myTMM->threadFunction();

	Console::ReadKey();
	Console::ReadKey();
	return 0;
}