const N = ...;
Semaphore*[N] semafori;

void process(){
	//ovde se nesto desava
}

void funkcija(int id){
	sem_wait(semafori[id]);
	process();
	if(id<N) sem_signal(semafori[id+1]);
}

int main(){
	
	//inicijalizacija
	for (int i = 0; i < N; ++i)
	{
		semafori[i]=sem_create(0);
	}
	semafori[0]=1; //da bi prvi mogao da prodje

	for (int i = 0; i < N; ++i)
	{
		int id = i;
		thread_create(funckija,i);
	}		
}
