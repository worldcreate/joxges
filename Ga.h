#ifndef _GA_H_
#define _GA_H_

#include <vector>
#include <time.h>
#include <windows.h>
#include <stdio.h>

using namespace std;

class Individual;

class Ga{
public:
	Ga(int,char*[],int);
	void printPopulation();
	void jox(vector<Individual*>&);
	void execute();
	~Ga();
private:
	void initialize();
	void crossOver();
	void printMinFitness();
	void mutation(Individual*);
	void shiftChange(vector<int>&,int,int);
	void removePopulation(int);
	int mPopulationSize;
	int mMutation;
	int mGeneration;
	int mChildNum;
	char fileName[256];
	vector<Individual*> mPopulation;
	int mArgc;
	char** mArgv;
	FILE *fOut;
};

#endif