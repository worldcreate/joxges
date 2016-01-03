#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include "Ga.h"
#include "Gt.h"
#include "Test.h"
#include "Util.h"


#define POPULATION 50
#define MUTATION 1
#define GENERATION 100
#define CHILDNUM 2
#define TRIAL 50
#define SEED 100

using namespace std;

int main(int argc,char *argv[]){
	int i=1;
	int population=POPULATION;
	int mutation=MUTATION;
	int generation=GENERATION;
	int childnum=CHILDNUM;
	int trial=TRIAL;
	int seed=SEED;
	char FileName[256]="probrem/FT10.txt";
	while(argc>i){
		if(argv[i][0]=='-'){
			const char *arg=&argv[i][2];
			switch(argv[i][1]){
				case 'g':
					generation=atoi(arg);
				break;
				case 'p':
					population=atoi(arg);
				break;
				case 'm':
					mutation=atoi(arg);
				break;
				case 'c':
					childnum=atoi(arg);
				break;
				case 't':
					trial=atoi(arg);
				break;
				case 's':
					seed=atoi(arg);
				break;
				case 'f':
					sprintf(FileName,"probrem/%s",arg);
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
		
		Ga ga;
		ga.setPopulation(population);
		ga.setGeneration(generation);
		ga.setMutation(mutation);
		ga.setChildNum(childnum);
		ga.setFileName(FileName);
		ga.execute();
	}
}