#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <thread>
#include <chrono>
#include <time.h>
#include <vector>
#include <array>
#include <map>
#include <utility>
#include <regex>
#include <memory>
#include <functional>
#include <Windows.h>
#include <complex>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

#define LOG(X) (std::cout << X << std::endl)
#define WAIT() system("pause")
#define ISBOOL(x) (x == 0 ? "false" : "true")
#define ASSERT(x) \
				if(!x){	\
					std::cout << "Error during runtime in file: " << __FILE__ \
							  << ",\nin function: " << __FUNCTION__ << ",\nline: " << __LINE__ << std::endl; \
					system("pause"); \
					exit(-1); \
				}
#define ASSERTM(x, expr) \
				if(!x){ \
					std::cout << expr << std::endl; \
					system("pause"); \
					exit(-1); \
				}

class Timer {
	chrono::steady_clock::time_point before;
public:
	void Start() {
		before = chrono::high_resolution_clock::now();
	}
	void Print() {
		cout << endl << "Time ocupated: " << std::chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - before).count() << " microseonds." << endl;
	}

	int GetTimeNanoS() {
		return std::chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - before).count();
	}

	static void Wait(int time_to_wait) {
		auto time_started = chrono::high_resolution_clock::now();

		while (true) {
			if (chrono::duration_cast<chrono::seconds>(chrono::high_resolution_clock::now() - time_started).count() > time_to_wait)
				break;
		}
	}
};


template<typename func = void(*)()>
class Event {
private:
	vector<func> functions_ = vector<func>();

public:
	Event operator += (func f) {
		functions_.push_back(f);
		return *this;
	}

	Event operator -= (func f) {
		for (auto iter = functions_.begin(); iter < functions_.end(); ++iter){
			if (*iter == f) {
				functions_.erase(iter);
				return *this;
			}
		}
		return *this;
	}

	Event operator()() {
		for (func f : functions_) {
			f();
		}
		return *this;
	}
	
};


struct TreeItem {
	int value;
	TreeItem* right = NULL;
	TreeItem* left = NULL;
	TreeItem() {}
	TreeItem(int v) : value(v) {}
};



class SortAllocator {
	int allocated_size_;
	int allocated_count_;

public:
	SortAllocator() : allocated_size_(0), allocated_count_(0) {}
	
	template<typename T>
	T Allocate() {
		++allocated_count_;
		T ret;
		allocated_size_ += sizeof(ret);
		return ret;
	}

	template<typename T>
	T Allocate(T value) {
		++allocated_count_;
		T ret = value;
		allocated_size_ += sizeof(ret);
		return ret;
	}

	template<typename T>
	T* AllocateArray(int size) {
		++allocated_count_;
		T* ret = new T[size];
		allocated_size_ += sizeof(*ret)*size;
		return ret;
	}

	int GetAllocatedCount() { return allocated_count_; }
	int GetAllocatedSize() { return allocated_size_; }

	void Reset() {
		allocated_count_ = 0;
		allocated_size_ = 0;
	}
};


class Sorts {
	int* arr_;
	int* sorted_;
	int size_;
	int min_;
	int max_;
	int swaps_ = 0;
	int compares_ = 0;
	SortAllocator alloc_;


private:
	void Swap(int& a, int& b) {
		++swaps_;
		std::swap(a, b);
	}
	bool CompareLess(int a, int b) {
		++compares_;
		return a < b;
	}
	bool CompareLess(int* a, int* b) {
		++compares_;
		return a < b;
	}
	bool CompareGreater(int a, int b) {
		++compares_;
		return a > b;
	}
	bool CompareIqual(int a, int b) {
		++compares_;
		return a == b;
	}
	bool CompareNotIqual(int a, int b) {
		return a != b;
	}
	bool CompareLessIqual(int a, int b) {
		++compares_;
		return a <= b;
	}
	bool CompareGreaterIqual(int a, int b) {
		++compares_;
		return a >= b;
	}

	


private:

	bool CompareNotNULL(TreeItem* a) {
		return a != NULL;
	}
	void SearchTree(TreeItem* list, int* arr, int& index) {
		if (CompareNotNULL(list->left))
			SearchTree(list->left, arr, index);
		arr[index] = list->value;
		++index;
		if (CompareNotNULL(list->right))
			SearchTree(list->right, arr, index);
	}
	void DeleteTree(TreeItem* list) {
		if (list->left != NULL)
			DeleteTree(list->left);
		if (list->right != NULL)
			DeleteTree(list->right);
		delete list;
	}

