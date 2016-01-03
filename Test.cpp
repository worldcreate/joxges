#include "Test.h"
#include "Ga.h"
#include "Gt.h"
#include "Individual.h"
#include "Util.h"
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <limits.h>

using namespace std;

#define N 10000

Test::Test(){

}

void Test::test(){
	int min=INT_MAX;
	int avg=0;
	for(int i=0;i<N;i++){
		Individual ind;
		//ind.initGene();
		int fit=ind.getFitness();
		avg+=fit;
		if(fit<min){
			min=fit;
		}
	}
	cout<<"min="<<min<<endl;
	cout<<"avg="<<avg/N<<endl;
}
