#include <stdio.h>
#include <pthread.h>
#include "uart.h"

//some variables are defined in uart.c
extern pthread_t tid1;
extern pthread_attr_t attr;
extern int count;//no of start sequences

int main(){	
	pthread_attr_init(&attr);
		
	pthread_create(&tid1, &attr, sender, NULL);		
	pthread_join(tid1,NULL);
			
	printf("\ncount of start sequence is %d\n", count);
	return 0;
}
