#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void* sender(void* param);
void* processing(void* param);
void interruptServiceRoutine(void);//whenever the 16 byte buffer is full this ISR will be generated

pthread_t tid1, tid2;
pthread_attr_t attr;

unsigned char* ptrSharedMem;
int count=0; //no of 0xA55A
int bufferFull=0;
bool flag=false;

int main(){	
	
	ptrSharedMem = (unsigned char*) calloc(16, sizeof(unsigned char));
	pthread_attr_init(&attr);
	
	pthread_create(&tid1, &attr, sender, NULL);		
	pthread_join(tid1,NULL);
	printf("count of start sequence is %d\n", count);
	return 0;
}

void interruptServiceRoutine(void){
	pthread_create(&tid2, &attr, processing, NULL);
	pthread_join(tid2,NULL);
	flag=true;
}

void* sender(void* param){
	printf("This is sender function of UART\n");
	ptrSharedMem[0]=0xA5;
	ptrSharedMem[1]=0x5A;
	bufferFull=bufferFull+2;
	int i = 0;
	for(i=2; i<16; i++){
		ptrSharedMem[i]=i;
		bufferFull++;
	}
	//generate an interrupt whenever we fully fill it
	if(bufferFull==16){
		bufferFull=0;
		interruptServiceRoutine();
	}
	while(!flag);
	pthread_exit(0);
}

void* processing(void* param){
	printf("This is processing function of UART\n");
	int i=0;
	for(;i<16;i++){
		if(i!=15){
			if(ptrSharedMem[i]==0xA5 && ptrSharedMem[i+1]==0x5A) count++;
		}
		printf("processing ptrSharedMem[%d]:%d\n", i, ptrSharedMem[i]);
		ptrSharedMem[i]=0;
	}
	pthread_exit(0);
}



