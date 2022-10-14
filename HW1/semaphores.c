#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>

int corenum;
long long tossnum;
long long num_in_circle = 0;
long thread_count;
sem_t* semaphores;
//tosstimes in every thread
long long tosstimes;
//div = RAND_MAX / (max - min)
double divisor = RAND_MAX / 2;

void* monte_carlo(void* rank);
int main(int argc,char* argv[]){
	//to use random seed 
	srand(time(NULL));
	//monte_carlo
	void* monte_carlo(void* rank){
		long long my_rank = (long long) rank;
		long long t;
		long long times = tosstimes;
		//generate random double between 1 and -1
		if(my_rank == thread_count-1){
			times = tossnum - (thread_count-1) * tosstimes;
		} 
		double x,y,distance_squared;
		long long partialsum = 0;
		for(t = 0; t < tosstimes; t++){
			x = -1 + (rand() / divisor);
			y = -1 + (rand() / divisor);
			distance_squared = x * x + y * y;
			if(distance_squared <= 1){	
				partialsum++;	
			}	
		} 
		sem_wait(&semaphores);
		num_in_circle += partialsum;
		sem_post(&semaphores);
		
		return NULL;
	}
	//save input parameters in command line
	corenum = atoi(argv[1]);
	tossnum = atoll(argv[2]);
	
	//use pthread
	long thread;
	pthread_t* thread_handles;
	thread_count = 10;
	thread_handles = (pthread_t*)malloc(thread_count * sizeof(pthread_t));
	semaphores = malloc(sizeof(sem_t));
	
	num_in_circle = 0;
	tosstimes = tossnum / thread_count;
	//pthread create
	sem_init(&semaphores,0,1);
	for(thread = 0; thread < thread_count; thread++)
		pthread_create(&thread_handles[thread],NULL,monte_carlo,(void*)thread);
	//pthread join
	for(thread = 0; thread < thread_count; thread++)
		pthread_join(thread_handles[thread],NULL);
	//compute pi
	double pi_estimate = (4 * (double)num_in_circle) / ((double) tossnum);
	printf("pi = %lf\n", pi_estimate);
	
	free(semaphores);
	free(thread_handles);
	return 0;
} 
