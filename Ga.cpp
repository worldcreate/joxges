#include "Ga.h"
#include "Individual.h"
#include "Util.h"
#include <algorithm>
#include <limits.h>
#include <math.h>
#include <stdio.h>

Ga::Ga(){
}

void Ga::setPopulation(int x){
	mPopulationSize=x;
}

void Ga::setMutation(int x){
	mMutation=x;
}

void Ga::setGeneration(int x){
	mGeneration=x;
}

void Ga::setChildNum(int x){
	mChildNum=x;
}

void Ga::setFileName(char *name){
	strcpy(fileName,name);
}

void Ga::execute(){
	int g=0;
	initialize();
	
	while(g<mGeneration){
		crossOver();
		g++;
	}

	printMinFitness();
}

void Ga::printPopulation(){
	for(int i=0;i<mPopulation.size();i++){
		mPopulation[i]->print();
	}
}

void Ga::initialize(){
	for(int i=0;i<mPopulationSize;i++){
		Individual* ind=new Individual();
		ind->initGene(fileName);
		mPopulation.push_back(ind);
	}
}

void Ga::crossOver(){
	random_shuffle(mPopulation.begin(),mPopulation.end());
	vector<Individual*> family;
	int r=Util::getRand(0,mPopulation.size()-1);
	family.push_back(mPopulation[r]);
	removePopulation(r);
	r=Util::getRand(0,mPopulation.size()-1);
	family.push_back(mPopulation[r]);
	removePopulation(r);
	jox(family);
	#ifdef DEBUG
		printf("family.size()=%d\n",family.size());
		for(int i=0;i<family.size();i++){
			printf("family[%d]\n",i);
			family[i]->print();
		}
	#endif
	// 子供を母集団と同じ個体にしないための操作
	int count=0;
	int index=0;
	int hist[2];
	while(true){
		bool ret=true;
		for(int i=0;i<mPopulation.size();i++){
			#ifdef DEBUG
				printf("family[%d]\n",index);
				family[index]->print();
				printf("mPopulation[%d]\n",i);
				mPopulation[i]->print();
			#endif
			if((*family[index])==(*mPopulation[i])){
				ret=false;
				break;
			}
		}
		if(ret){
			mPopulation.push_back(family[index]);
			hist[count]=index;
			count++;
			if(count>1){
				break;
			}
		}
		index++;
		if(index>family.size()-1){
			for(int i=count;i<2;i++){
				int k=0;
				while(true){
					bool isFind=false;
					for(int j=0;j<count;j++){
						if(hist[j]==k)
							isFind=true;
					}
					if(!isFind){
						mPopulation.push_back(family[k]);
						hist[i]=k;
						count++;
						break;
					}
					k++;
				}
			}
			break;
		}
	}
	for(int i=0;i<family.size();i++){
		if(i==hist[0])
			continue;
		if(i==hist[1])
			continue;
		delete(family[i]);
	}
}

