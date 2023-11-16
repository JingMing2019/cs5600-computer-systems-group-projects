# Group Number 7 
Yingjie Huang, Jing Ming, Yulun Wang

# Description of project
This project is using semaphores and mutex locks in a multi-threaded C program, to prevent deadlock and starvation and manage the movement of customers between two floors in a department store. The program allows multiple customers to use the stairs efficiently and the current status and turnaround time of each customer will be displayed after execution.

# Project implementation 
## List the functions you wrote, and the purpose of each function  
1. `void *threadfunction(void *vargp);`  
The function is to represent the behavior of a customer thread in the program and manage customer interactions by checking and updating the direction of customers on the stairs, ensuring synchronization and preventing conflicts. It uses two semaphores(available_stairs_sem and is_stairs_empty_sem) and two mutex locks(release_semaphore_mutex and change_current_direction_mutex) to coordinate the movement of customers, including waiting for available spaces on the stairs and handling the completion of stair-crossing tasks. The function also records timing information for each customer to make sure each thread will sleep for a constant period.
2. `void cleanup();`
This function is to perform cleanup operations in our multi-thread program. It includes freeing dynamically allocated memory(tid), destroying mutexes used for synchronization, and destroying semaphores used for coordinating access to shared resources. 
3. `int main(int argc, char *argv[]);` 
This function is the entry point of this program and it parses command-line arguments to obtain the number of customers(num_customers), the number of stairs(num_stairs) and a seed for random number generation. According to the arguments values, we could validate the inputs, initialze semaphores, allocate memory, create and join threads, calculate turnaround time and finally do the cleanup work.

## Explain how you tested your project and list the test cases you used.
We tested our project by passing various configurations of `num_customers`, `num_stairs` and `seed` arguments to the main function to see if the function works well under different situations. Besides, edges cases such as invalid `num_customers` and `num_stairs` input have been tested as well. Since the `direction` of each customer/thread is randomly set by `rand()`, we also manually created some corner cases like all customers want to go down the stairs to test functionality. At last we tested cases that all customers come at the same time by comment the `sleep()` instruction after each thread creation.
**Test Cases**
1. Various Configurations of arguments (`num_customers`, `num_stairs`, `seed`)
  1. (3, 1, 100)
  2. (5, 2, 10)
  3. (6, 7, 9)
  4. (18, 10, 300)
  5. (30, 13, 999)
2. Edge cases ragarding the input
  1. not enough arguments
  2. `num_customers` = 31, `num_customers` = -1
  3. `num_stairs` = 100, `num_stairs` = 0
3. Manually set `direction`
  1. `direction` = {0, 0, 0, 1, 0, 0, 0}, `num_customers` = 7,  `num_stairs` = 2
  2. `direction` = {0, 0, 0, 1, 0, 0, 0}, `num_customers` = 7,  `num_stairs` = 13
  3. `direction` = {0, 0, 0, 1, 0, 1, 0}, `num_customers` = 7,  `num_stairs` = 3
  4. `direction` = {0, 0, 0, 1, 0, 1, 0}, `num_customers` = 7,  `num_stairs` = 12
  5. `direction` = {0, 0, 0, 0, 0, 0}, `num_customers` = 6,  `num_stairs` = 2
  6. `direction` = {0, 0, 0, 0, 0, 0}, `num_customers` = 6,  `num_stairs` = 13
4. Customers came at the same time, comment `sleep()`
  1. `num_customers` = 8,  `num_stairs` = 5, `seed` = 99
  2. `direction` = {0, 0, 0, 1, 0, 0, 0}, `num_customers` = 7,  `num_stairs` = 2
  3. `direction` = {0, 0, 0, 1, 0, 1, 0}, `num_customers` = 7,  `num_stairs` = 3
5. Use `valgrind` to see if the process creates memory leak or other problems.

