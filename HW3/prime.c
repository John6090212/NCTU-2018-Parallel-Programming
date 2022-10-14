#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

int isprime(long long int n) {
    long long int i,squareroot;
    if (n>10) {
        squareroot = (long long int) sqrt(n);
        for (i=3; i<=squareroot; i=i+2)
            if ((n%i)==0)
        		return 0;
        return 1;
    }
    else
    	return 0;
}

int main(int argc, char *argv[])
{
	MPI_Init(&argc,&argv);
    int rank,processnum;
    int tag = 0;
    int source;
    int dest = 0;
    MPI_Status status;
    long long int local_start;
    long long int local_end;
    double time0;
    
    
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&processnum);
    
    //time0 = MPI_Wtime();
  
    long long int pc,       /* prime counter */
        foundone; /* most recent prime found */
    long long int n, limit;

	long long int result[2];	

	sscanf(argv[1],"%llu",&limit);	
	if(rank == 0){
		printf("Starting. Numbers to be scanned= %lld\n",limit);
	}
	

	
	long long int totalload = limit - 11 + 1;
	long long int workload = totalload / processnum;
	long long int remain = totalload % processnum;
    
	     
  	
  	if(rank == 0){
  		local_start = 11;
		local_end = local_start + workload + remain - 1;
		//printf("rank %d local_start = %lld local_end = %lld\n",rank,local_start,local_end);
		pc = 4; /* Assume (2,3,5,7) are counted here */
	}
	else{
		local_start = 11 + rank * workload + remain;
		local_end = local_start + workload - 1;
		//printf("rank %d local_start = %lld local_end = %lld\n",rank,local_start,local_end);
		pc = 0;
	}
    
    if(local_start % 2 == 0){
    	local_start++;
	}
    	
    for (n=local_start; n<=local_end; n+=2) {
    	if (isprime(n)) {
      		pc++;
      		foundone = n;
    	}		
			
    }
    result[0] = pc;
    result[1] = foundone;
    //printf("rank %d pc = %lld foundone = %lld\n",rank,pc,foundone);
    long long int result_pc,result_foundone;
    if(rank == 0){
    	result_pc = pc;
    	result_foundone = foundone;
    	for(source = 1; source <= processnum - 1; source++){
    		MPI_Recv(result,2,MPI_LONG_LONG_INT,source,tag,MPI_COMM_WORLD,&status);
    		result_pc += result[0];
    		if(result[1] > result_foundone)
    			result_foundone = result[1];
		}
	}
	else{
		MPI_Send(result,2,MPI_LONG_LONG_INT,dest,tag,MPI_COMM_WORLD);
	}
	
	if(rank == 0){ 
		printf("Done. Largest prime is %d Total primes %d\n",result_foundone,result_pc);
		//printf("Execution time is : %f\n",MPI_Wtime()-time0);
	}
	
	MPI_Finalize();
	
    return 0;
} 
