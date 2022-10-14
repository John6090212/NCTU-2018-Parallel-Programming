#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <fstream>
#include <iostream>
#include <CL/cl.h>
using namespace std;
/*
unsigned int * histogram(unsigned int *image_data, unsigned int _size) {

	unsigned int *img = image_data;
	unsigned int *ref_histogram_results;
	unsigned int *ptr;

	ref_histogram_results = (unsigned int *)malloc(256 * 3 * sizeof(unsigned int));
	ptr = ref_histogram_results;
	memset (ref_histogram_results, 0x0, 256 * 3 * sizeof(unsigned int));

	// histogram of R
	for (unsigned int i = 0; i < _size; i += 3)
	{
		unsigned int index = img[i];
		ptr[index]++;
	}

	// histogram of G
	ptr += 256;
	for (unsigned int i = 1; i < _size; i += 3)
	{
		unsigned int index = img[i];
		ptr[index]++;
	}

	// histogram of B
	ptr += 256;
	for (unsigned int i = 2; i < _size; i += 3)
	{
		unsigned int index = img[i];
		ptr[index]++;
	}

	return ref_histogram_results;
}
*/
int main(int argc, char const *argv[])
{
                      

    cl_device_id device_id;             
    cl_context context;                
    cl_command_queue commands;          
    cl_program program;                 
    cl_kernel kernel;                   
    //change to the output we need!!!!!!
    cl_mem inputdata;                       
    cl_mem outputdata;
    int err;
    
    err = clGetDeviceIDs(NULL,CL_DEVICE_TYPE_GPU,1,&device_id,NULL);
    //just for test
    if(err != CL_SUCCESS){
    	cout << "Error: Failed to create a device group!" << endl;
    	return EXIT_FAILURE;
	}
	
	context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
	//just for test
	if(!context){
        cout << "Error: Failed to create a compute context!" << endl;
        return EXIT_FAILURE;
    }
    
    commands = clCreateCommandQueueWithProperties(context, device_id, 0, &err);
    //just for test
    if(!commands){
        cout << "Error: Failed to create a command commands!" << endl;
        return EXIT_FAILURE;
    }
    
    FILE* programfile = fopen("histogram.cl","r");
    fseek(programfile,0,SEEK_END);
    size_t programsize = ftell(programfile);
    rewind(programfile);
    
    char* programbuffer = (char*)malloc(programsize + 1);
    programbuffer[programsize] = '\0';
    fread(programbuffer,sizeof(char),programsize,programfile);
    fclose(programfile);
    //test programfile
	/*
	printf("file is like below:\n");
	printf("%s\n",programbuffer);
	*/
    program = clCreateProgramWithSource(context, 1, (const char **) &programbuffer, NULL, &err);
    //just for test
    if(!program){
    	cout << "Error: Failed to create compute program!" << endl;
    	return EXIT_FAILURE;
	}
	free(programbuffer);
	
	err = clBuildProgram(program,0,NULL,NULL,NULL,NULL);
	//just for test
	/*
	if(err != CL_SUCCESS){
        size_t len;
        char buffer[2048];

        cout << "Error: Failed to build program executable! error num = " << err << endl;
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
        exit(1);
    }
    */
    if (err != CL_SUCCESS) {
		char *buff_erro;
		cl_int errcode;
		size_t build_log_len;
		errcode = clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &build_log_len);
		if (errcode) {
            printf("clGetProgramBuildInfo failed at line %d\n", __LINE__);
            exit(-1);
        }

    	buff_erro = (char*)malloc(build_log_len);
    	if (!buff_erro) {
        	printf("malloc failed at line %d\n", __LINE__);
        	exit(-2);
    }

    errcode = clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, build_log_len, buff_erro, NULL);
    if (errcode) {
        printf("clGetProgramBuildInfo failed at line %d\n", __LINE__);
        exit(-3);
    }

    fprintf(stderr,"Build log: \n%s\n", buff_erro); //Be careful with  the fprint
    free(buff_erro);
    fprintf(stderr,"clBuildProgram failed\n");
    exit(EXIT_FAILURE);
}
    
    kernel = clCreateKernel(program,"histogram",&err);
    //just for test
    if(!kernel || err != CL_SUCCESS){
    	cout << "Error: Failed to create compute kernel!" << endl;
    	exit(1);
	}
	
	unsigned int * histogram_results;
	unsigned int *histogram_index;
	unsigned int i=0, a, input_size;
	fstream inFile("input", ios_base::in);
	ofstream outFile("0516011.out", ios_base::out);

	inFile >> input_size;
	unsigned int *image = new unsigned int[input_size];
	while( inFile >> a ) {
		image[i++] = a;
	}
	
	inputdata = clCreateBuffer(context,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,sizeof(unsigned int)*input_size,image,NULL);
	//need to determine whether it is write only or not
	outputdata = clCreateBuffer(context,CL_MEM_WRITE_ONLY,sizeof(unsigned int)*input_size,NULL,NULL);
	//just for test
	if(!inputdata || !outputdata){
        cout << "Error: Failed to allocate device memory!" << endl;
        exit(1);
    } 
	/*
	err = clEnqueueWriteBuffer(commands, inputdata, CL_TRUE, 0, sizeof(unsigned int)*input_size, image, 0, NULL, NULL);
    if(err != CL_SUCCESS){
        cout << "Error: Failed to write to source array of inputdata!" << endl;
        exit(1);
    }
    
    err = clEnqueueWriteBuffer(commands, outputdata, CL_TRUE, 0, sizeof(unsigned int)*input_size, ref_histogram_results, 0, NULL, NULL);
    if(err != CL_SUCCESS){
        cout << "Error: Failed to write to source array of outputdata!" << endl;
        exit(1);
    }
    */
    err = 0;
    err = clSetKernelArg(kernel,0,sizeof(cl_mem),&inputdata);
	err = clSetKernelArg(kernel,1,sizeof(cl_mem),&outputdata); 
    //just for test
    if(err != CL_SUCCESS){
        cout << "Error: Failed to set kernel arguments! " << err << endl;
        exit(1);
    }
                       
    /*
    err = clGetKernelWorkGroupInfo(kernel, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
    //just for test
    if(err != CL_SUCCESS){
        cout << "Error: Failed to retrieve kernel work group info! "<< err << endl;
        exit(1);
    }
    */
 	size_t global = input_size; 
	err = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, 0, 0, NULL, NULL);
    if(err){
        cout << "Error: Failed to execute kernel! error message = " << err << endl;
        return EXIT_FAILURE;
    } 
    
	histogram_results = (unsigned int *)malloc(256 * 3 * sizeof(unsigned int));
	memset(histogram_results, 0x0, 256 * 3 * sizeof(unsigned int));
	histogram_index = (unsigned int *)malloc(sizeof(unsigned int)*input_size);
	
	clFinish(commands);
	
	err = clEnqueueReadBuffer(commands,outputdata,CL_TRUE,0,sizeof(unsigned int)*input_size,histogram_index,0,NULL,NULL);
	if(err != CL_SUCCESS){
        cout << "Error: Failed to read output array! " << err << endl;
        exit(1);
    }
    
    for(unsigned int i = 0; i < input_size; i++){
    	histogram_results[histogram_index[i]]++;
	}
    
	for(unsigned int i = 0; i < 256 * 3; ++i) {
		if (i % 256 == 0 && i != 0)
			outFile << std::endl;
		outFile << histogram_results[i]<< ' ';
	}

	inFile.close();
	outFile.close();
	
	clReleaseMemObject(inputdata);
    clReleaseMemObject(outputdata);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(commands);
    clReleaseContext(context);
	free(histogram_results);
	delete image;
	
	return 0;
}
