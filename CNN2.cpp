
#pragma warning(diagble:4996)
#define _CRT_SECURE_NO_WARNINGS

#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "cnn.h"
#include<time.h>

cl_int err;
#define CHECK_ERROR(err) \
   if(err != CL_SUCCESS) { \
   printf("[%s:%d] OpenCL error %d\n", __FILE__, __LINE__, err); \
   exit(EXIT_FAILURE); \
   }

cl_program program;
cl_context context;
const char *source_code = "kernel.cl";
char *kernel_source;
size_t kernel_source_size;
cl_kernel kernel;
cl_command_queue queue;

cl_uint num_platforms;
cl_platform_id *platforms;
cl_uint num_devices;
cl_device_id *devices = NULL;

char str[1024];
cl_device_type device_type;
size_t max_work_group_size;
cl_ulong global_mem_size;
cl_ulong local_mem_size;
cl_ulong max_mem_alloc_size;
cl_uint p, d;


char *get_source_code(const char *file_name, size_t *len)
{
	char *source_code;
	char buf[2] = "\0";
	int cnt = 0;
	size_t length;
	FILE *file = NULL;
	fopen_s(&file, "kernel.cl", "r");
	if (file == NULL) {
		printf("[%s:%d] Failed to open %s\n", __FILE__, __LINE__, file_name);
		exit(EXIT_FAILURE);
	}
	fseek(file, 0, SEEK_END);
	length = (size_t)ftell(file);
	rewind(file);
	source_code = (char *)malloc(length + 1);
	fread(source_code, length, 1, file);
	for (int i = 0; i < length; i++) {
		buf[0] = source_code[i];
		if (buf[0] == '\n') {
			cnt++;
		}
	}
	source_code[length - cnt] = '\0';
	fclose(file);
	*len = length - cnt;
	return source_code;
}

static void pooling2x2(float *input, float *output, int N) {
	int i, j, k, l;
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			float max = 0;
			for (k = 0; k < 2; k++) {
				for (l = 0; l < 2; l++) {
					float pixel = input[(i * 2 + k) * 2 * N + j * 2 + l];
					max = (max > pixel) ? max : pixel;
				}
			}
			output[i * N + j] = max;
		}
	}//input's size : 4 * N * N
}//output's size : N * N

/*
* D = channel size
* N = width and height of an output image
* Thus, input is (D, N * 2, N * 2) and output is (D, N, N).
*/
static void pooling_layer(float *inputs, float *outputs, int D, int N) {
	int i;
	for (i = 0; i < D; i++) {
		float * input = inputs + i * N * N * 4;
		float * output = outputs + i * N * N;
		pooling2x2(input, output, N);
	}
}
/*
static void convolution3x3(float *input, float *output, float *filter, int N) {
   int i, j, k, l;
   for (i = 0; i < N; i++) {
	  for (j = 0; j < N; j++) {
		 float sum = 0;
		 for (k = 0; k < 3; k++) {
			for (l = 0; l < 3; l++) {
			   int x = i + k - 1;
			   int y = j + l - 1;
			   if (x >= 0 && x < N && y >= 0 && y < N)
				  sum += input[x * N + y] * filter[k * 3 + l];
			}
		 }
		 output[i * N + j] += sum;
	  }
   }
}
*/
/*
* D2 = output channel size
* D1 = input channel size
* N = width and height of an input image
* input image is zero-padded by 1.
* Thus, input is (D1, N, N) and output is (D2, N, N)
*/


#define ReLU(x) (((x)>0)?(x):0)


/*
 * M = output size
 * N = input size
 */



