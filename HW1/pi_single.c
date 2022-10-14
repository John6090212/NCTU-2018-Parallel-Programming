#include <stdio.h>
#include <stdlib.h>
#include <time.h>



int main(int argc,char* argv[]){
	long long num_in_circle = 0;
	long long haha = atoi(argv[1]);
	long long tossnum = atoi(argv[2]);
	//to use random seed 
	srand(time(NULL));
	
	double range = 2;
	double divisor = RAND_MAX / range;
	double x,y,distance_squared;
	num_in_circle = 0;
	long long i;
	unsigned int seed = time(NULL);
	for(i = 0; i < tossnum; i++){
		x = -1 + ((double)rand_r(&seed) / divisor);
		y = -1 + ((double)rand_r(&seed) / divisor);
		distance_squared = x * x + y * y;
		if(distance_squared <= 1){
			num_in_circle++;
		}
	}
	//compute pi
	double pi_estimate = (4 * (double)num_in_circle) / ((double) tossnum);
	printf("pi = %lf\n", pi_estimate);
	
	return 0;
} 
