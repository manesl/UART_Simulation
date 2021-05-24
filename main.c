#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void* sender(void* param);
void receiveBufferFull(void);
void* processing(void* param);
void transmitComplete(void);

pthread_t tid1, tid2;
pthread_attr_t attr;

unsigned char* senderFIFO;
int count=0; //no of 0xA55A
int bufferFull=0;
bool flag=false, txFlag=false, processCompleteFlag=true;

int main(){	
	
	senderFIFO = (unsigned char*) calloc(16, sizeof(unsigned char));
	pthread_attr_init(&attr);
	
	pthread_create(&tid1, &attr, sender, NULL);		
	pthread_join(tid1,NULL);
		
	printf("count of start sequence is %d\n", count);
	return 0;
}

void receiveBufferFull(void){
	if(processCompleteFlag){
		pthread_create(&tid2, &attr, processing, NULL);
	}
	else{
		while(!processCompleteFlag); //you wait until the tid2 thread completes
		pthread_create(&tid2, &attr, processing, NULL);
	}
	printf("not waiting for the tid2 to join back and returning\n");
}

void* sender(void* param){
	printf("This is sender function of UART\n");
	senderFIFO[0]=0xA5;
	senderFIFO[1]=0x5A;
	bufferFull=bufferFull+2;
	int i, j = 2;
	for(i=2; i<32; i++){
		senderFIFO[j]=0xFF;
		bufferFull++;
		j++;
		printf("senderFIFO[%d]:%d, bufferFull:%d, next j:%d\n", j-1, senderFIFO[j-1], bufferFull, j); 
		//generate an interrupt whenever we fully fill it
		if(bufferFull==16){
			bufferFull=0;
			receiveBufferFull();
			j=0;
			while(!txFlag);//wait until the processing func makes a local copy
			txFlag=false;//for next transmission
		}
	}

	while(!flag);
	pthread_exit(0);
}

void transmitComplete(void){
	txFlag = true;
}

void* processing(void* param){
	processCompleteFlag=false;
	pthread_detach(pthread_self());//detach itself from calling thread
	printf("This is processing function of UART\n");
	unsigned char* receiverFIFO = (unsigned char*) malloc(sizeof(unsigned char)*16);
	memcpy(receiverFIFO, senderFIFO, 16);
	//call transmitComplete interrupt
	transmitComplete();
	int i=0;
	for(;i<16;i++){
		if(i!=15){
			if(receiverFIFO[i]==0xA5 && receiverFIFO[i+1]==0x5A) count++;
		}
		printf("processing receiverFIFO[%d]:%d\n", i, receiverFIFO[i]);
	}
	processCompleteFlag=true;
	flag=true;
	free(receiverFIFO);
	pthread_exit(0);
}



