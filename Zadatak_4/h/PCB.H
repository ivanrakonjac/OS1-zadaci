#ifndef _PCB_H_
#define _PCB_H_

#include <iostream.h>
#include <dos.h>

class PCB{

public:

	static int ajdi;
	static PCB* runnig;

	enum State { READY, RUNNING, BLOCKED, FINISHED, NEW };

	PCB();
	PCB(unsigned stackSize, int timeSlice, void (*body)());


private:

	friend void interrupt timer(...);
	friend void exitThread();
	friend void f();

	unsigned bp;
	unsigned sp;
	unsigned ss;
	State state;
	unsigned* stack;
	int quantum;
	int id;

	friend int main();



};

#endif