## Explain how you guarantee that your code is free of deadlock and starvation.
1. The mutex is used to protect shared data and ensure that critical sections are accessed by only one thread at a time. It guards the critical sections where current direction is updated.
2. The sem semaphore is used to ensure mutual exclusion during the creation of threadFunction to prevent race conditions in thread creation. It is also used to control access to critical sections where direction is updated, preventing multiple threads from simultaneously updating direction.
3. Deadlock Prevention:
Deadlock can occur when two or more threads are blocked indefinitely, waiting for each other. In this code, the use of pthread_mutex_lock and pthread_mutex_unlock around critical sections ensures that only one thread at a time can modify the shared count (on_stairs). Deadlock is prevented by releasing the mutex (pthread_mutex_unlock) before waiting on the semaphore (sem_wait).
4. Starvation Prevention:
Starvation occurs when a thread is perpetually denied access to the resource it needs. In this code: the semaphore (sem) is used to ensure mutual exclusion during thread creation, preventing a scenario where threads might be stuck in a race condition during creation.
Each customer thread checks if there are customers going in the opposite direction and waits if necessary. This prevents starvation by allowing fairness in using the stairs.

## Find the average Turnaround time of the examples you run, and explain using these performance measures how you adjusted your project to make your design “efficient”.
**Average Turnaround Time of Test Cases**
1. Average turnaround time for 3 customers and 1 stair is 3.0012 seconds.
2. Average turnaround time for 5 customers and 2 stairs is 3.2017 seconds.
3. Average turnaround time for 6 customers and 7 stairs is 3.1689 seconds.
4. Average turnaround time for 18 customers and 10 stairs is 3.3924 seconds.
5. Average turnaround time for 30 customers and 13 stairs is 7.3306 seconds.
From first 4 results we can see that the average turnaround time is around 3 seconds. Remind that in our program each thread will sleep constant 3 seconds after crossing the stairs to simulate the going over stairs action. The experimented average turnaround time is close to this sleep time. This indicates that each thread does not wait for a long time to step on stairs after they've arrived, which is an evidence to prove that our program is efficient. Though the fifth test case shows that the average turnaround time is 7 seconds, which is much longer than the 3 seconds. This is because we sleep random seconds between the creation of each threads, say 1 to 3 seconds. This very short sleep time makes later threads came as early as many previous threads have not finished their tasks which increases the turnaround time for later threads. If we make this sleep time longer, more threads will finish crossing stairs before the arrival of later threads. Hence it can shorten the turnaround time of later threads and in turn shorten the average turnaround time.
6. Average turnaround time for 7 customers (only the forth customer is going down while the other customers are going up) and 2 stairs is 3.8575 seconds.
7. Average turnaround time for 7 customers (the forth and sixth customers are going down while the other customers are going up) and 2 stairs is 4.7161 seconds.
Comapre above 6 and 7 test cases, we can see that case 7 takes longer turnaround time. This is because the strategy of our program is if a thread with the opposite direciton (different than the direction that stairs are currently serving) arrives, we hold all later threads, wait for previous threads to finish crossing and then change the current direction to let this thread go on the stairs. This strategy is designed to prevent starvation. Since case 7 need to do 1 more direction change than case 6, it needs more time for later threads to wait.

## Explain how we can compile, run and test your code.
### COMPILE
Type `make stairs` in the terminal. If compiled successfully, a `stairs` executable file and `stairs.o` object file will appear under directory.
### RUN
Type `./stairs arg1 arg2 arg3 > output.txt` in the terminal to write the output to .txt file or `./stairs arg1 arg2 arg3` to show the output directly in the terminal.
### TEST
Using test cases wrote above. For example, test case **1. (3, 1, 100)**, just type `./stairs 3 1 100` in the terminal. Then a test case with 3 customers and 1 staris will running and output logs and average turnaround time.
Besides, if you want test the code with pre-defined `direction`. Go to line 159 under `stairs.c`, uncomment this line and set any number and any length as you wish. Then go to line 169 under the same file. Uncommet this line and comment the line before. Save the file and compile. After which run `./stairs length_of_direction num_stairs any_number`. Here `length_of_direction` is the length of `direciton` at line 169. `num_stairs` is number of stairs you want to test. Since we have manually set `direction`, you can input any number for the third argument `seed`.  

## List the contributions of each student in your group.
1. Yingjie - Implementation of `threadfunction`.
2. Jing - Implementation of `main` function.
3. Yulun - Implementation of `cleanup` function and semaphore design.
