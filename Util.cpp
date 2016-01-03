#include "util.h"
#include <time.h>
#include <stdlib.h>
#include <iostream>

int Util::getRand(int min,int max){
	return (int)(((double)rand() + 1.0) / ((double)RAND_MAX + 2.0)*(max - min + 1)) + min;
}

void Util::setSeed(int n){
	srand(n);
}