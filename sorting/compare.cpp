#include <fstream>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include "select.h"
#include "QSort.h"
using namespace std;

const int MAXSIZE=1000000;  //the maximum size of the arrays to be tested
const int MINSIZE=10;		//the minimum size of the arrays to be tested
const int STEP=10;
const int METHODSNUMBER=3;
const int INDEXNUMBER=5;
const string METHOSNAMES[3]={"RSelect", "DSelect","SortSelect"};

int RSelect(int Array[], int n, int i){
	return RandomizedSelect(Array, 0, n-1, i);
}

int DSelect(int Array[], int n, int i){
	return DeterministicSelect(Array, 0, n-1, i);
}

int SortSelect(int Array[], int n, int i){
	QuickSort(Array, n);
	return Array[i];
}

int main(){
	ofstream output;
	output.open("CompareResult.txt");
	int * integers= new int [MAXSIZE];
	int * auxiliary= new int [MAXSIZE];
	clock_t start, end;
	double time[METHODSNUMBER]={0,0,0};
	int (*select[METHODSNUMBER])(int[], int,int)={RSelect, DSelect, SortSelect};
	int index[INDEXNUMBER];
	for(int i=MINSIZE; i<=MAXSIZE; i*=STEP){
		for(int j=0;j<i;j++){
			integers[j]=auxiliary[j]=mrand48();
		}
		for(int j=0; j<INDEXNUMBER; j++){
			index[j]= rand()%i; //randomly generate the index of the wanted elements
		}		
		for(int choice=0; choice<METHODSNUMBER; choice++){
			for(int j=0; j<INDEXNUMBER; j++){
				start = clock();
				cout<<select[choice](integers, i, index[j])<<endl;
				end = clock();
				time[choice] += (double) (( end - start )* 1000.0 / CLOCKS_PER_SEC);
				for(int k=0;k<i;k++){
					integers[k]=auxiliary[k];  //change the array back 
				}
			}
			time[choice] /= INDEXNUMBER;
			//output<<METHOSNAMES[choice]<<" with size "<<i<<": "<<time[choice]<<"ms"<<endl;
			time[choice] = 0;
		}
	}
	delete[] integers;	
	output.close();
}
