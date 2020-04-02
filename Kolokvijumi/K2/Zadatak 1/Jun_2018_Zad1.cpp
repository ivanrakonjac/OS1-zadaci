class BoundedBuffer
{

private:
	Mutex* mutex;
	int intHead,intTail,
	int doubleHead,doubleTail;
	int kapacitet;

	int* intBuffer;
	double* doubleBuffer;

	Semaphore* intPun,intPrazan,doublePun,doublePrazan;

public:
	BoundedBuffer();
	~BoundedBuffer();

	void put1(int);
	void put2(double);
	int get (int*, double);
	
};

BoundedBuffer::BoundedBuffer(int kapacitet){

	this->kapacitet=kapacitet;

	intBuffer = new int[kapacitet];
	doubleBuffer = new double[kapacitet];

	mutex = new Mutex(1);

	intHead=intTail=doubleHead=doubleTail=0;

	intPun=new Semaphore(0);
	intPrazan=new Semaphore(0);
	doublePun=new Semaphore(0);
	doublePrazan=new Semaphore(0);
}

void BoundedBuffer::put1(int val){
	intPun.wait(); //ako je buffer pun => cekaj
	mutex.wait();
	intBuffer[intTail]=val;
	intTail=(intTail+1)%kapacitet;
	mutex.signal();
	intPrazan.signal(); //ako neko ceka jer je bio prazan


void BoundedBuffer::put2(double val){
	doublePun.wait(); 
	mutex.wait();
	doubleBuffer[doubleTail]=val;
	doubleTail=(doubleTail+1)%kapacitet;
	mutex.signal();
	doublePrazan.signal();
}

int BoundedBuffer::get(int* pi,double* pd){
	int t=Semaphore::waitOr(intPrazan,doublePrazan);
	mutex.wait();
	if(t==1){
		*pi = intBuffer[intHead];
		intHead=(intHead+1)%N;
		mutex.signal();
		intPun.signal(); //ako neko ceka jer nije bilo mesta
	}
	else{
		*pd=doubleBuffer[doubleHead];
		doubleHead=(doubleHead+1)%N;
		mutex.signal();
		doublePun.signal(); //ako neko  ceka jer nije bilo mesta
	}

	return s;
}