static void convolution_layer(float *inputs, float *outputs, float *filters, float *biases, int D2, int D1, int N) {
	int i, j;

	memset(outputs, 0, sizeof(float) * N * N * D2);

	cl_mem buf_inputs, buf_outputs, buf_filters, buf_biases, buf_inp, buf_fil, buf_bias;

	float *l_inp, *l_fil, *l_bias;

	l_inp = (float*)malloc(sizeof(float) * (N * N * D1));
	l_fil = (float*)malloc(sizeof(float) * (3 * 3 * D1 * D2));
	l_bias = (float*)malloc(sizeof(float) * D2);

	//printf("D1 : %d D2 : %d j : %d i : %d\n",D1,D2, j, i);

	buf_inputs = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * (N * N * D1), NULL, &err);
	CHECK_ERROR(err);
	buf_outputs = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * (N * N * D2), NULL, &err);
	CHECK_ERROR(err);
	buf_filters = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * (3 * 3 * D1 * D2), NULL, &err);
	CHECK_ERROR(err);
	buf_biases = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * (D2), NULL, &err);
	CHECK_ERROR(err);/*
	buf_inp = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * (N * N * D1), NULL, &err);
	CHECK_ERROR(err);
	buf_fil = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * (3 * 3 * D1 * D2), NULL, &err);
	CHECK_ERROR(err);
	buf_bias = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * (D2), NULL, &err);
	CHECK_ERROR(err);*/



	err = clEnqueueWriteBuffer(queue, buf_inputs, CL_TRUE, 0, sizeof(float) * (N * N * D1), inputs, 0, NULL, NULL);
	CHECK_ERROR(err);
	err = clEnqueueWriteBuffer(queue, buf_outputs, CL_TRUE, 0, sizeof(float) * (N * N * D2), outputs, 0, NULL, NULL);
	CHECK_ERROR(err);
	err = clEnqueueWriteBuffer(queue, buf_filters, CL_TRUE, 0, sizeof(float) * (3 * 3 * D1 * D2), filters, 0, NULL, NULL);
	CHECK_ERROR(err)
	err = clEnqueueWriteBuffer(queue, buf_biases, CL_TRUE, 0, sizeof(float) * (D2), biases, 0, NULL, NULL);
	CHECK_ERROR(err)
	/*err = clEnqueueWriteBuffer(queue, buf_inp, CL_TRUE, 0, sizeof(float) * (N * N * D1), l_inp, 0, NULL, NULL);
	CHECK_ERROR(err)
	err = clEnqueueWriteBuffer(queue, buf_fil, CL_TRUE, 0, sizeof(float) * (3 * 3 * D1 * D2), l_fil, 0, NULL, NULL);
	CHECK_ERROR(err)
	err = clEnqueueWriteBuffer(queue, buf_bias, CL_TRUE, 0, sizeof(float) * (D2), l_bias, 0, NULL, NULL);
	CHECK_ERROR(err)*/


	err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&buf_inputs);
	CHECK_ERROR(err);
	/*   err = clSetKernelArg(kernel, 1, sizeof(cl_float)*3*3*D1, NULL);
	   CHECK_ERROR(err);*/
	err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&buf_outputs);
	CHECK_ERROR(err);
	err = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&buf_filters);
	CHECK_ERROR(err);
	err = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*)&buf_biases);
	CHECK_ERROR(err);
	err = clSetKernelArg(kernel, 4, sizeof(cl_int), (void*)&D1);
	CHECK_ERROR(err);
	err = clSetKernelArg(kernel, 5, sizeof(cl_int), (void*)&D2);
	CHECK_ERROR(err);
	err = clSetKernelArg(kernel, 6, sizeof(cl_int), (void*)&N);
	CHECK_ERROR(err);
	err = clSetKernelArg(kernel, 7, sizeof(float)*N*N*D1, NULL);
	CHECK_ERROR(err);
	err = clSetKernelArg(kernel, 7, sizeof(float)*N*N*D2, NULL);
	CHECK_ERROR(err);
	err = clSetKernelArg(kernel, 7, sizeof(float)*D2, NULL);
	CHECK_ERROR(err);
	
	size_t global_size[2] = { N * N,D2 };
	//size_t local_size[2] = { 4,4};

	clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global_size, NULL, 0, NULL, NULL);

	err = clEnqueueReadBuffer(queue, buf_outputs, CL_TRUE, 0, sizeof(float) * (N * N * D2), outputs, 0, NULL, NULL);
	CHECK_ERROR(err);
	//convolution3x3(input, output, filter, N);

	/*
	for (i = 0; i < D2; i++) {
	   float * output = outputs + N * N * i;
	   float bias = biases[i];
	   for (j = 0; j < N * N; j++) {
		  output[j] = ReLU(output[j] + bias);
	   }
	}*/
	clReleaseMemObject(buf_inputs);
	clReleaseMemObject(buf_outputs);
	clReleaseMemObject(buf_filters);
}

