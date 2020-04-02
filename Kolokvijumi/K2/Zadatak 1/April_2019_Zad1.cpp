class Mutex
{
private:
	int ownerID;
	sem_t* mutex;
public:
	Mutex(const char* name);
	~Mutex()
	int entry();
	int exit();
	
};

Mutex::Mutex(const char* name){
	mutex=sem_open(name,O_CREATE,1);
	ownerID=0;
}

Mutex::~Mutex(){
	sem_close(this->mutex);
}

int Mutex::entry(){//wait
	if(ownerID==0){
		ownerID=getpid(); //upisem koji je proces uzeo semafor
	}
	sem_wait(this->mutex); //odradim wait nad semaform
}

int Mutex::exit(){ //signal()
	if(ownerID!=0){
		if(ownerID==getpid()){
			sem_post(this->mutex);
			ownerID=0;
			return 0;
		}
		return-1;
		
	}
	return -1;
}

