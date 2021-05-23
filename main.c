#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void* sender(void* param);
void* receiver(void* param);
void* processing(void* param);

int main(){

	pthread_t tid1, tid2, tid3;
	pthread_attr_t attr;
	
	pthread_attr_init(&attr);
	
	pthread_create(&tid1,&attr,sender, NULL);
	pthread_create(&tid2, &attr, receiver, NULL);
	pthread_create(&tid3, &attr, processing, NULL);
		
	pthread_join(tid1,NULL);
	pthread_join(tid2, NULL);
	pthread_join(tid3, NULL);
	
	return 0;
}

void* sender(void* param){
	printf("This is sender function of UART\n");
	pthread_exit(0);
}

void* receiver(void* param){
	printf("This is receiver function of UART\n");
	pthread_exit(0);
}

void* processing(void* param){
	printf("This is processing function of UART\n");
	pthread_exit(0);
}