/*
* M = output size
* N = input size
*/
static void fc_layer(float *input_neuron, float *output_neuron, float *weights, float *biases, int M, int N) {
	int i, j;
	for (j = 0; j < M; j++) {
		float sum = 0;
		for (i = 0; i < N; i++) {
			sum += input_neuron[i] * weights[j * N + i];
		}
		sum += biases[j];
		output_neuron[j] = ReLU(sum);
	}
}

static void softmax(float *output, int N) {
	int i;
	float max = output[0];
	for (i = 1; i < N; i++) {
		max = (output[i] > max) ? output[i] : max;
	}
	float sum = 0;
	for (i = 0; i < N; i++) {
		sum += exp(output[i] - max);
	}
	for (i = 0; i < N; i++) {
		output[i] = exp(output[i] - max) / sum;
	}
}

static int find_max(float *fc, int N) {
	int i;
	int maxid = 0;
	float maxval = 0;
	for (i = 0; i < N; i++) {
		if (maxval < fc[i]) {
			maxval = fc[i];
			maxid = i;
		}
	}
	return maxid;
}

float* alloc_layer(size_t n) {
	return (float*)malloc(n * sizeof(float));
}

void cnn_init() {
	// nothing to init in the sequential version
}

void cnn(float *images, float **network, int *labels, float *confidences, int num_images) {
	// slice the network into weights and biases
	float *w1_1, *b1_1, *w1_2, *b1_2;
	float *w2_1, *b2_1, *w2_2, *b2_2;
	float *w3_1, *b3_1, *w3_2, *b3_2, *w3_3, *b3_3;
	float *w4_1, *b4_1, *w4_2, *b4_2, *w4_3, *b4_3;
	float *w5_1, *b5_1, *w5_2, *b5_2, *w5_3, *b5_3;
	float *w1, *b1, *w2, *b2, *w3, *b3;
	w1_1 = network[0]; b1_1 = network[1];
	w1_2 = network[2]; b1_2 = network[3];
	w2_1 = network[4]; b2_1 = network[5];
	w2_2 = network[6]; b2_2 = network[7];
	w3_1 = network[8]; b3_1 = network[9];
	w3_2 = network[10]; b3_2 = network[11];
	w3_3 = network[12]; b3_3 = network[13];
	w4_1 = network[14]; b4_1 = network[15];
	w4_2 = network[16]; b4_2 = network[17];
	w4_3 = network[18]; b4_3 = network[19];
	w5_1 = network[20]; b5_1 = network[21];
	w5_2 = network[22]; b5_2 = network[23];
	w5_3 = network[24]; b5_3 = network[25];
	w1 = network[26]; b1 = network[27];
	w2 = network[28]; b2 = network[29];
	w3 = network[30]; b3 = network[31];
	//printf("아오~~~~~1");

	// allocate memory for output of each layer
	float *c1_1, *c1_2, *p1;
	float *c2_1, *c2_2, *p2;
	float *c3_1, *c3_2, *c3_3, *p3;
	float *c4_1, *c4_2, *c4_3, *p4;
	float *c5_1, *c5_2, *c5_3, *p5;
	float *fc1, *fc2, *fc3;
	c1_1 = alloc_layer(64 * 32 * 32);
	c1_2 = alloc_layer(64 * 32 * 32);
	p1 = alloc_layer(64 * 16 * 16);
	c2_1 = alloc_layer(128 * 16 * 16);
	c2_2 = alloc_layer(128 * 16 * 16);
	p2 = alloc_layer(128 * 8 * 8);
	c3_1 = alloc_layer(256 * 8 * 8);
	c3_2 = alloc_layer(256 * 8 * 8);
	c3_3 = alloc_layer(256 * 8 * 8);
	p3 = alloc_layer(256 * 4 * 4);
	c4_1 = alloc_layer(512 * 4 * 4);
	c4_2 = alloc_layer(512 * 4 * 4);
	c4_3 = alloc_layer(512 * 4 * 4);
	p4 = alloc_layer(512 * 2 * 2);
	c5_1 = alloc_layer(512 * 2 * 2);
	c5_2 = alloc_layer(512 * 2 * 2);
	c5_3 = alloc_layer(512 * 2 * 2);
	p5 = alloc_layer(512 * 1 * 1);
	fc1 = alloc_layer(512);
	fc2 = alloc_layer(512);
	fc3 = alloc_layer(10);
	//printf("아오~~~~~2");

	// run network
	printf("num of images : %d\n", num_images);
	for (int i = 0; i < num_images; ++i)
	{
		printf("i : %d\n", i);
		float *image = images + i * 3 * 32 * 32;

		/*   printf("inputs\n");
		   for (int a = 0; a < 32; a++)
		   {
			  for (int b = 0; b < 32; b++)
			  {
				 printf("%.2f ", image[(b * 32 + a)]);
			  }
			  printf("\n");
		   }*/

		   //printf("pre_convolution_1\n");
		convolution_layer(image, c1_1, w1_1, b1_1, 64, 3, 32);
		//   printf("convolution_1\n");

		convolution_layer(c1_1, c1_2, w1_2, b1_2, 64, 64, 32);
		//printf("convolution_2\n");
		pooling_layer(c1_2, p1, 64, 16);

		convolution_layer(p1, c2_1, w2_1, b2_1, 128, 64, 16);
		//printf("convolution_3\n");
		convolution_layer(c2_1, c2_2, w2_2, b2_2, 128, 128, 16);
		//printf("convolution_4\n");
		pooling_layer(c2_2, p2, 128, 8);
		
		convolution_layer(p2, c3_1, w3_1, b3_1, 256, 128, 8);
		//printf("convolution_5\n");
		convolution_layer(c3_1, c3_2, w3_2, b3_2, 256, 256, 8);
		//printf("convolution_6\n");
		convolution_layer(c3_2, c3_3, w3_3, b3_3, 256, 256, 8);
		//printf("convolution_7\n");
		pooling_layer(c3_3, p3, 256, 4);

		convolution_layer(p3, c4_1, w4_1, b4_1, 512, 256, 4);
		//printf("convolution_8\n");
		convolution_layer(c4_1, c4_2, w4_2, b4_2, 512, 512, 4);
		//printf("convolution_9\n");
		convolution_layer(c4_2, c4_3, w4_3, b4_3, 512, 512, 4);
		//printf("convolution_10\n");
		pooling_layer(c4_3, p4, 512, 2);

		convolution_layer(p4, c5_1, w5_1, b5_1, 512, 512, 2);
		//printf("convolution_11\n");
		convolution_layer(c5_1, c5_2, w5_2, b5_2, 512, 512, 2);
		//printf("convolution_12\n");
		convolution_layer(c5_2, c5_3, w5_3, b5_3, 512, 512, 2);
		//printf("convolution_13\n");
		pooling_layer(c5_3, p5, 512, 1);

		fc_layer(p5, fc1, w1, b1, 512, 512);
		//      printf("fc_1\n");
		fc_layer(fc1, fc2, w2, b2, 512, 512);
		//   printf("fc_2\n");
		fc_layer(fc2, fc3, w3, b3, 10, 512);
		//printf("fc_3\n");

		softmax(fc3, 10);
		//printf("soft\n");

		labels[i] = find_max(fc3, 10);
		//printf("find_max\n");
		confidences[i] = fc3[labels[i]];
		//printf("confi\n");
		//printf("Y ");

	}

	free(c1_1); free(c1_2); free(p1);
	free(c2_1); free(c2_2); free(p2);
	free(c3_1); free(c3_2); free(c3_3); free(p3);
	free(c4_1); free(c4_2); free(c4_3); free(p4);
	free(c5_1); free(c5_2); free(c5_3); free(p5);
	free(fc1); free(fc2); free(fc3);
}

