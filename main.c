#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void* sender(void* param);
void interruptServiceRoutine(void);//whenever the 16 byte buffer is full this ISR will be generated
void* processing(void* param);

pthread_t tid1, tid2;
pthread_attr_t attr;

unsigned char* senderFIFO;
int count=0; //no of 0xA55A
int bufferFull=0;
bool flag=false;

int main(){	
	
	senderFIFO = (unsigned char*) calloc(16, sizeof(unsigned char));
	pthread_attr_init(&attr);
	
	pthread_create(&tid1, &attr, sender, NULL);		
	pthread_join(tid1,NULL);
		
	printf("count of start sequence is %d\n", count);
	return 0;
}

void interruptServiceRoutine(void){
	if(flag){
		pthread_create(&tid2, &attr, processing, NULL);
	}
	else{
		while(!flag); //you wait until the tid2 thread completes
		pthread_create(&tid2, &attr, processing, NULL);
	}
	printf("not waiting for the tid2 to join back and returning\n");
}

void* sender(void* param){
	printf("This is sender function of UART\n");
	senderFIFO[0]=0xA5;
	senderFIFO[1]=0x5A;
	bufferFull=bufferFull+2;
	int i = 0;
	for(i=2; i<16; i++){
		senderFIFO[i]=i;
		bufferFull++;
	}
	//generate an interrupt whenever we fully fill it
	if(bufferFull==16){
		bufferFull=0;
		interruptServiceRoutine();
	}

	printf("returned from the ISR\n");
	while(!flag);
	pthread_exit(0);
}

void* processing(void* param){
	int val;
	val=pthread_detach(pthread_self());//detach itself from calling thread
	printf("This is processing function of UART and the detach state is %d\n", val);
	
	int i=0;
	for(;i<16;i++){
		if(i!=15){
			if(senderFIFO[i]==0xA5 && senderFIFO[i+1]==0x5A) count++;
		}
		printf("processing senderFIFO[%d]:%d\n", i, senderFIFO[i]);
		senderFIFO[i]=0;
	}
	
	flag=true;
	pthread_exit(0);
}



