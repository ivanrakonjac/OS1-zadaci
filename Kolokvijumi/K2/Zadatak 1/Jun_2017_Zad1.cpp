Thread* blockedOnBarrier=0; //ovo je dodato u klasu thread;

class Barrier
{
private:
	Thread* owner;
	int open; //open==1 otvorena; open==0 zatvorena

	static int L=0;

	void lock();
	void unlock();

protected:
	void block();
	void deblock();

public:
	Barrier(int open=1);

	void open();
	void close();
	void pass();
	
};

Barrier::Barrier(int open){
	this->owner=Thread::runnig;
	this->open=open;
}

void Barrier::pass(){
	if(Thread::runnig!=owner){
		return;
	}
	else{
		if(open==1){
			return;
		}
		else if(open==0){
			lock(&L);
			block();
			unlock(&L);
		}
	}
}

void Barrier::close(){
	open=0;
}

void Barrier::open(){
	lock(&L);
	open=1;
	deblock();
	unlock(&L);
}

void Barrier::block(){
	Thread::running->blockedOnBarrier=this;
	if(setjmp(Thread::running->context)==0){
		//Blokiranje
		Thread:running=Scheduler::get();
		longjmp(Thread::running->context,1);
	}else return;
}

void Barrier::deblock(){
	//Deblokiraj
	if(this->owner->blockedOnBarrier==this){
		this->owner->blockedOnBarrier=0;
		Scheduler::put(this->owner);
	}
}

void Barrier::lock(int* val){
	//dok je L==1, uposleno cekaj
	while(test_and_set(*val));
}

void Barrier::unlock(int* val){
	*val=0;
}