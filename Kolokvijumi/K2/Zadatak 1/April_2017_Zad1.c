struct ProcessorPrivate
{
	int ctn=0;

	disable_interrupts();
	enable_interrupts();
	test_and_set(short* lck);
	this_prcessor()
};

ProcessorPrivate processorPrivate[numOfProcessor];
short lck=0;

void lock(short* lck){
	int id = this->this_prcessor();
	ProcessorPrivate* pp = processorPrivate[id];

	if(pp->cnt==0){ //ako jos nije usao u krit sekciju
		pp->disable_interrupts();
		pp->cnt--;
		while(pp->test_and_set(&lck));
	}else{
		pp->cnt--;
	}
}

void unlock(short* lck){
	int id = this->this_prcessor();
	ProcessorPrivate* pp = processorPrivate[id];

	if(pp->cnt==0){//ako nije ni usao u krit sekciju a zove ulock
		return;
	}
	else if(pp->cnt<0){
		pp->cnt++;
		if(pp->cnt==0){
			lck=0;
			pp->disable_interrupts();
		}
	}
}