//  v2_zad1.cpp
//  prevodjenje iz komandne linije: bcc -mh -Ic:\bc31\include -Lc:\bc31\lib v2_zad1.cpp

//////////////////////////////
//program broji pritiske tastera i ispisuje ih nakon nekog vremena
//////////////////////////////

#include <stdio.h>
#include <dos.h>

typedef void interrupt (*pInterrupt)(...); //pok na prekidnu rutinu koja vraca void i prima (...) param
pInterrupt oldISR;

volatile int cntr = 0; // volatile jer se moze menjati asinhrono, tj. van izvrsavanja glavnog programa

// prekidna rutina koja broji pritiske tastera;
// interrupt kljucna rec oznacava da se radi o prekidnoj rutini
// (cuva na steku i restaurira sve programski dostupne registre - AX, ..., DI
// i na kraju poziva IRET)
void interrupt keyboardISR(...){
	asm {
		in al, 60h			// dohvati Scan Code
	}
	++cntr;				// uvecaj broj pritisaka

	// protokol zavrsetak obrade prekida;
	// obavestava se periferija i kontroler prekida
	// da je prekid obradjen
	asm {
		in al, 61h			// slanje signala o pritisku
		or al, 10000000b		// kontroleru tastature
		out 61h, al			// bez modifikovanja
		and al, 01111111b		// ostalih bita
		out 61h, al			//

		mov al, 20h			// slanje signala EOI (End-Of-Interrupt)
		out 20h, al			// kontroleru prekida
	}
}
// stara prekidna rutina
unsigned oldKbdOFF, oldKbdSEG;

// postavlja novu prekidnu rutinu
// u Interrupt vektor tabeli
void inic(){
	asm cli;
	oldISR = getvect(9);     // getvect(br) vraca sta pise u IV tabeli na ulazu br
	setvect(9,keyboardISR);	 // setvect(br,adresa) postavlja adresu prekidne rutine na ulaz br IV tabele
	asm sti;
}

// restauira staru prekidnu rutinu
// u Interrupt vektor tabeli
void restore(){
	asm cli;
	setvect(9,oldISR);
	asm sti;
}

void doSomething(){
	for (int i = 0; i < 10; ++i) {
		printf("main %d\n", i);
		for (int j = 0; j< 30000; ++j)
			for (int k = 0; k < 30000; ++k);
	}

	printf("Tastatura je generisala %d dogadjaja!\n", cntr/2);
	// Napomena: periferija generise prekid pri pritiskanju
	// i otpustanju tastera
	//zato cntr delim sa 2
}

int main(){

	inic();

	doSomething();

	restore();

	return 0;
}
