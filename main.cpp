#include <iostream>
#include <stdlib.h>
#include "Test.h"
#include "Ges.h"
#include "Util.h"

using namespace std;

int main(int argc,char *argv[]){
	int i=1;
	int seed=300;
	int trial=1;
	while(argc>i){
		if(argv[i][0]=='-'){
			const char *arg=&argv[i][2];
			switch(argv[i][1]){
				case 'T':{
					Test t;
					t.test(argc,argv);
					exit(0);
				}
				break;
				case 't':
					trial=atoi(arg);
				break;
				case 's':
					seed=atoi(arg);
				break;
			}
		}
		i++;
	}
	for(int i=0;i<trial;i++){
		Util::setSeed(seed+i);
		Ges g(argc,argv,i);
		g.execute();
	}
	MemoryManagement::releaseInstance();
}