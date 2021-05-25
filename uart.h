#include <pthread.h>

/**
 * @brief The sender function fills senderFIFO buffer and continue filling it if 
 * the previous senderFIFO is copied into local receiverFIFO buffer. 
 *
 * @param void pointer to an argument that is passed from the pthread_create()
 * function
 *
 * @return void pointer 
 */
void* sender(void* param);

/**
 * @brief whenever the first 16 bytes are written to the senderFIFO buffer
 * this ISR is triggered, the ISR is responsible for creating a processing 
 * thread (it creates a processing thread only when the previous 
 * processing thread has completed its operation). It polls the 
 * prevProcessCompleteFlag to check whether the previous processing operation
 * has been completed. Once created thread, it does not wait for it and returns to the sender thread
 * to continue with another transmission round. The sender thread and processing thread
 * work simultaneously.
 *
 * @param void
 * 
 * @return void
 */
void transmitBufferFull(void);

/**
 * @brief The receiver function consumes the senderFIFO into a local
 * receiverFIFO. It resets the prevProcessingCompleteFlag so that 
 * another processing thread is not created unless the operation is 
 * completed by this current thread. It calls the receiveComplete() ISR. 
 * It looks for start sequences (0xa55a) and increment the count variable accordingly. 
 * Finally it sets the finalFlag and prevProcessingCompleteFlag
 * to indicate that the processing of given 16 bytes of data has completed,
 * 
 * @param void pointer to an argument which is passed in the pthread_create()
 * 
 * @return void pointer
 */
void* processing(void* param); 


/**
 * @brief When the 16 bytes are transferred to the receiver/processing function 
 * It sets the receiveCompleteFlag to indicate the sender thread to fill another 16byte senderFIFO buffer. 
 * This way the sender and processing threads can work simultaneously.
 * 
 * @param void
 *
 * @return void
 */
void receiveComplete(void);
