#include <iostream>
#include "select.h"
using namespace std;

int RSelect(int Array[], int n, int i){
	return RandomizedSelect(Array, 0, n-1, i);
}

int DSelect(int Array[], int n, int i){
	return DeterministicSelect(Array, 0, n-1, i);
}


int main(){
	int (*select[2])(int[],int,int)={RSelect, DSelect}; 
	int choice, number, order;
	cin>>choice;//choice is only valid when it's among 0,1
	cin>>number;//in the range [-2^{31},2^{31}-1]
	cin>>order;
	int* integers= new int[number];//initialize with 'new' since stack requires continuous room which implies more limited size than heap 
	for(int i = 0 ; i < number ; i++){
		cin>>integers[i];
	}
	cout<<"The order-"<<order<<" item is "<<select[choice](integers, number, order)<<endl;
	delete[] integers;
}

