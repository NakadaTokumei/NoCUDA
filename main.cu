#include <iostream>
#include <cstdio>
#include <cstdint>

__global__
void TestKernel(
	uint32_t* out_arr
)
{
	uint32_t i = 0;

	for(; i < threadIdx.x; i++)
		out_arr[threadIdx.x] += i;
}

int main(int argc, char** argv)
{
	uint32_t* arr = nullptr;
	uint32_t arr_host[100] = {0,};

	std::cout << "Hello, CUDA" << std::endl;

	cudaMalloc(reinterpret_cast<void**>(&arr), sizeof(uint32_t)*100);

	TestKernel<<<1, 100>>>(arr);

	cudaMemcpy(arr_host, arr, sizeof(uint32_t)*100, cudaMemcpyDeviceToHost);

	cudaFree(arr);

	for(uint32_t i = 0; i < 100; i++)
		std::cout << i << " : " << arr_host[i] << std::endl;
}