const char *CLASS_NAME[] = {
   "airplane",
   "automobile",
   "bird",
   "cat",
   "deer",
   "dog",
   "frog",
   "horse",
   "ship",
   "truck"
};

void print_usage_and_exit(char **argv) {
	fprintf(stderr, "Usage: %s <number of image> <output>\n", argv[0]);
	fprintf(stderr, " e.g., %s 3000 result.out\n", argv[0]);
	exit(EXIT_FAILURE);
}

void* read_bytes(const char *fn, size_t n) {
	FILE *f = fopen(fn, "rb");
	void *bytes = malloc(n);
	size_t r = fread(bytes, 1, n, f);
	fclose(f);
	if (r != n) {
		fprintf(stderr,
			"%s: %zd bytes are expected, but %zd bytes are read.\n",
			fn, n, r);
		exit(EXIT_FAILURE);
	}
	return bytes;
}

/*
* Read images from "cifar10_image.bin".
* CIFAR-10 test dataset consists of 10000 images with (3, 32, 32) size.
* Thus, 10000 * 3 * 32 * 32 * sizeof(float) = 122880000 bytes are expected.
*/
const int IMAGE_CHW = 3 * 32 * 32 * sizeof(float);
float* read_images(size_t n) {
	return (float*)read_bytes("cifar10_image.bin", n * IMAGE_CHW);
}

