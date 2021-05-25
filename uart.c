#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "uart.h"

pthread_t tid1, tid2;
pthread_attr_t attr;//common basic attributes
int count=0;//no of startsequences

uint8_t* senderFIFO;
bool finalFlag=false, receiveCompleteFlag=false, prevProcessCompleteFlag=true;

void transmitBufferFull(void){
	while(!prevProcessCompleteFlag); //you wait until the tid2 thread completes
	pthread_create(&tid2, &attr, processing, NULL);
}

void* sender(void* param){
	FILE* fp = fopen("test.csv", "r");
	if(!fp) printf("file cannot be opened\n");
	else {
		char buffer[100];
		uint8_t length = 16, i;

		senderFIFO = (uint8_t*) calloc(16, sizeof(uint8_t));
		if(senderFIFO==NULL) printf("could not allocate heap\n");

		while( fgets(buffer, 100, fp) ){
			i=0;
			printf("\nThis is sender function of UART");
			printf("\nsenderFIFO:");
			senderFIFO[i++]=strtol(strtok(buffer, ","), NULL, 16);
			while(length){
				printf("%x ", senderFIFO[i-1]);
				length--;
				senderFIFO[i++]=strtol(strtok(NULL, ","), NULL, 16);
			}
			finalFlag=false;
			length=16;
			printf("\n");

			transmitBufferFull();

			while(!receiveCompleteFlag);//wait until the processing func makes a local copy
			receiveCompleteFlag=false;//for next transmission
		}
	}

	while(!finalFlag);
	free(senderFIFO);
	pthread_exit(0);
}

void receiveComplete(void){
	receiveCompleteFlag = true;
}

void* processing(void* param){
	prevProcessCompleteFlag=false;
	pthread_detach(pthread_self());//detach itself from calling thread which is transmitBufferFull() ISR
	printf("\nThis is processing function of UART");
	uint8_t* receiverFIFO = (uint8_t*) malloc(sizeof(uint8_t)*16);
	memcpy(receiverFIFO, senderFIFO, 16);
	//call receiveComplete interrupt
	receiveComplete();
	uint8_t i=0;
	printf("\nrecevierFIFO:");
	for(; i<16; i++){
		if(i!=15){
			if(receiverFIFO[i]==0x5a && receiverFIFO[i+1]==0xa5) count++;
		}
		printf("%x ", receiverFIFO[i]);
	}
	prevProcessCompleteFlag=true;
	finalFlag=true;
	free(receiverFIFO);
	pthread_exit(0);
}




