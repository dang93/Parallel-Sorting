#pragma once

#include <vector>
#include "MySortingAlgorithms.h"
#include <iostream>
#include <thread>
#include <atomic>

const int numThr = 8;
std::atomic<int> curThr = 0;

#pragma optimize( "", off )

void print_vector(const std::vector<int>& vec)
{
	std::cout << "{ ";
	int count = 1;
	for (std::vector<int>::const_iterator it = vec.begin(); it != vec.end() - 1; ++it)
	{
		if (count % 10 == 0) std::cout << "\n";
		std::cout << *it << ", ";
		count++;
	}
	std::cout << *(vec.end() - 1) << " }" << std::endl;
}

// Swap element at position a and b
template<typename T>
void swap(std::vector<T>& vec, int a, int b)
{
	if (a < 0 || a >= int(vec.size())) return;
	if (b < 0 || b >= int(vec.size())) return;
	if (a == b) return;

	T temp = vec[a];
	vec[a] = vec[b];
	vec[b] = temp;
}

template<typename T>
void verify_sorted(const std::vector<T>& vec)
{
	for (std::vector<T>::const_iterator it = vec.begin(); it != vec.end() - 1; ++it)
	{
		if (*it > *(it + 1))
		{
			std::cout << "error, not in order: { " << *it << ", " << *(it + 1) << " }\n";
			return;
		}
	}
	std::cout << "in order\n";
}

template<typename T>
void insertion_sort(std::vector<T>& vec)
{
	for (unsigned int i = 1; i < vec.size(); ++i)
	{
		int j = i - 1;
		while (j >= 0 && vec[j] >= vec[j + 1])
		{
			swap<T>(vec, j, j + 1);
			j--;
		}
	}
}

template<typename T>
void merge_sort(std::vector<T>& vec, int left, int right)
{
	if (left < right)
	{
		int middle = (left + right) / 2;
		merge_sort<T>(vec, left, middle);
		merge_sort<T>(vec, middle + 1, right);

		merge<T>(vec, left, middle, right);
	}
}

template<typename T>
void merge(std::vector<T>& vec, int left, int middle, int right)
{
	int i, j, k;

	int size_left_vec = middle - left + 1;
	int size_right_vec = right - middle;

	// Temp Vectors
	std::vector<T> left_vec(size_left_vec);
	std::vector<T> right_vec(size_right_vec);

	// Copy data
	for (i = 0; i < size_left_vec; ++i)
	{
		left_vec[i] = vec[left + i];
	}
	for (j = 0; j < size_right_vec; ++j)
	{
		right_vec[j] = vec[middle + 1 + j];
	}

	// Merge temp vectors back into main vector
	i = 0;
	j = 0;
	k = left;

	while (i < size_left_vec && j < size_right_vec)
	{
		// Guarantee Non-overlapping sub-vectors, so we are safe 
		// writing without locking

		//mtx.lock();
		if (left_vec[i] <= right_vec[j])
		{
			vec[k] = left_vec[i];
			i++;
		}
		else
		{
			vec[k] = right_vec[j];
			j++;
		}
		//mtx.unlock();
		k++;
	}

	// Copy any remaining elements from left, right vectors
	while (i < size_left_vec)
	{
		vec[k] = left_vec[i];
		i++;
		k++;
	}

	while (j < size_right_vec)
	{
		vec[k] = right_vec[j];
		j++;
		k++;
	}
}

template<typename T>
void merge_sort_parallel(std::vector<T>& vec, int left, int right)
{
	std::thread *t_array[numThr];
	int start, end, middle;
	int size = vec.size();

	// Split main vector into subvector, giving each to a thread
	for (int i = 0; i < numThr; ++i)
	{
		start = i*size / numThr;
		end = (i + 1)*size / numThr - 1;
		if (i == numThr - 1)
			end = size - 1;

		t_array[i] = new std::thread(
			merge_sort<T>, ref(vec),
			start, end);
	}

	for (int i = 0; i < numThr; i++)
	{
		t_array[i]->join();
		delete t_array[i];
	}

	// Each of the original subvectors has been sorted, begin merging them
	for (int i = 0; i < numThr / 2; ++i)
	{
		start = i*size / (numThr / 2);
		end = (i + 1)*size / (numThr / 2) - 1;
		if (i == ((numThr / 2) - 1))
			end = size - 1;
		middle = (start + end) / 2;

		t_array[i] = new std::thread(
			merge<T>, ref(vec),
			start, middle, end);
	}
	for (int i = 0; i < numThr / 2; ++i)
	{
		t_array[i]->join();
		delete t_array[i];
	}

	for (int i = 0; i < numThr / 4; ++i)
	{
		start = i*size / (numThr / 4);
		end = (i + 1)*size / (numThr / 4) - 1;
		if (i == ((numThr / 4) - 1))
			end = size - 1;
		middle = (start + end) / 2;

		t_array[i] = new std::thread(
			merge<T>, ref(vec),
			start, middle, end);
	}
	for (int i = 0; i < numThr / 4; ++i)
	{
		t_array[i]->join();
		delete t_array[i];
	}

	// Final merge
	merge<T>(ref(vec), 0, (vec.size() / 2) - 1, vec.size() - 1);
}


template<typename T>
int partition(std::vector<T>& vec, int start, int end)
{
	int pivot = end;
	int low = start - 1;
	
	for (int i = start; i <= end - 1; ++i)
	{
		// If element is smaller or equal to pivat
		if (vec[i] <= vec[pivot])
		{
			low++;
			swap<T>(vec, low, i);
		}
	}

	swap<int>(vec, low + 1, pivot);
	return low + 1;
}

template<typename T>
void quick_sort(std::vector<T>& vec, int left, int right)
{
	if (left < right)
	{
		int partition_index = partition<int>(vec, left, right);

		quick_sort<T>(ref(vec), left, partition_index - 1);
		quick_sort<T>(ref(vec), partition_index + 1, right);
	}
}

template<typename T>
void quick_sort_parallel(std::vector<T>& vec, int start, int end)
{
	if (start >= end) return;

	int partition_index = partition<int>(vec, start, end);

	// If there are no available threads, resort to non-parallelized sort
	if (curThr >= numThr || ((numThr - curThr) < 2 ))
	{
		quick_sort<T>(ref(vec), start, partition_index - 1);
		quick_sort<T>(ref(vec), partition_index + 1, end);
	}
	else
	{
		std::thread *t_array[2];
		curThr += 2;

		t_array[0] = new std::thread(
			quick_sort_parallel<T>, ref(vec),
			start, partition_index - 1);
		t_array[1] = new std::thread(
			quick_sort_parallel<T>, ref(vec),
			partition_index + 1, end);

		t_array[0]->join();
		t_array[1]->join();
	}
}

#pragma optimize( "", on )