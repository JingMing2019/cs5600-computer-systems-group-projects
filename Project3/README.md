**Group Number 7:**  
Yingjie Huang, Jing Ming, Yulun Wang

**Description of project**  
This project is using semaphores and mutex locks in a multi-threaded C program, to prevent deadlock and starvation and manage the movement of customers between two floors in a department store. The program allows multiple customers to use the stairs efficiently and the current status and turnaround time of each customer will be displayed after execution.

**Project implementation**  
•  List the functions you wrote, and the purpose of each function  
**`void *threadfunction(void *vargp);`**  
The function is to represent the behavior of a customer thread in the program and manage customer interactions by checking and updating the direction of customers on the stairs, ensuring synchronization and preventing conflicts. It uses two semaphores(available_stairs_sem and is_stairs_empty_sem) and two mutex locks(release_semaphore_mutex and change_current_direction_mutex) to coordinate the movement of customers, including waiting for available spaces on the stairs and handling the completion of stair-crossing tasks. The function also records timing information for each customer to make sure each thread will sleep for a constant period.  
**`void cleanup();`**  
This function is to perform cleanup operations in our multi-thread program. It includes freeing dynamically allocated memory(tid), destroying mutexes used for synchronization, and destroying semaphores used for coordinating access to shared resources.  
**`int main(int argc, char *argv[])`**  
This function is the entry point of this program and it parses command-line arguments to obtain the number of customers(num_customers), the number of stairs(num_stairs) and a seed for random number generation. According to the arguments values, we could validate the inputs, initialze semaphores, allocate memory, create and join threads, calculate turnaround time and finally do the cleanup work.

• Explain how you tested your project and list the test cases you used.  
• Explain how you guarantee that your code is free of deadlock and starvation.  
   * The mutex is used to protect shared data and ensure that critical sections are accessed by only one thread at a time. It guards the critical sections where current direction is updated.

   * The sem semaphore is used to ensure mutual exclusion during the creation of threadFunction to prevent race conditions in thread creation.
It is also used to control access to critical sections where direction is updated, preventing multiple threads from simultaneously updating direction.

   * Deadlock Prevention:
Deadlock can occur when two or more threads are blocked indefinitely, waiting for each other. In this code, the use of pthread_mutex_lock and pthread_mutex_unlock around critical sections ensures that only one thread at a time can modify the shared count (on_stairs). Deadlock is prevented by releasing the mutex (pthread_mutex_unlock) before waiting on the semaphore (sem_wait).
   * Starvation Prevention:
Starvation occurs when a thread is perpetually denied access to the resource it needs. In this code: the semaphore (sem) is used to ensure mutual exclusion during thread creation, preventing a scenario where threads might be stuck in a race condition during creation.
Each customer thread checks if there are customers going in the opposite direction and waits if necessary. This prevents starvation by allowing fairness in using the stairs.
• Find the average Turnaround time of the examples you run, and explain using these performance measures how you adjusted your project to make your design “efficient”.
   * Average turnaround time for 4 customer and 6 stairs is 7s
  Average turnaround time for 3 customer and 6 stairs is 10s
  Average turnaround time for 2 customer and 10 stairs is 6.5s
  The use of a mutex (pthread_mutex_t mutex) and a semaphore (sem_t sem) is designed to minimize contention in critical sections. Contention occurs when multiple threads attempt to access shared resources simultaneously. The use of mutexes ensures that only one thread can update the shared direction at a time
• Explain how we can compile, run and test your code.  

• List the contributions of each student in your group
* Contribution:
    * Yingjie - Implementation of threadFunction