/*
* Read labels from "cifar10_label.bin".
* 10000 * sizeof(int) = 40000 bytes are expected.
*/
int* read_labels(size_t n) {
	return (int*)read_bytes("cifar10_label.bin", n * sizeof(int));
}

/*
* Read network from "network.bin".
* conv1_1 : weight ( 64,   3, 3, 3) bias ( 64)
* conv1_2 : weight ( 64,  64, 3, 3) bias ( 64)
* conv2_1 : weight (128,  64, 3, 3) bias (128)
* conv2_2 : weight (128, 128, 3, 3) bias (128)
* conv3_1 : weight (256, 128, 3, 3) bias (256)
* conv3_2 : weight (256, 256, 3, 3) bias (256)
* conv3_3 : weight (256, 256, 3, 3) bias (256)
* conv4_1 : weight (512, 256, 3, 3) bias (512)
* conv4_2 : weight (512, 512, 3, 3) bias (512)
* conv4_3 : weight (512, 512, 3, 3) bias (512)
* conv5_1 : weight (512, 512, 3, 3) bias (512)
* conv5_2 : weight (512, 512, 3, 3) bias (512)
* conv5_3 : weight (512, 512, 3, 3) bias (512)
* fc1     : weight (512, 512) bias (512)
* fc2     : weight (512, 512) bias (512)
* fc3     : weight ( 10, 512) bias ( 10)
* Thus, 60980520 bytes are expected.
*/
const int NETWORK_SIZES[] = {
   64 * 3 * 3 * 3, 64,
   64 * 64 * 3 * 3, 64,
   128 * 64 * 3 * 3, 128,
   128 * 128 * 3 * 3, 128,
   256 * 128 * 3 * 3, 256,
   256 * 256 * 3 * 3, 256,
   256 * 256 * 3 * 3, 256,
   512 * 256 * 3 * 3, 512,
   512 * 512 * 3 * 3, 512,
   512 * 512 * 3 * 3, 512,
   512 * 512 * 3 * 3, 512,
   512 * 512 * 3 * 3, 512,
   512 * 512 * 3 * 3, 512,
   512 * 512, 512,
   512 * 512, 512,
   10 * 512, 10
};

float* read_network() {
	return (float*)read_bytes("network.bin", 60980520);
}

float** slice_network(float *p) {
	float **r = (float**)malloc(sizeof(float*) * 32);
	for (int i = 0; i < 32; ++i) {
		r[i] = p;
		p += NETWORK_SIZES[i];
	}
	return r;
}

