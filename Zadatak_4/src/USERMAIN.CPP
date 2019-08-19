//usermain.cpp

#include <iostream.h>
#include <dos.h>
#include "SCHEDULE.H"
#include "PCB.H"
#include "SYSTEM.H"

// zabranjuje prekide
#define lock asm cli

// dozvoljava prekide
#define unlock asm sti

PCB** p; //pointer na niz pointera na PCB

void f(){
	for (int i =0; i < 30; ++i) {
		System::lockFlag=0;
		cout<<"funkcija_"<<PCB::runnig->id<<" "<<i<<endl;
		System::lockFlag=1;
		if(System::context_switch_on_demand)
			System::dispatch();
		for (int k = 0; k<10000; ++k)
			for (int j = 0; j <30000; ++j);
	}

	lock
	cout<<"funckija_"<<PCB::runnig->id<<" GOTOVA!"<<endl;
	unlock
	exitThread();
}


void doSomething(){
	lock


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

void tick() {
	//u javnom testu
}

int userMain(int argc, char *argv[]) {
	cout<<"USer main"<<endl;

	doSomething();

	return 0;
}
