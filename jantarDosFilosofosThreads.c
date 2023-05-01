#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

#define N 				5
#define LEFT			(i + N - 1) % N
#define	RIGHT			(i + 1 ) % N
#define	THINKING		0
#define	HUNGRY			1
#define EATING			2
#define DELAY			3000000


int state[N];
sem_t mutex, s[N]; /* s[N] == semaforos dos filosofo, 1 para cada filosofo */

void think();
void eat();



void *philosopher(void *n){
	int i = (int)n; /* i == numero do filoso */
	while(1){
		think(i);
		take_forks(i);
		eat(i);
		put_forks(i);
		
	}
}

void take_forks(int i){
	sem_wait(&mutex);
	state[i] = HUNGRY;
	test(i);
	sem_post(&mutex);
	sem_wait(&s[i]);
	usleep(DELAY/2);
}

void put_forks(int i){
	sem_wait(&mutex);
	state[i] = THINKING;
	test(LEFT);
	test(RIGHT);
	sem_post(&mutex);
}

void test(int i){
	if(state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING){
		state[i] = EATING;
		usleep(DELAY);
		sem_post(&s[i]);
	}
}

int main(void){
	sem_init(&mutex, 0, 1);
	pthread_t num_thread[N]; /* array de filosofos(threads)*/
	
	for(int i = 0 ; i < 5 ; i++) sem_init(&s[i], 0, 0);
	for(int i = 0 ; i < 5 ; i++) pthread_create(&num_thread[i], 0, philosopher, (void*)i);
	for (int i = 0 ; i < 5 ; i++) pthread_join(&num_thread[i], 0);
	
	return 0;
}

void think(int i){
	printf("O filoso %d está pensando..\n", i);
	usleep(DELAY/2);
}

void eat(int i){
	printf("O filoso %d está comendo...\n", i);
	usleep(DELAY/2);
	
}




