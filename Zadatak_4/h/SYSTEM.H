#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include "PCB.H"
#include <iostream.h>
#include "SCHEDULE.H"
#include <DOS.H>

//class PCB;

typedef void interrupt (*pInterrupt)(...); //pok na prekidnu rutinu
void interrupt timer(...);


#define lock System::lockFlag=0;

#define unlock System::lockFlag = 1; \
if(System::context_switch_on_demand) System::dispatch();

class System{

public:

	static volatile unsigned int runningTimeCntr;
	static volatile unsigned int lockFlag;
	static volatile unsigned int context_switch_on_demand;

	static void inic();
	static void restore();
	static void dispatch();
};

#endif
