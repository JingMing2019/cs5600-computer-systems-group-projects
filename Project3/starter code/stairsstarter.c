#include "stairs.h"





// You can write your own semwait function that can call sem_wait(sem) or sem_trywait(sem)
// in addition to checking/setting proper variables
// properly use pthread_mutex_lock/unlock
 

// You can write your own sempost function that call sem_post(sem)
// in addition to checking/setting proper variables
// properly use pthread_mutex_lock/unlock

 

void *threadfunction (void *vargp) {
    // write the threadfunction whose name should be part of pthread_create(..)
	// Don't forget your timing computations

    

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    

    //printf("Number of Customers: %d\nNumber of stairs: %d\n", ...., .....);
    

  
    // sem_init(.....);
	// generate an array of threads, set their direction randomly, call pthread_create, 
	// then sleep for some random nonzero time

  // your code here

	// for each thread created, call pthread_join(..)
    



   // printf turnaround time for each thread and average turnaround time
    
  // free every pointer you used malloc for

 
    return 0;
}
