#include "System.h"

volatile unsigned int System::runningTimeCntr=20;
volatile unsigned int System::lockFlag=1;
volatile unsigned int System::context_switch_on_demand=0;

pInterrupt oldTimer;
unsigned stara=0x8;
unsigned nova=0x60;

//pomocne promenljive za prekid tajmera
unsigned tsp;
unsigned tss;
unsigned tbp;
unsigned tid;

// nova prekidna rutina tajmera
void interrupt timer(...){

	if (!System::context_switch_on_demand) System::runningTimeCntr--;
	if (System::runningTimeCntr == 0 || System::context_switch_on_demand) {
		if(System::lockFlag){
			System::context_switch_on_demand=0;
			asm {
				// cuva sp
				mov tsp, sp
				mov tss, ss
				mov tbp, bp
			}

			PCB::runnig->sp = tsp;
			PCB::runnig->ss=tss;
			PCB::runnig->bp=tbp;

			// scheduler
			//PCB::runnig = getNextPCBToExecute();

			if(PCB::runnig->state!=PCB::FINISHED) Scheduler::put((PCB*)PCB::runnig);
			PCB::runnig=Scheduler::get();

			if(PCB::runnig->quantum==0){
				Scheduler::put((PCB*)PCB::runnig);
				PCB::runnig=Scheduler::get();
			}

			tsp = PCB::runnig->sp;
			tss = PCB::runnig->ss;
			tbp = PCB::runnig->bp;

			tid = PCB::runnig->id;
			System::runningTimeCntr = PCB::runnig->quantum;


			asm {
				// restaurira sp
				mov sp, tsp
				mov ss, tss
				mov bp, tbp
			}
		}
		else{
			System::context_switch_on_demand=1;
		}
	}

	// poziv stare prekidne rutine
	// koja se nalazila na 08h, a sad je na 60h;
	// poziva se samo kada nije zahtevana promena konteksta
	// tako da se stara rutina poziva
	// samo kada je stvarno doslo do prekida
	if(!System::context_switch_on_demand) asm int 60h;

	//context_switch_on_demand = 0;
}

// postavlja novu prekidnu rutinu
void System::inic(){
	asm cli
	//cuvam staru prekidnu rutinu
	oldTimer=getvect(stara);
	//na 08h stavljam novu
	setvect(stara,timer);
	//staru premestam na 60h
	setvect(nova,oldTimer);
	asm sti
}

// vraca staru prekidnu rutinu
void System::restore(){
	asm cli
	setvect(stara,oldTimer);
	asm sti
}

// sinhrona promena konteksta
void System::dispatch(){
	asm cli
	System::context_switch_on_demand = 1;
	timer();
	asm sti;
}

void exitThread(){
	PCB::runnig->state = PCB::FINISHED;
	System::dispatch();
}
