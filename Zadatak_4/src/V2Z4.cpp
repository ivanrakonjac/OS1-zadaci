//  v2_zad4.cpp
//  prevodjenje iz komandne linije: bcc -mh -Ic:\bc31\include -Lc:\bc31\lib v2_zad4.cpp

//////////////////////////
//dodata PCB.H i ostale izmene da bi radilo
//////////////////////////

//pretpostavljeni memorijski model: huge
#include <iostream.h>
#include <dos.h>
#include "SCHEDULE.H"
#include "PCB.H"

typedef void interrupt (*pInterrupt)(...); //pok na prekidnu rutinu
pInterrupt oldTimer;
unsigned stara=0x8;
unsigned nova=0x60;

// zabranjuje prekide
#define lock asm cli

// dozvoljava prekide
#define unlock asm sti

PCB** p;
//volatile PCB* running;
volatile int nextThread = 2;

// stara prekidna rutina
unsigned oldTimerOFF, oldTimerSEG;

// deklaracija nove prekidne rutine
void interrupt timer(...);

// postavlja novu prekidnu rutinu
void inic(){
	asm cli;
	//cuvam staru prekidnu rutinu
	oldTimer=getvect(stara);
	//na 08h stavljam novu
	setvect(stara,timer);
	//staru premestam na 60h
	setvect(nova,oldTimer);
	asm sti;
}

// vraca staru prekidnu rutinu
void restore(){
	asm cli;
	setvect(stara,oldTimer);
	asm sti;
}

//pomocne promenljive za prekid tajmera
unsigned tsp;
unsigned tss;
unsigned tbp;
unsigned tid;

volatile unsigned int lockFlag=1;

volatile int cntr = 20;
volatile int context_switch_on_demand = 0;

// nova prekidna rutina tajmera
void interrupt timer(...){
	int dummy=0;
	if (!context_switch_on_demand) cntr--; 
	if (cntr == 0 || context_switch_on_demand) {
		if(lockFlag){
			context_switch_on_demand=0;
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
			cntr = PCB::runnig->quantum;


			asm {
				// restaurira sp
				mov sp, tsp
				mov ss, tss
				mov bp, tbp
			}
		}
		else{
			context_switch_on_demand=1;
		}
	} 

	// poziv stare prekidne rutine 
	// koja se nalazila na 08h, a sad je na 60h;
	// poziva se samo kada nije zahtevana promena konteksta
	// tako da se stara rutina poziva 
	// samo kada je stvarno doslo do prekida
	if(!context_switch_on_demand) asm int 60h;

	//context_switch_on_demand = 0;
}

// sinhrona promena konteksta
void dispatch(){ 
	lock
	context_switch_on_demand = 1;
	timer();
	unlock
}

void exitThread(){
	PCB::runnig->state = PCB::FINISHED;
	dispatch();
}

void a(){
	for (int i =0; i < 30; ++i) {
		lock
		cout<<"u a() i = "<<i<<" id= "<<tid<<endl;
		unlock
		for (int k = 0; k<10000; ++k)
			for (int j = 0; j <30000; ++j);
	}
	exitThread();
}

void b(){
	for (int i =0; i < 30; ++i) {
		lock
		cout<<"u b() i = "<<i<<" id= "<<tid<<endl;
		unlock
		for (int k = 0; k<10000; ++k)
			for (int j = 0; j <30000; ++j);
	}
	exitThread();
}

void c(){
	for (int i =0; i < 30; ++i) {
		lock
		cout<<"u c() i = "<<i<<" id= "<<tid<<endl;
		unlock
		for (int k = 0; k<10000; ++k)
			for (int j = 0; j <30000; ++j);
	}
	exitThread();
}

void f(){
	for (int i =0; i < 30; ++i) {
			lockFlag=0;
			cout<<"funkcija_"<<tid<<" "<<i<<endl;
			lockFlag=1;
			if(context_switch_on_demand)
				dispatch();
			for (int k = 0; k<10000; ++k)
				for (int j = 0; j <30000; ++j);
		}
		exitThread();
}


void doSomething(){
	lock

	//testiram sa 10 niti

	p= new PCB*[10];

	for (int br = 1; br < 10; ++br) {
		p[br] = new PCB(1024,(br%2)?40:20,f);  //(br%2)?40:20 da bi se niti razlikovale po vremenu izvrsavanja
		Scheduler::put(p[br]);
	}


	p[0] = new PCB();
	PCB::runnig = p[0];
	unlock

	for (int i = 0; i < 15; ++i) {
		lock
		cout<<"main "<<i<<endl;
		unlock

		for (int j = 0; j< 30000; ++j)
			for (int k = 0; k < 30000; ++k);
	}
	cout<<"Srecan kraj!"<<endl;
}

int main(){
	
	inic();

	doSomething();

	restore();

	return 0;

}
