#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include "Ga.h"
#include "Gt.h"
#include "Test.h"
#include "Util.h"

#define TRIAL 50
#define SEED 100

using namespace std;

int main(int argc,char *argv[]){
	int i=1;
	int trial=TRIAL;
	int seed=SEED;
	while(argc>i){
		if(argv[i][0]=='-'){
			const char *arg=&argv[i][2];
			switch(argv[i][1]){
				case 't':
					trial=atoi(arg);
				break;
				case 's':
					seed=atoi(arg);
				break;
				case 'h':
					printf("Usage Jox [OPTION]\n");
					printf("  -g\t generation num\n");
					printf("  -p\t population num\n");
					printf("  -m\t mutation rate\n");
					printf("  -c\t make child num\n");
					printf("  -t\t trial num\n");
					printf("  -s\t random seed\n");
					printf("  -f\t setting file name\n");
					exit(0);
				break;
			}
		}
		i++;
	}
	
	for(int i=0;i<trial;i++){
		Util::setSeed(SEED+i);
		
		Ga ga(argc,argv);
		ga.execute();
	}
}