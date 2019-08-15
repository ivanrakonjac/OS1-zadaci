//  v2_zad4.cpp
//  prevodjenje iz komandne linije: bcc -mh -Ic:\bc31\include -Lc:\bc31\lib v2_zad4.cpp

//pretpostavljeni memorijski model: huge
#include <iostream.h>
#include <dos.h>
#include "SCHEDULE.H"

unsigned int ID=0;
unsigned int n; //broj niti

// zabranjuje prekide
#define lock asm cli

// dozvoljava prekide
#define unlock asm sti

struct PCB{
	unsigned bp;
	unsigned sp;
	unsigned ss;
	unsigned finished;
	int quantum;
	int id;
};

//PCB *p[3];
PCB** p;
volatile PCB* running; 
volatile int nextThread = 2;

// rasporedjivac niti (scheduler)
PCB* getNextPCBToExecute(){
	if (nextThread == 1)
		nextThread = 2;
	else nextThread = 1;
	
	if (p[nextThread]->finished){
		if (nextThread == 1)
			nextThread = 2;
		else nextThread = 1;
		if (p[nextThread]->finished)
			nextThread = 0;
	}
	return p[nextThread];
}

// stara prekidna rutina
unsigned oldTimerOFF, oldTimerSEG;

// deklaracija nove prekidne rutine
void interrupt timer();

// postavlja novu prekidnu rutinu
void inic(){
	asm{
		cli
		push es
		push ax
		
		mov ax,0
		mov es,ax // es = 0
		
		// pamti staru rutinu
		mov ax, word ptr es:0022h
		mov word ptr oldTimerSEG, ax
		mov ax, word ptr es:0020h
		mov word ptr oldTimerOFF, ax
		
		// postavlja novu rutinu
		mov word ptr es:0022h, seg timer
		mov word ptr es:0020h, offset timer
		
		// postavlja staru rutinu na int 60h
		mov ax, oldTimerSEG
		mov word ptr es:0182h, ax
		mov ax, oldTimerOFF
		mov word ptr es:0180h, ax

		pop ax
		pop es
		sti
	}
}

// vraca staru prekidnu rutinu
void restore(){
	asm {
		cli
		push es
		push ax

		mov ax,0
		mov es,ax

		mov ax, word ptr oldTimerSEG
		mov word ptr es:0022h, ax
		mov ax, word ptr oldTimerOFF
		mov word ptr es:0020h, ax

		pop ax
		pop es
		sti
	}
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
void interrupt timer(){
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

			running->sp = tsp;
			running->ss = tss;
			running->bp = tbp;

			// scheduler
			//running = getNextPCBToExecute();
			if(!running->finished) Scheduler::put((PCB*)running);
			running=Scheduler::get();

			if(running->quantum == 0){
				Scheduler::put((PCB*)running);
				running=Scheduler::get();
			}

			tsp = running->sp;
			tss = running->ss;
			tbp = running->bp;

			tid = running->id;
			cntr = running->quantum;

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
	running->finished = 1;
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

void createProcess(PCB *newPCB, void (*body)()){
	unsigned* st1 = new unsigned[1024];
	// setovanje I flega u pocetnom PSW-u za nit
	st1[1023] =0x200;
	// postavljanje adrese funkcije koju ce nit da izvrsava
#ifndef BCC_BLOCK_IGNORE
	st1[1022] = FP_SEG(body);
	st1[1021] = FP_OFF(body);
	//svi sacuvani registri pri ulasku u interrupt rutinu
	newPCB->sp = FP_OFF(st1+1012); 
	newPCB->ss = FP_SEG(st1+1012);
	newPCB->bp = FP_OFF(st1+1012);
#endif
	// oznaka za kraj rada niti je na pocetku 0
	newPCB->finished = 0;
	newPCB->id=++ID;
}

void doSomething(){
	lock
	/*
	p[1] = new PCB();
	createProcess(p[1],a);
	cout<<"Napravio a"<<endl;
	p[1]->quantum = 40;
	Scheduler::put(p[1]);

	p[2] = new PCB();
	createProcess(p[2],b);
	cout<<"Napravio b"<<endl;
	p[2]->quantum = 20;
	Scheduler::put(p[2]);

	p[3] = new PCB();
	createProcess(p[3],c);
	cout<<"Napravio c"<<endl;
	p[3]->quantum = 10;
	Scheduler::put(p[3]);
	*/

	cout<<"Koliko niti zelite da kreirate?"<<endl;
	cin>>n;
	cout<<n<<endl;

	p=new PCB*[n];

	for(int b=0;b<n;++b){
		p[b] = new PCB();
		createProcess(p[b],f);
		cout<<"Napravio funkciju_"<<b<<endl;
		p[b]->quantum = 20;
		Scheduler::put(p[b]);
	}

	p[0] = new PCB();
	p[0]->finished=0;
	p[0]->quantum=0;

	running = p[0];
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
