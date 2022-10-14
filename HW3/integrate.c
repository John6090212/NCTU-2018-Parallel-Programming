#include <stdio.h>
#include <math.h>
#include "mpi.h"

#define PI 3.1415926535

int main(int argc, char **argv) 
{
	MPI_Init(&argc,&argv);
    int rank,processnum;
	double time0;
    
    
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&processnum);
    
    //time0 = MPI_Wtime();
    
    long long i, num_intervals;
    double rect_width, area, sum, x_middle; 

    sscanf(argv[1],"%llu",&num_intervals);

    rect_width = PI / num_intervals;
    
    long long local_start,local_end;
    long long workload = num_intervals / processnum;
    long long remain = num_intervals % processnum;
    
    if(rank == 0){
    	local_start = 1;
    	local_end = local_start + workload + remain;
	}
	else{
		local_start = 1 + rank * workload + remain;
		local_end = local_start + workload - 1;
	}

    sum = 0;
    for(i = local_start; i <= local_end; i++) {

      /* find the middle of the interval on the X-axis. */ 

        x_middle = (i - 0.5) * rect_width;
    	area = sin(x_middle) * rect_width; 
    	sum = sum + area;
    } 
	
	double result_sum;
	MPI_Reduce(&sum,&result_sum,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
	
	
	if(rank == 0){
		printf("The total area is: %f\n", (float)result_sum);
		//printf("Execution time is : %f\n",MPI_Wtime()-time0);
	}
    
    
	MPI_Finalize();
	
    return 0;
}
