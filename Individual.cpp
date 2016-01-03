#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "Individual.h"
#include "Gt.h"

Individual::Individual():mFitness(0){

}

Individual::Individual(int machineNum,int jobNum):mFitness(0){
	mGene=vector<vector<int> >(machineNum,vector<int>(jobNum,-1));
}

void Individual::initGene(const char *fileName){
	Gt gt(fileName);
	gt.execute();
	mGene=gt.convertAStoMatrix(gt.getASTable());
	check();
	mFitness=gt.getMakespan();
}

void Individual::fixGene(const char *fileName){
	Gt gt(fileName);
	vector<vector<int> >buffer;
	mGene=gt.fixMatrix(mGene);
	check();
	mFitness=gt.getMakespan();
}

void Individual::check(){
	// Check Matrix
	for(int i=0;i<mGene.size();i++){
		for(int j=0;j<mGene[0].size();j++){
			if(mGene[i][j]==-1){
				cout<<"error!!!"<<endl;
				print(mGene);
				exit(-1);
			}
		}
	}
	//
}

void Individual::print(){
	for(int i=0;i<mGene.size();i++){
		for(int j=0;j<mGene[0].size();j++){
			cout<<mGene[i][j]<<" ";
		}
		cout<<endl;
	}
	cout<<"fitness="<<mFitness<<endl;
	cout<<endl;
}

void Individual::print(vector<vector<int> > &vec){
	for(int i=0;i<vec.size();i++){
		for(int j=0;j<vec[0].size();j++){
			cout<<vec[i][j]<<" ";
		}
		cout<<endl;
	}
	cout<<endl;
}

int Individual::getJobNum(){
	return mGene[0].size();
}

int Individual::getMachineNum(){
	return mGene.size();
}

int Individual::getFitness(){
	return mFitness;
}

vector<int>& Individual::operator[](int n){
	return mGene[n];
}

bool Individual::operator<(Individual& obj){
	return this->mFitness<obj.getFitness();
}

bool Individual::operator>(Individual& obj){
	return this->mFitness>obj.getFitness();
}

bool Individual::less(Individual *l, Individual *r){
	return l->getFitness()<r->getFitness();
}

bool Individual::operator==(Individual& dst){
	for(int i=0;i<mGene.size();i++){
		for(int j=0;j<mGene.size();j++){
			if(mGene[i][j]!=dst[i][j])
				return false;
		}
	}
	return true;
}

bool Individual::operator!=(Individual& dst){
	return !(this->operator==(dst));
}

Individual::~Individual(){
	
}