int main(int argc, char **argv) {
	err = clGetPlatformIDs(0, NULL, &num_platforms);
	CHECK_ERROR(err);

	platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id) * num_platforms);
	err = clGetPlatformIDs(num_platforms, platforms, NULL);
	CHECK_ERROR(err);

	for (p = 0; p < num_platforms; p++)
	{

		err = clGetPlatformInfo(platforms[p], CL_PLATFORM_NAME, 1024, str, NULL);
		CHECK_ERROR(err);

		err = clGetPlatformInfo(platforms[p], CL_PLATFORM_VENDOR, 1024, str, NULL);
		CHECK_ERROR(err);

		err = clGetDeviceIDs(platforms[p], CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices);
		CHECK_ERROR(err);

		devices = (cl_device_id*)malloc(sizeof(cl_device_id)*num_devices);
		err = clGetDeviceIDs(platforms[p], CL_DEVICE_TYPE_ALL, num_devices, devices, NULL);
		CHECK_ERROR(err);

		for (d = 0; d < num_devices; d++)
		{

			err = clGetDeviceInfo(devices[d], CL_DEVICE_TYPE, sizeof(cl_device_type), &device_type, NULL);
			CHECK_ERROR(err);

			err = clGetDeviceInfo(devices[d], CL_DEVICE_NAME, 1024, str, NULL);
			CHECK_ERROR(err);

			err = clGetDeviceInfo(devices[d], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &max_work_group_size, NULL);
			CHECK_ERROR(err);

			err = clGetDeviceInfo(devices[d], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &global_mem_size, NULL);
			CHECK_ERROR(err);

			err = clGetDeviceInfo(devices[d], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), &local_mem_size, NULL);
			CHECK_ERROR(err);

			err = clGetDeviceInfo(devices[d], CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(cl_ulong), &max_mem_alloc_size, NULL);
			CHECK_ERROR(err);
		}
	}
	context = clCreateContext(NULL, 1, &devices[0], NULL, NULL, &err);
	CHECK_ERROR(err);

	//command queue생성
	//cl_command_queue queue;
	queue = clCreateCommandQueue(context, devices[0], 0, &err);
	CHECK_ERROR(err);

	//OpenCL 프로그램 컴파일
	const char *source_code = "kernel.cl";
	size_t source_size = strlen(source_code);
	//   cl_program program;
	program = clCreateProgramWithSource(context, 1, &source_code, &source_size, &err);
	CHECK_ERROR(err);

	//char *kernel_source;
	//size_t kernel_source_size;
	kernel_source = get_source_code("kernel.cl", &kernel_source_size);

	program = clCreateProgramWithSource(context, 1, (const char**)&kernel_source, &kernel_source_size, &err);
	CHECK_ERROR(err);

	//program build
	err = clBuildProgram(program, 1, devices, NULL, NULL, NULL);

	if (err == CL_BUILD_PROGRAM_FAILURE) {
		// Determine the size of the log
		size_t log_size;
		clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

		// Allocate memory for the log
		char *log = (char *)malloc(log_size);

		// Get the log
		clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

		// Print the log
		printf("%s\n", log);
	}
	CHECK_ERROR(err);
	//"-cl-fast-relaxed-math"*/

	//kernel object
	//cl_kernel kernel;
	kernel = clCreateKernel(program, "convolution", &err);
	CHECK_ERROR(err);



	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	printf("argc : %d\n", argc);
	if (argc != 3) {
		print_usage_and_exit(argv);
	}
	int num_images = atoi(argv[1]);
	float *images = read_images(num_images);
	float *network = read_network();
	float **network_sliced = slice_network(network);
	int *labels = (int*)calloc(num_images, sizeof(int));
	float *confidences = (float*)calloc(num_images, sizeof(float));
	time_t start, end;

	cnn_init();
	start = clock();

	cnn(images, network_sliced, labels, confidences, num_images);

	end = clock();
	printf("Elapsed time: %f sec\n", (double)(end - start) / CLK_TCK);

	FILE *of = fopen(argv[2], "w");
	int *labels_ans = read_labels(num_images);
	double acc = 0;
	for (int i = 0; i < num_images; ++i) {
		fprintf(of, "Image %04d: %s %f\n", i, CLASS_NAME[labels[i]], confidences[i]);
		if (labels[i] == labels_ans[i]) ++acc;
	}
	fprintf(of, "Accuracy: %f\n", acc / num_images);
	fclose(of);

	free(images);
	free(network);
	free(network_sliced);
	free(labels);
	free(confidences);
	free(labels_ans);

	return 0;
}









