Project: UART 16 byte FIFO and count the no of start sequences (0xa55a)

Author: Shweta Mane

Date: May 24, 2021

Machine: Linux Ubuntu (x86-64)

Compiler: gcc

Command: make clean; make all

CMD line encoding: UTF-8 (en-US)

Instructions: make clean->make all->./main.out are the commands needed to be ran.
              After that if you want to add personal test cases, add in the test.csv file but also append a comma in the end.

Files:

main.c: It creates the first sender thread and it waits on it to join back. When the sender thread completes all of its operation. We can display the updated value of the count variable.
The count variable stores the no of start sequences.

uart.c: Contains four functions
i)sender function
ii)transmitBufferFull ISR
iii)processing function
iv)receiveComplete ISR

uart.h: header file for packet.c with function prototypes.

test.csv: It has the test cases that will be read by the sender thread.
					
Assumptions and Information:
1) Every line in the test.csv need to be 16bytes long.
2) A comma must be appended at the end of each line in test.csv.
3) When the sender function fills the buffer, a processing thread is created
for it by the ISR. Then the ISR returns and the sender thread continue filling the buffer as the processing thread
works on the previous FIFO. Thus the threads work simultaneously.
4) The ISR only halts the sender thread and itself if the processing thread has not completed its previous operation yet. At a time only two or one thread can be active.


