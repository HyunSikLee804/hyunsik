/*__kernel
void convolution(__global float *inputs, __global float *outputs, __global float *filters, int D1,int D2,int N) {
   int i = get_global_id(0);
   int j = get_global_id(1);
   int k, l;

   //printf("ADf");

   
   __global float *input = inputs + N * N * i;
   __global float *output = outputs + N * N * j;
   __global float *filter = filters + 3 * 3 * (j * D1 + i);

   

   float sum = 0;
   for (k = 0; k < 3; k++) {
       for (l = 0; l < 3; l++) {
           int x = i + k - 1;
           int y = j + l - 1;
           if (x >= 0 && x < N && y >= 0 && y < N)
               sum += input[x * N + y] * filter[k * 3 + l];
       }
   }
   outputs[i * N + j] += sum;
}*/
/*
__kernel void convolution(__global float *inputs, __global float *outputs, __global float *filters, int N, int D1, int D2,
	__local float* l_in, __local float* l_filter) {
	int i = get_global_id(0);
	int j = get_global_id(1);
	int l_i = i % (N*N);
	int k, l, m, x, y;
	int l_x, l_y;
	float sum = 0;
	int p, in, fil;
	x = l_i / N;
	y = l_i % N;
	printf("커널이다");
	for (p = 0; p < N*N*D1; p++)
		l_in[p] = inputs[p];
	for (p = 0; p < 9 * D1*D2; p++)
		l_filter[p] = filters[p];
	//for (p = 0; p < D2; p++)
		//l_bias[p] = l_bias[p];
	barrier(CLK_LOCAL_MEM_FENCE);

	for (k = 0; k < D1; k++) {
		for (l = 0; l < 3; l++) {
			for (m = 0; m < 3; m++) {
				l_x = x + l - 1;
				l_y = y + m - 1;
				in = l_in[k*N*N + l_x * N + l_y];
				fil = l_filter[j*D1 * 9 + 9 * k + 3 * l + m];
				(fil&&in&&l_x >= 0 && l_x < N&&l_y >= 0 && l_y < N) && (sum += in*fil);
			}
		}
	}
	//sum += bias[j];
	outputs[i + (N*N)*j] = sum > 0 ? sum : 0;

}*/
/*__kernel
void convolution(__global float *inputs, __global float *outputs, __global float *filters, int D1, int D2, int N) {
	int i;
	int j = get_global_id(0);
	int k, l;

	//printf("ADf");
	for (i = 0; i < D1; i++) {
		__global float * input = inputs + N * N * i;
		__global float * output = outputs + N * N * j;
		__global float * filter = filters + 3 * 3 * (j * D1 + i);
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
*/
__kernel void convolution(__global float *inputs, __global float *outputs, __global float *filters, __global float *biases, int D1, int D2, int N, __local float* l_inp, __local float* l_fil, __local float* l_bias)
{
	int index = get_global_id(0);//처리하고자 하는 행 (0 ~ N * N * D1) 
	int output_index = get_global_id(1);//0 ~ (D2 - 1)

	int index_of_matrix = index % (N * N);//하나의 이미지 내에서의 인덱스
	int x_index_of_matrix = index_of_matrix % N;//처리하고자 하는 곳의 열 좌표
	int y_index_of_matrix = index_of_matrix / N;//처리하고자 하는 곳의 행 좌표

	int l_x, l_y;
	int p, inp, fil;
	//좌표는 (y,x) 로 표현한다.


	int i, j, iter;//for문 용
	float sum = 0.0;

	for (p = 0; p < N*N*D1; p++)
		l_inp[p] = inputs[p];
	for (p = 0; p < 9 * D1*D2; p++)
		l_fil[p] = filters[p];
	for (p = 0; p < D2; p++)
		l_bias[p] = l_bias[p]; //여기는 왜 똑같을까? biases에 넣어야함
	barrier(CLK_LOCAL_MEM_FENCE);
	
	for (iter = 0; iter < D1; iter++)
	{
		for (j = 0; j < 3; j++)
		{
			for (i = 0; i < 3; i++)
			{
				l_x = y_index_of_matrix + j - 1;
				l_y = x_index_of_matrix + i - 1;
				
				inp = l_inp[iter*N*N + l_x * N + l_y];
				fil = l_fil[output_index*D1 * 9 + 9 * iter + 3 * j + i];
				
				(fil&&inp&&l_x >= 0 && l_x < N&&l_y >= 0 && l_y < N) && (sum += inp * fil);
				
				//(l_x >= 0 && l_x < N && l_y >= 0 && l_y < N) && (sum += inputs[iter*N*N + l_x * N + l_y] * filters[output_index*D1 * 9 + 9 * iter + 3 * j + i]);
				/*
				if ((y_index_of_matrix + (j - 1) >= 0) && (y_index_of_matrix + (j - 1) < N) && (x_index_of_matrix + (i - 1) >= 0) && (x_index_of_matrix + (i - 1) < N))
				{
					sum = sum + inputs[N * N * iter + (y_index_of_matrix + (j - 1)) * N + x_index_of_matrix + (i - 1)] * filters[output_index * 3 * 3 * D1 + iter * 3 * 3 + j * 3 + i];
				}*/
				//컨볼루션 구현
			}
		}
		/*
		if ((y_index_of_matrix >= 2) && (y_index_of_matrix < N-2) && (x_index_of_matrix >=2) && (x_index_of_matrix < N-2)) {

		   sum+= inputs[N * N * iter + (y_index_of_matrix +(-1- 1)) * N + x_index_of_matrix +(-1 - 1)] * filters[output_index * 3 * 3 * D1 + iter * 3 * 3 +( -1 * 3) -1];
		   sum += inputs[N * N * iter + (y_index_of_matrix +(-1 - 1)) * N + x_index_of_matrix +(0 - 1)] * filters[output_index * 3 * 3 * D1 + iter * 3 * 3 + (-1 * 3) + 0];
		   sum += inputs[N * N * iter + (y_index_of_matrix +(- 1 - 1)) * N + x_index_of_matrix +( 1 - 1)] * filters[output_index * 3 * 3 * D1 + iter * 3 * 3 + (-1 * 3) + 1];

		   sum += inputs[N * N * iter + (y_index_of_matrix +(0 - 1)) * N + x_index_of_matrix + (-1 - 1)] * filters[output_index * 3 * 3 * D1 + iter * 3 * 3 + (0 * 3) - 1];
		   sum += inputs[N * N * iter + (y_index_of_matrix +(0 - 1)) * N + x_index_of_matrix + (0 - 1)] * filters[output_index * 3 * 3 * D1 + iter * 3 * 3 + (0 * 3) + 0];
		   sum += inputs[N * N * iter + (y_index_of_matrix +(0 - 1)) * N + x_index_of_matrix + (1 - 1)] * filters[output_index * 3 * 3 * D1 + iter * 3 * 3 + (0 * 3) + 1];

		   sum += inputs[N * N * iter + (y_index_of_matrix +(1 - 1)) * N + x_index_of_matrix + (-1 - 1)] * filters[output_index * 3 * 3 * D1 + iter * 3 * 3 + (1 * 3) - 1];
		   sum += inputs[N * N * iter + (y_index_of_matrix + (1 - 1)) * N + x_index_of_matrix + (0 - 1)] * filters[output_index * 3 * 3 * D1 + iter * 3 * 3 + (1* 3) + 0];
		   sum += inputs[N * N * iter + (y_index_of_matrix + (1 - 1)) * N + x_index_of_matrix + (1 - 1)] * filters[output_index * 3 * 3 * D1 + iter * 3 * 3 + (1 * 3) + 1];

		}
		*/
	}
	sum += biases[output_index];
	outputs[index + (N*N)*output_index] = sum > 0 ? sum : 0;
	//outputs[index + (N*N)*output_index] = (sum + biases[output_index]) > 0 ? (sum + biases[output_index]) : 0;
	/*sum += biases[output_index];

	outputs[(N * N) * output_index + y_index_of_matrix * N + x_index_of_matrix] = (sum > 0) ? sum : 0;*/
}