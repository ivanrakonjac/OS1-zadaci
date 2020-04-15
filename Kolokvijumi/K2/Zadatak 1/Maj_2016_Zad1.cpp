//K2 Maj 2016.
//Zad2

class Mutex{
private:
	int val=1;
	int L=0;

	Thread* owner=null;
	Queue<Thread*> waitingQueue = new WaitingQueue();

	void lock(int*);
	void unlock(int*);

protected:
	void block();
	void deblock();

public:
	int wait();
	int signal();
}

int Mutex::wait(){
	lock(&L);
	if(val==1){
		owner=Thread::running;
		val=0;
		unlock(&L);
		return 0;
	} else if(val==0){
		unlock(&L);
		
		block();

		lock(&L);
		owner=Thread::running;
		val=0;
		unlock(&L);
		return 0;
	}
	
}

int Mutex::signal(){
	lock(&L);
	if(Thread::running==owner){
		val=1;
		deblock();
		unlock(&L);
		return 0;
	}
	else{
		unlock(&L);
		return -1;
	}
}

void Mutex::block(){
	
	if(setjmp(Thread::running->context)==0){
		//Blokiranje
		waitingQueue.add(Thread::running);
		Thread:running=Scheduler::get();
		longjmp(Thread::running->context,1);
	}
}

void Mutex::deblock(){
	//Deblokiraj
	Scheduler::put(waitingQueue.removeFirst());
}

void Mutex::lock(int* val){
	//dok je L==1, uposleno cekaj
	while(test_and_set(*val));
}

void Mutex::unlock(int* val){
	*val=0;
}