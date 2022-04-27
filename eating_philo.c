//pickup(), putdown(), test(): 辛愷庭
// main function: 江宥潔
// run(): 易頡
// refactoring, research: 戴陽

#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

int MAX_EAT = 200;
int num = 5;
double avg_waiting_time[5];
double max_waiting_time[5];
enum
{
    thinking,
    hungry,
    eating
} state[5];
pthread_mutex_t lock;
pthread_cond_t self[5];
pthread_t philo[5];
int identity[5] = {0, 1, 2, 3, 4}; // anything like identity = [i for i in range(num)]??
int e_count = 0;

void *run_phi(void *arg);
void *pickup(int);
void *putdown(int);
void *test(int);

int main()
{
    pthread_mutex_init(&lock, NULL);
    for (int i = 0; i < num; ++i)
    {
        state[i] = thinking;
        pthread_cond_init(&self[i], NULL);
        avg_waiting_time[i] = 0;
    }
    for (int i = 0; i < num; ++i)
    {
        pthread_create(&philo[i], NULL, run_phi, &identity[i]);
    }
    for (int i = 0; i < num; ++i)
    {
        pthread_join(philo[i], NULL);
        printf("No. %d finished.\n", i);
    }
    printf("No. is hungry for...\n");
    for (int i = 0; i < num; i++)
    {
        printf("Np. %d : %f, max waiting time : %f\n", i, avg_waiting_time[i], max_waiting_time[i]);
    }
    return 0;
}
void *run_phi(void *arg)
{
    int *tmp = (int *)arg;
    int no = *tmp;
    double total_waiting_time = 0, single_waiting_time = 0;
    time_t t_start, t_end, t;
    printf("Philosopher %d is now working\n", no);
    srand((unsigned)time(&t));

    for (int i = 0; i < MAX_EAT; ++i)
    {
        printf("eating count: %d\n", e_count);

        int some_time = (rand()) % 3 + 1;
        sleep(some_time);
        printf("No. %d slept for %d seconds.\n", no, some_time);
        time(&t_start);
        pickup(no);
        time(&t_end);
        single_waiting_time = difftime(t_end, t_start);
        if (single_waiting_time > max_waiting_time[no]) {   //update max waiting time
            max_waiting_time[no] = single_waiting_time;
        }
        printf("No. %d has been hungry for %f seconds\n", no, single_waiting_time);

        sleep(some_time);
        printf("No. %d ate for %d seconds; %d times eating.\n", no, some_time, i + 1);
        putdown(no);
        ++e_count;

        total_waiting_time += single_waiting_time;
    }
    avg_waiting_time[no] = total_waiting_time / (double)MAX_EAT;
    return NULL;
}

// void *eat(int no){printf("eating...")};

void *pickup(int no)
{
    pthread_mutex_lock(&lock);
    state[no] = hungry;
    pthread_mutex_unlock(&lock);
    test(no); // merge 2 critical sections together?
    pthread_mutex_lock(&lock);
    if (state[no] != eating)
        pthread_cond_wait(&self[no], &lock);
    pthread_mutex_unlock(&lock);
    return NULL;
}

void *putdown(int no)
{
    pthread_mutex_lock(&lock);
    state[no] = thinking;
    pthread_mutex_unlock(&lock);
    test((no + 4) % 5);
    test((no + 1) % 5);
}

void *test(int no)
{
    int rhs = (no + 1) % 5, lhs = (no + 4) % 5;

    if (no % 2)
    {
        pthread_mutex_lock(&lock);
        if (state[rhs] != eating && state[lhs] != eating && state[no] == hungry)
        {
            state[no] = eating;
            pthread_cond_signal(&self[no]);
        }
        pthread_mutex_unlock(&lock);
    }
    else
    {
        pthread_mutex_lock(&lock);
        if (state[rhs] != eating && state[lhs] != eating && state[no] == hungry)
        {
            state[no] = eating;
            pthread_cond_signal(&self[no]);
        }
        pthread_mutex_unlock(&lock);
    }
}