void Ga::jox(vector<Individual*> &family){
	#ifdef DEBUG
		cout<<"parent1"<<endl;
		family[0]->print();
		cout<<"parent2"<<endl;
		family[1]->print();
		cout<<"================"<<endl;
	#endif
	for(int i=0;i<mChildNum;i+=2){
		vector<int> saveJob;
		for(int j=0;j<family[0]->getJobNum();j++){
			int r=Util::getRand(0,1);
			if(r==1)
				continue;
			saveJob.push_back(j);
		}
		#ifdef DEBUG
			cout<<"saveJob"<<endl;
			for(int j=0;j<saveJob.size();j++)
				cout<<saveJob[j]<<" ";
			cout<<endl;
			cout<<"======================"<<endl;
		#endif
		Individual *c1=new Individual(family[0]->getMachineNum(),family[0]->getJobNum());
		Individual *c2=new Individual(family[0]->getMachineNum(),family[0]->getJobNum());
		for(int m=0;m<family[0]->getMachineNum();m++){
			vector<int> notSaveJob1,notSaveJob2;
			for(int j=0;j<family[0]->getJobNum();j++){
				int value=(*family[0])[m][j];
				if(Util::includeVector(saveJob,value)){
					(*c1)[m][j]=value;
				}else{
					notSaveJob1.push_back(value);
				}
				value=(*family[1])[m][j];
				if(Util::includeVector(saveJob,value)){
					(*c2)[m][j]=value;
				}else{
					notSaveJob2.push_back(value);
				}
			}
			for(int j=0,k=0,l=0;j<family[0]->getJobNum();j++){
				if((*c1)[m][j]==-1){
					(*c1)[m][j]=notSaveJob2[k];
					k++;
				}
				if((*c2)[m][j]==-1){
					(*c2)[m][j]=notSaveJob1[l];
					l++;
				}
			}
		}
		#ifdef DEBUG
			cout<<"before mutation"<<endl;
			cout<<"c1"<<endl;
			for(int i=0;i<c1->getMachineNum();i++){
				for(int j=0;j<c1->getJobNum();j++){
					cout<<(*c1)[i][j]<<" ";
				}
				cout<<endl;
			}
			cout<<"c2"<<endl;
			for(int i=0;i<c2->getMachineNum();i++){
				for(int j=0;j<c2->getJobNum();j++){
					cout<<(*c2)[i][j]<<" ";
				}
				cout<<endl;
			}
			cout<<"==============================="<<endl;
		#endif
		
		mutation(c1);
		mutation(c2);
		
		#ifdef DEBUG
			cout<<"after mutation"<<endl;
			cout<<"c1"<<endl;
			for(int i=0;i<c1->getMachineNum();i++){
				for(int j=0;j<c1->getJobNum();j++){
					cout<<(*c1)[i][j]<<" ";
				}
				cout<<endl;
			}
			cout<<"c2"<<endl;
			for(int i=0;i<c2->getMachineNum();i++){
				for(int j=0;j<c2->getJobNum();j++){
					cout<<(*c2)[i][j]<<" ";
				}
				cout<<endl;
			}
			cout<<"==============================="<<endl;
		#endif
		c1->fixGene(fileName);
		c2->fixGene(fileName);

		family.push_back(c1);
		family.push_back(c2);
	}
	
	sort(family.begin(),family.end(),Individual::less);
	#ifdef DEBUG
		cout<<"fitness"<<endl;
		for(int i=0;i<family.size();i++){
			family[i]->print();
		}
		cout<<"============"<<endl;
	#endif
		
}

void Ga::mutation(Individual* individual){
	int r=Util::getRand(1,100);
	if(r<=mMutation){
		#ifdef DEBUG
			cout<<endl;
			cout<<"mutation"<<endl;
			cout<<endl;
		#endif
		int job=Util::getRand(0,individual->getJobNum()-1);
		for(int m=0;m<individual->getMachineNum();m++){
			vector<int> order;
			int jobIndex;
			for(int j=0;j<individual->getJobNum();j++){
				if((*individual)[m][j]==job){
					jobIndex=j;
					continue;
				}
				order.push_back(j);
			}

			int s=Util::getRand(0,order.size()-1);
			shiftChange((*individual)[m],jobIndex,order[s]);
			#ifdef DEBUG
				cout<<"job="<<job<<endl;
				cout<<"jobIndex="<<jobIndex<<endl;
				cout<<"order[s]="<<order[s]<<endl;
			#endif
		}
	}
}

void Ga::shiftChange(vector<int> &vec,int src,int dst){
	int value=vec[src];
	if(src<dst){
		for(int i=src;i<dst;i++){
			vec[i]=vec[i+1];
		}
		vec[dst]=value;
	}else{
		for(int i=src-1;i>=dst;i--){
			vec[i+1]=vec[i];
		}
		vec[dst]=value;
	}
}

void Ga::printMinFitness(){
	int temp=INT_MAX;
	double ave=0;
	double variance=0;
	for(int i=0;i<mPopulationSize;i++){
		int t=mPopulation[i]->getFitness();
		ave+=mPopulation[i]->getFitness();
		if(temp>t){
			temp=t;
		}
	}
	ave/=mPopulationSize;
	for(int i=0;i<mPopulationSize;i++){
		variance+=pow(mPopulation[i]->getFitness()-ave,2);
	}
	variance/=mPopulationSize;
	cout<<"min="<<temp;
	cout<<",variance="<<variance<<endl;
}

void Ga::removePopulation(int tar){
	vector<Individual*>::iterator it=mPopulation.begin();
	for(int i=0;it!=mPopulation.end();it++,i++){
		if(i==tar){
			mPopulation.erase(it);
			break;
		}
	}
}

Ga::~Ga(){
	for(int i=0;i<mPopulation.size();i++){
		delete(mPopulation[i]);
	}
}