	int QuickSortAlign(int low, int high) {
		int paste_index = alloc_.Allocate<int>(low);
		int i;
		for (i = alloc_.Allocate<int>(low+1); CompareLess(i, high); ++i) {
			if (CompareLess(sorted_[i], sorted_[low])) {
				++paste_index;
				Swap(sorted_[i], sorted_[paste_index]);
			}
		}

		if (CompareNotIqual(paste_index, low)) {
			Swap(sorted_[paste_index], sorted_[low]);
		}

		return paste_index;
	}
	void QuickSort_(int low, int high) {
		if (CompareLess(low, high)) {
			int index = alloc_.Allocate<int>(QuickSortAlign(low, high));
			QuickSort_(low, index);
			QuickSort_(index + 1, high);
		}
	}
	
	void GnomeSort_(int margin) {
		int i;
		for (i = alloc_.Allocate<int>(0); CompareLess(i, size_ - margin); ++(i)) {
			if (CompareGreater(sorted_[i], sorted_[i + margin]))
				Swap(sorted_[i], sorted_[i + margin]);
		}
		if(margin != 1)
			return GnomeSort_(margin-1);
		
	}

	void Heapify(int top, int index) {
		int now = alloc_.Allocate<int>(index);
		int left = alloc_.Allocate<int>(2 * index + 1);
		int right = alloc_.Allocate<int>(2 * index + 2);

		if (CompareLess(left, top) && CompareLess(sorted_[now], sorted_[left]))
			now = left;
		if (CompareLess(right, top) && CompareLess(sorted_[now], sorted_[right]))
			now = right;

		if (CompareNotIqual(now, index)) {
			Swap(sorted_[index], sorted_[now]);
			Heapify(top, now);
		}
	}

	void Merge(int* l, int* m, int* r, int* temp) {
		int* cl = l;
		int* cr = m;
		int cur = 0;
		while (CompareLess(cl, m) && CompareLess(cr, r)) {
			if (cl < cr) temp[cur++] = *cl, cl++;
			else temp[cur++] = *cr, cr++;
		}
		while (CompareLess(cl, m)) temp[cur++] = *cl, cl++;
		while (CompareLess(cr, r)) temp[cur++] = *cr, cr++;
		cur = 0;
		int* i;
		for (i = l; CompareLess(i, r); ++i)
			*i = temp[cur++];
	}
	void Mergesort_(int* l, int* r, int* temp) {
		if (r - l <= 1) return;
		int *m = l + (r - l) / 2;
		Mergesort_(l, m, temp);
		Mergesort_(m, r, temp);
		Merge(l, m, r, temp);
	}
	
	

public:
	Sorts(int size, int min_bound = -100000, int max_bound = 100000) : arr_(new int[size]), size_(size), sorted_(new int[size]), min_(min_bound), max_(max_bound) {
		for (int i = 0; i < size_; ++i) {
			arr_[i] = rand() % (max_-min_) - max_;
		}
		ResetSortedArray();
	}
	
	int* GetArray() { return arr_; }
	int GetSize() { return size_; }



public:
	/*
	*********************************************
					Sorts
	*********************************************
	*/	
	void QuickSort() {
		LOG("QuickSorting....");
		QuickSort_(0, size_);
	}

	void BubbleSort() {
		int i;
		for (i = alloc_.Allocate<int>(0); CompareLess(i, size_-1); ++(i)) {
			if (CompareGreater(sorted_[i], sorted_[i + 1])) {
				Swap(sorted_[i], sorted_[i+1]);
				return BubbleSort();
			}
		}
	}

	void GnomeSort() {
		LOG("GnomeSorting...");
		GnomeSort_(size_ / 2);
	}

	void ShakerSort() {
		LOG("ShakerSorting...");
		int m = alloc_.Allocate<int>(size_);
		int index;
		for (index = alloc_.Allocate<int>(0); CompareLess(index, m); ++index) {
			int i;
			for (i = alloc_.Allocate(index + 1); CompareLess(i, m); ++(i)) {
				if (CompareLess(sorted_[i], sorted_[i - 1]))
					Swap(sorted_[i], sorted_[i - 1]);
			}

			--m;
			for (i = alloc_.Allocate(m - 1); CompareGreater(i, index); --i) {
				if (CompareLess(sorted_[i], sorted_[i - 1]))
					Swap(sorted_[i], sorted_[i - 1]);
			}
		}
	}

	void STLSort() {
		LOG("STLSorting...");
		std::sort(sorted_, sorted_ + size_);
	}

