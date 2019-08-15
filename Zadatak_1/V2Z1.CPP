//  v2_zad1.cpp
//  prevodjenje iz komandne linije: bcc -mh -Ic:\bc31\include -Lc:\bc31\lib v2_zad1.cpp

#include <stdio.h>

volatile int cntr = 0; // zasto volatile?

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
	asm{
		cli				// zabranjuju se prekidi
		push es
		push ax

		mov ax,0
		mov es,ax			// es = 0

		// pamti staru rutinu
		// oldKbdSEG = [00026h]; - 9*4h+2h=26h
		mov ax, word ptr es:0026h
		mov word ptr oldKbdSEG, ax
		// oldKbdOFF = [00024h]; - 9*4h=24h
		mov ax, word ptr es:0024h
		mov word ptr oldKbdOFF, ax

		// postavlja novu rutinu
		mov word ptr es:0026h, seg keyboardISR
		mov word ptr es:0024h, offset keyboardISR

		pop ax
		pop es
		sti				// dozvoljavaju se prekidi
	}
}

// restauira staru prekidnu rutinu
// u Interrupt vektor tabeli
void restore(){
	asm {
		cli
		push es
		push ax

		mov ax,0
		mov es,ax

		// vraca originalnu rutinu
		mov ax, word ptr oldKbdSEG
		mov word ptr es:0026h, ax
		mov ax, word ptr oldKbdOFF
		mov word ptr es:0024h, ax

		pop ax
		pop es
		sti
	}
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
}

int main(){

	inic();

	doSomething();

	restore();

	return 0;
}
