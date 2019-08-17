//PCB.CPP

#include "PCB.H"

int PCB::ajdi=0;
PCB* PCB::runnig=0;

PCB::PCB(){
	state=PCB::READY;
	quantum = 0;
	id=-1;

	stack=0;

	sp=0;
	ss=0;
	bp=0;
}

PCB::PCB(unsigned stackSize, int timeSlice, void (*body)()){

		state=PCB::NEW;
		quantum=timeSlice;
		id=++ajdi;

		asm cli;

		stack = new unsigned [stackSize];
		stack[stackSize-1] = 0x200;

		stack[stackSize-2] = FP_SEG(body);
		stack[stackSize-3] = FP_OFF(body);

		sp = FP_OFF(stack+1012);
		ss = FP_SEG(stack+1012);
		bp = FP_OFF(stack+1012);

		asm sti;
}