	void TreeSort() {
		LOG("TreeSorting...");
		TreeItem* list = new TreeItem(alloc_.Allocate<TreeItem>(sorted_[0]));

		TreeItem* now = list;
		int i;
		for (i = alloc_.Allocate<int>(1); CompareLess(i, size_); ++(i)) {
			while (true) {
				if (CompareLessIqual(sorted_[i], now->value)) {
					if (CompareNotNULL(now->left))
						now = now->left;
					else {
						now->left = new TreeItem(alloc_.Allocate<TreeItem>(TreeItem(sorted_[i])));
						break;
					}
				}
				else {
					if (CompareNotNULL(now->right))
						now = now->right;
					else {
						now->right = new TreeItem(alloc_.Allocate<TreeItem>(TreeItem(sorted_[i])));
						break;
					}
				}
			}
			now = list;
		}


		int index = alloc_.Allocate<int>(0);
		SearchTree(list, sorted_, index);

		DeleteTree(list);
	}

	void HeapSort() {
		LOG("HeapSorting...");
		int i;
		for (i = alloc_.Allocate<int>(size_ / 2 - 1); CompareGreaterIqual(i, 0); --(i))
			Heapify(size_, i);
		for (i = alloc_.Allocate<int>(size_ - 1); CompareGreaterIqual(i, 0); --(i)) {
			Swap(sorted_[0], sorted_[i]);
			Heapify(i, 0);
		}
	}

	void SelectionSort() {
		LOG("SelectionSorting...");
		int i = alloc_.Allocate<int>(0);
		int smallest = alloc_.Allocate<int>(0);
		int attemp = alloc_.Allocate<int>(0);
		for (i = 0; CompareLess(i, size_); ++(i)) {
			smallest = i;

			for (attemp = i + 1; attemp < size_; ++attemp) {
				if (sorted_[smallest] > sorted_[attemp])
					smallest = attemp;
			}
			Swap(sorted_[i], sorted_[smallest]);
		}

	}

	void MergeSort() {
		LOG("MergeSorting...");
		int* temp = alloc_.AllocateArray<int>(size_);
		Mergesort_(sorted_, sorted_+size_, temp);
		delete[] temp;
	}

	void Combsort() {
		LOG("CombSorting...");
		if (CompareLess(size_, 2)) return;
		double k = alloc_.Allocate<double>(1.2473309);
		int step = alloc_.Allocate<int>(size_ - 1);
		int* i;

		while (CompareLess(1, step)) {
			for (i = sorted_; CompareLess(i + step, sorted_+size_); ++i) {
				if (CompareLess((i + step), i))
					Swap(*i, *(i + step));
			}
			step /= k;
		}
		bool b = true;
		while (b) {
			b = false;
			for (i = sorted_; CompareLess(i + 1, sorted_+size_); ++i) {
				if (*i > *(i + 1)) {
					Swap(*i, *(i + 1));
					b = true;
				}
			}
		}
	}

	/*
	*********************************************
					End of sorts
	*********************************************
	*/

public:

	void PrintArray() {
		for (int i = 0; i < size_; ++i) {
			LOG(sorted_[i]);
		}
	}

	void PrintStats() {
		std::cout << "Compares: " << compares_ << std::endl;
		std::cout << "Memory allocated: " << alloc_.GetAllocatedSize() << std::endl;
		std::cout << "Times allocated: " << alloc_.GetAllocatedCount() << std::endl;
		LOG("-----------");
	}


	void ResetSortedArray() {
		for (int i = 0; i < size_; ++i) {
			sorted_[i] = arr_[i];
		}
	}

	void ResetStats() {
		compares_ = 0;
		alloc_.Reset();
	}
};




int main() {
	
	Timer timer;
	Sorts* sort = new Sorts(100);


	typedef void (Sorts::*sfunction)();
	std::vector<sfunction> funcs = { &Sorts::QuickSort, &Sorts::HeapSort, &Sorts::SelectionSort, &Sorts::MergeSort, &Sorts::STLSort, &Sorts::Combsort, &Sorts::TreeSort, &Sorts::GnomeSort,
											  &Sorts::ShakerSort, &Sorts::BubbleSort };


	for (int i = 0; i < funcs.size(); ++i) {
		for (int i2 = 0; i2 < 5; ++i2) {
			sort->ResetStats();
			sort->ResetSortedArray();
			timer.Start();
			(sort->*funcs[i])();
			timer.Print();
			sort->PrintStats();
		}
	}


	WAIT();
	return 0;
}