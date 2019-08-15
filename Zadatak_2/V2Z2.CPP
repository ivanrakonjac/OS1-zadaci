#include <iostream.h>
#include <dos.h>

#define lock asm cli

#define unlock asm sti

typedef void interrupt (*pInterrupt)(...);

pInterrupt oldISR;

volatile int cntr=100;

void interrupt timer(...){
	if(--cntr==0){
		cntr=100;
		cout<<"Istekao je timer aman"<<endl;
	}
}

unsigned int rut=0x1C;

void inic(){
	lock
	oldISR=getvect(rut);
	setvect(rut,timer);
	unlock
}

void restore(){
	lock
	setvect(rut,oldISR);
	unlock	
}

void doSomething(){
	for (int i = 0; i < 30; ++i)
	{
		lock
		cout<<i<<endl;
		unlock

		for (int j = 0; j < 30000; ++j)
		{
			for (int k = 0; k < 30000; ++k)
			{
			}
		}
	}
}


int main(){
	
	inic();
	doSomething();
	restore();

	return 0;
}