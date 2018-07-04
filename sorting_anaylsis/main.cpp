#include <vector>
#include <iostream>
#include <thread>
#include <stdlib.h>
#include <time.h>
#include <chrono>

#include "MySortingAlgorithms.h"

using namespace std;

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "usage: " << argv[0] << " [data size]\n";
		return 1;
	}

	long vec_size = atol(argv[1]);
	if (vec_size > 1000000 || vec_size < 1)
	{
		std::cout << "[data size] must be in range(1, 1,000,000)\n";
		return 1;
	}

	vector<int> unsorted_list(vec_size);
	vector<int> list;
	chrono::high_resolution_clock::time_point time_1, time_2;

	srand(time(NULL));
	
	for (int i = 0; i < vec_size; ++i)
	{
		unsorted_list[i] = rand() % 1000;
	}
	
	// Merge
	cout << "\n\nMerge Sort\n";
	list = unsorted_list;
	time_1 = chrono::high_resolution_clock::now();
	merge_sort<int>(list, 0, list.size()-1);
	time_2 = chrono::high_resolution_clock::now();
	auto merge_time = chrono::duration_cast<chrono::microseconds>(time_2 - time_1).count();
	cout << "Merge sort completed in: " << merge_time << " microseconds";
	

	// Parallel Merge
	cout << "\n\nParallel Merge Sort\n";
	list = unsorted_list;
	time_1 = chrono::high_resolution_clock::now();
	merge_sort_parallel<int>(list, 0, list.size()-1);
	time_2 = chrono::high_resolution_clock::now();
	auto parallel_merge_time = chrono::duration_cast<chrono::microseconds>(time_2 - time_1).count();
	cout << "Parallel Merge sort completed in: " << parallel_merge_time << " microseconds \n";


	// Quick Sort
	cout << "\n\nQuick Sort\n";
	list = unsorted_list;
	time_1 = chrono::high_resolution_clock::now();
	quick_sort<int>(list, 0, list.size() - 1);
	time_2 = chrono::high_resolution_clock::now();
	auto quick_merge_time = chrono::duration_cast<chrono::microseconds>(time_2 - time_1).count();
	cout << "Quick Sort completed in: " << quick_merge_time << " microseconds\n";

	// Parallel Quick Sort
	cout << "\n\nParallel Quick Sort\n";
	list = unsorted_list;
	time_1 = chrono::high_resolution_clock::now();
	quick_sort_parallel<int>(list, 0, list.size() - 1);
	time_2 = chrono::high_resolution_clock::now();
	auto pquick_merge_time = chrono::duration_cast<chrono::microseconds>(time_2 - time_1).count();
	cout << "Quick Sort completed in: " << pquick_merge_time << " microseconds\n";
}