#ifndef __SELECT_H__
#define __SELECT_H__
#include <cstdlib>

static void InsertionSort(int Array[],int left, int right){
	//REQUIRES: Array initialized with index x and y, 
	//with elements in [-2^{31}, 2^{31}-1] and no more than 2^{31} elements
	//MODIFIES: Array
	//EFFECTS: sort the Array
	int number = right - left + 1;
	int j;
	for(int i=1; i<number; i++){
		int temp=Array[left+i];
		for(j=i; j>0 && temp<Array[left+j-1]; j--){ 
			//once Array[j-1]<=Array[j] stop checking since all elements before have been sorted
			Array[left+j]=Array[left+j-1]; 
			//copy Array[j-1] to Array[j] instead of swap everytime to save time
		}
		Array[left+j]=temp;
	}
}

static inline void swap(int Array[],int x,int y){
	//REQUIRES: Array initialized with index x and y, 
	//with elements in [-2^{31}, 2^{31}-1] and no more than 2^{31} elements
	//MODIFIES: Array[x] and Array[y]
	//EFFECTS: swap Array[x] with Array[y]
	int temp=Array[x]; 
	Array[x]=Array[y]; 
	Array[y]=temp;
}

//Partition in place
static void partition_in_place(int Array[], int left, int right, int & pivot_index){
	//REQUIRES: Array initialized with with left and right as legal index 
	//with elements in [-2^{31}, 2^{31}-1] and no more than 2^{31} elements
	//MODIFIES: Array
	//EFFECTS: rearrange the array to make it partitioned by a pivot 
	//and return the index of the pivot
	swap(Array, left, pivot_index);
	int i=left;
	int j=right;
	int pivot=Array[left];
	while(i < j){
        while(i<j && pivot<=Array[j])
			j--;
        while(i<j && Array[i]<=pivot)
			i++;
        swap(Array,i,j);
    }
	swap(Array,left,i);
	pivot_index = i;
}

//Get a random pivot
static int RandPivot(int left, int right){
	//REQUIRES: left and right in [0,2^{31}-1]
	//EFFECTS: return a number in the range [left, right]
	int result = (rand() % (right - left) ) + left;
	return result; 
	//here returns a random index in the range as the pivot index
}

//Randomized Select
int RandomizedSelect(int Array[],int left, int right, int i){
	//REQUIRES: Array of integers initialized with index [left,right]
	//EFFECTS: return the (i+1)-th smallest element of the array 
	if (left==right)
		return Array[left];
	int pivot_index = RandPivot(left, right);
	partition_in_place(Array, left, right, pivot_index);
	int number = pivot_index - left + 1; // size of the left subarray
	if(i== (number - 1))
		return Array[pivot_index];
	else if(i < (number-1))
		return RandomizedSelect(Array, left, pivot_index-1, i);
	else return RandomizedSelect(Array, pivot_index+1, right, i - number);	
}

//Deterministic Select
int DeterministicSelect(int Array[],int left, int right, int i){
	//REQUIRES: Array of integers initialized with index [left,right]
	//EFFECTS: return the (i+1)-th smallest element of the array 
	if (left==right)
		return Array[left];
	int index = left;
	int num = (right - left + 1 + 5) / 5; //number of groups
	int * C = new int [num];
	int cnt;
	for(cnt = 0; cnt < num-1; cnt ++){
		InsertionSort(Array, index, index+4);
		C[cnt] = Array[index+2];
		index += 5;
	}
	InsertionSort(Array, index, right);
	C[cnt] = ((index+2)<=right) ? Array[index+2] : Array[right]; //The last median
	int pivot = DeterministicSelect(C, 0, num-1, num/2); //find the median of medians
	delete[] C;
	for(index = left; Array[index]!=pivot; index++){} //'index' is the index of pivot
	partition_in_place(Array, left, right, index);
	if (i == (index - left)) 
		return pivot;
	else if (i < (index - left) ) 
		return DeterministicSelect(Array, left, index-1, i);
	else
		return DeterministicSelect(Array, index+1, right, i - (index - left + 1));
}


#endif

