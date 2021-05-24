#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <string.h>
#include <stdint.h>

void* sender(void* param);
void receiveBufferFull(void);
void* processing(void* param);
void transmitComplete(void);

pthread_t tid1, tid2;
pthread_attr_t attr;

uint8_t* senderFIFO;
int count=0;
bool flag=false, txFlag=false, processCompleteFlag=true;

int main(){	
	pthread_attr_init(&attr);
		
	pthread_create(&tid1, &attr, sender, NULL);		
	pthread_join(tid1,NULL);
			
	printf("count of start sequence is %d\n", count);
	free(senderFIFO);
	return 0;
}

void receiveBufferFull(void){
	while(!processCompleteFlag); //you wait until the tid2 thread completes
	pthread_create(&tid2, &attr, processing, NULL);
}

void* sender(void* param){
	FILE* fp = fopen("example.csv", "r");
	if(!fp) printf("file cannot be opened\n");
	else {
		char buffer[100];
		uint8_t length = 16, i;
		printf("This is sender function of UART\n");

		senderFIFO = (uint8_t*) calloc(16, sizeof(uint8_t));
		if(senderFIFO==NULL) printf("could not allocate heap\n");

		while( fgets(buffer, 100, fp) ){
			i=0;
			senderFIFO[i++]=strtol(strtok(buffer, ","), NULL, 16);
			while(length){
				printf("%x ", senderFIFO[i-1]);
				length--;
				senderFIFO[i++]=strtol(strtok(NULL, ","), NULL, 16);
			}
			flag=false;
			length=16;
			printf("\n");
			receiveBufferFull();
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
	pthread_detach(pthread_self());//detach itself from calling thread which is receiveBufferFull() ISR
	printf("This is processing function of UART\n");
	uint8_t* receiverFIFO = (uint8_t*) malloc(sizeof(uint8_t)*16);
	memcpy(receiverFIFO, senderFIFO, 16);
	//call transmitComplete interrupt
	transmitComplete();
	uint8_t i=0;
	for(; i<16; i++){
		if(i!=15){
			if(receiverFIFO[i]==0x5a && receiverFIFO[i+1]==0xa5) count++;
		}
		printf("processing receiverFIFO[%d]:%x\n", i, receiverFIFO[i]);
	}
	processCompleteFlag=true;
	flag=true;
	free(receiverFIFO);
	pthread_exit(0);
}



