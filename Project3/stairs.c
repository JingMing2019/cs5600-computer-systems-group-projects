#include "stairs.h"
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>


// You can write your own semwait function that can call sem_wait(sem) or sem_trywait(sem)
// in addition to checking/setting proper variables
// properly use pthread_mutex_lock/unlock
// int semwait(sem_t* sem) {
//     return sem_wait(sem);
// }

// You can write your own sempost function that call sem_post(sem)
// in addition to checking/setting proper variables
// properly use pthread_mutex_lock/unlock
// int sempost(sem_t* sem) {
//     return sem_post(sem);
// }

void *threadfunction (void *vargp) {
    // write the threadfunction whose name should be part of pthread_create(..)
	// Don't forget your timing computations
    thread_arg_t *t_arg = (thread_arg_t*)vargp;

    // Record the start time.
    gettimeofday(&t_arg->start_time, NULL);

    int id = t_arg->index;
    int direction = t_arg->direction;

    // Print customer going up or down.
    printf("%d\twants\t%s\n", id, (direction == 0) ? "up" : "down");

    // Compare current_direction of customers crossing the stairs now and
    // direction of new arrival customer. If they are different, wait for stairs
    // to be empty and later change current direction. If they share the same
    // direction, no need to change direction, simply go to next instruction.
    pthread_mutex_lock(&change_current_direction_mutex);
    if (current_direction != direction) {
        // If current_direction is not -1, means a set of customers are going
        // over the stairs on the opposite direction, new customer should wait
        // for them to finish.
        if (current_direction != -1) {
            printf("%d going %s should wait.\n", id,
                (direction == 0) ? "up" : "down");
        }
        // Wait for stairs to be empty. Once return means this new customer
        // can change the direction and start crossing the stairs.
        sem_wait(&is_stairs_empty_sem);

        current_direction = direction;
        printf("Crossing direction set to %s.\n",
            (direction == 0) ? "up" : "down");
    }
    pthread_mutex_unlock(&change_current_direction_mutex);

    // Wait for available spaces on stairs. Initially there are `num_stairs`
    // available spaces. If staris are serving `num_stairs` of customers, the
    // next customer should wait. Once return, means customer can cross the
    // stairs now.
    sem_wait(&available_stairs_sem);
    printf("%d\tgoing\t%s\n", id, (direction == 0) ? "up" : "down");

    // Simulate customer walking on stairs. Sleep for constant time.
    sleep(3);

    // Release semaphore.
    pthread_mutex_lock(&release_semaphore_mutex);
    // Increase available stairs.
    sem_post(&available_stairs_sem);
    // once return from sempost means customer has finished crossing the stairs.
    printf("%d\tdone\t%s\n", id, (direction == 0) ? "up" : "down");
    // Get current value of `available_stairs_sem`.
    int tmp_sem_value;
    sem_getvalue(&available_stairs_sem, &tmp_sem_value);
    // If value is the same as `num_stairs`, means customers previously going on
    // the stairs has finished their task. Increase the `is_stairs_empty_sem`,
    // wake up other thread that wait for stairs to be empty.
    // Reset `current_direction` to -1 indicates that no customers are on the
    // stairs now.
    if (tmp_sem_value == num_stairs) {
        sem_post(&is_stairs_empty_sem);
        printf("The stairs are vacant.\n");
        current_direction = -1;
    }
    pthread_mutex_unlock(&release_semaphore_mutex);

    // Record the end time.
    gettimeofday(&t_arg->end_time, NULL);

    pthread_exit(NULL);
}

// function to clean up thread, mutex and semaphore
void cleanup() {
    free(tid);
    pthread_mutex_destroy(&release_semaphore_mutex);
    pthread_mutex_destroy(&change_current_direction_mutex);
    sem_destroy(&available_stairs_sem);
    sem_destroy(&is_stairs_empty_sem);
}

int main(int argc, char *argv[]) {
    // Check if there contains enough input.
    if (argc != 4) {
        printf("Program needs 3 arguments. Number of customers, number of"
         "stairs and seed for random number generation.\n");
        exit(1);
    }

    // Get number of customers, number of stairs and seed from command-line
    // arguments.
    int num_customers = atoi(argv[1]);
    num_stairs = atoi(argv[2]);
    int seed = atoi(argv[3]);

    // Check input validation.
    if (num_customers > MAX_CUSTOMERS || num_customers < MIN_CUSTOMERS) {
        printf("Error Input %d. Number of customers should not exceed [%d, %d].", 
            num_customers, MIN_CUSTOMERS, MAX_CUSTOMERS);
        exit(1);
    }

    if (num_stairs > MAX_STAIRS || num_stairs < MIN_STAIRS) {
        printf("Error Input %d. Number of stairs should not exceed [%d, %d].", 
            num_stairs, MIN_STAIRS, MAX_STAIRS);
        exit(1);
    }
   
    printf("Number of Customers:\t%d\nNumber of Stairs:\t%d\n", num_customers, 
        num_stairs);

    // Initializes the semaphore. Set the pshared as 0 so that this semaphore is
    // shared between the threads of a process.
    // Set value as `num_stairs` means initially there are `num_stairs` empty
    // spaces for customers to use.
    sem_init(&available_stairs_sem, 0, num_stairs);
    // Set value as 1 indicates stairs are empty initially. This is a binary
    // counting semaphore. When it decrease to 0, other threads should wait for
    // it to increase. So when it is 0, means stairs are occupied by group of
    // customers going the same direction.
    sem_init(&is_stairs_empty_sem, 0, 1);

    // Allocate memory for each thread.
    tid = (pthread_t *)malloc(sizeof(pthread_t) * num_customers);

    if (tid == NULL) {
        printf("Memory allocation for thread failed.");
        exit(1);
    }

    // Define an array of thread arguments.
    thread_arg_t args[num_customers];
    // Seed the random number generator as current time (time(NULL)) or input
    // seed.
    srand(seed);

    // int direction[7] = {0, 0, 0, 1, 0, 0, 0};
    // int direction[7] = {0, 0, 0, 1, 0, 1, 0};

    // generate an array of threads, set their direction randomly, call pthread_create,
    // then sleep for some random nonzero time
    for (int i = 0; i < num_customers; i++) {
        // Set thread index as i.
        args[i].index = i;
        // Set direction as random 0 or 1.
        args[i].direction = rand() % 2;
        // args[i].direction = direction[i];
        // args[i].direction = 1;
        if (pthread_create(&tid[i], NULL, threadfunction, (void *)&args[i])) {
            printf("Error occurs in thread creation.");
            exit(1);
        }
        // Sleep for random 1 to 3 seconds.
        sleep(rand() % 3 + 1);
        // sleep(1);
    }

    // for each thread created, call pthread_join(..), wait for each thread
    // to terminate.
    for (int i = 0; i < num_customers; i++) {
        pthread_join(tid[i], NULL);
    }

    // printf turnaround time for each thread and average turnaround time
    double turnaround[num_customers];
    // Initialize the sum of turnaround as 0.
    double sum_turnaround = 0;

    for (int i = 0; i < num_customers; i++) {
        turnaround[i] = (args[i].end_time.tv_sec - args[i].start_time.tv_sec) +
         (args[i].end_time.tv_usec - args[i].start_time.tv_usec) * 1e-6;
        printf("Customer %d turnaround time is %.4f seconds.\n", i, 
            turnaround[i]);
        sum_turnaround += turnaround[i];
    }

    double avg_turnaround = sum_turnaround / num_customers;
    printf("Average turnaround time is %.4f seconds.\n", avg_turnaround);

    // free every pointer you used malloc for
    cleanup();

    return 0;
}
