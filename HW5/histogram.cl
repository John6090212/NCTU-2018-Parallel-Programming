__kernel void histogram(__global unsigned int *image_data,__global unsigned int *outputdata){
	unsigned int i = get_global_id(0);

	unsigned int index = image_data[i];
	// histogram of R
	if(i % 3 == 0){
		outputdata[i] = index;
	}
	// histogram of G
	else if(i % 3 == 1){
		outputdata[i] = index+256;
	}
	// histogram of B	
	else if(i % 3 == 2){
		outputdata[i] = index+512;
	}
	
}