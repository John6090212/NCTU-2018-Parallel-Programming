#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int corenum;
long long tossnum;
long long num_in_circle = 0;
long thread_count;
pthread_mutex_t mutex;
//tosstimes in every thread
long long tosstimes;
//div = RAND_MAX / (max - min)
double divisor = RAND_MAX / 2;

void* monte_carlo(void* rank);
int main(int argc,char* argv[]){
	
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
		//to use random seed 
		unsigned int seed = time(NULL);
		for(t = 0; t < times; t++){
			x = -1 + ((double)rand_r(&seed) / divisor);
			y = -1 + ((double)rand_r(&seed) / divisor);
			distance_squared = x * x + y * y;
			if(distance_squared <= 1){	
				partialsum++;	
			}	
		} 
		pthread_mutex_lock(&mutex);
		num_in_circle += partialsum;
		pthread_mutex_unlock(&mutex);
		
		return NULL;
	}
	//save input parameters in command line
	corenum = atoi(argv[1]);
	tossnum = atoll(argv[2]);
	
	//use pthread
	long thread;
	pthread_t* thread_handles;
	thread_count = corenum;
	thread_handles = (pthread_t*)malloc(thread_count * sizeof(pthread_t));
	pthread_mutex_init(&mutex,NULL);
	
	num_in_circle = 0;
	tosstimes = tossnum / thread_count;
	//pthread create
	for(thread = 0; thread < thread_count; thread++)
		pthread_create(&thread_handles[thread],NULL,monte_carlo,(void*)thread);
	//pthread join
	for(thread = 0; thread < thread_count; thread++)
		pthread_join(thread_handles[thread],NULL);
	//compute pi
	double pi_estimate = (4 * (double)num_in_circle) / ((double) tossnum);
	printf("pi = %lf\n", pi_estimate);
	
	pthread_mutex_destroy(&mutex);
	free(thread_handles);
	return 0;
} 
