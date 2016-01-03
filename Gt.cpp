#include <iostream>
#include <limits.h>
#include "Gt.h"
#include "Util.h"
#include "FileReader.h"

#ifndef NULL
#define NULL 0
#endif

using namespace std;

Gt::Gt():mFix(false){
}

Gt::Gt(const char* filename):mFix(false){
	FileReader fr(filename);
	Table table=fr.getTable();
	setTable(table);
}

/* setTable(Table &)							 */
/* JSPテーブルの設定							 */
/* Pair<int,int>の二次元配列(vector)を引数		 */
/* とする										 */
/* vector[job][order]とし、pair<machine,time>	 */
/* とする										 */
void Gt::setTable(Table &table){
	mTable=table;
	mJobNum=table.size();
	mMachineNum=table[0].size();
}

/* execute()									 */
/* 設定されたテーブルを元にAS(Active 			 */
/* Schedule)を作り出す							 */
void Gt::execute(){
	// step1
	vector<vector<int> > firstIndex(mMachineNum,vector<int>(mJobNum,-1));
	for(int j=0;j<mJobNum;j++){
		JobPair jp=mTable[j][0];
		firstIndex[jp.machine][j]=jp.time;
	}
	mCreateTable.push_back(firstIndex);
	int index=0;
	pair<int,int> T;
	T.first=0;
	T.second=-1;
	while(true){
		// step2
		int machine=0;
		machine=getMinTimeOverT(mCreateTable[index],T);
		if(machine==-1)
			break;
		// step3
		// step4
		pair<int,int> TT=T;
		fixConflict(index,machine,TT);
		setNextJobpair(index,machine,TT);
			
		addNextIndexTable(index);
		index++;

	}
}

/* getMinTimeOverT(const vector<vector<int>>&,int*)	 */
/* ASの生成テーブルを引数とし、Tよりは大きく、最小の */
/* T'を取得する										 */
/* 返り値としては、取得したT'のmachineを返す		 */
int Gt::getMinTimeOverT(const vector<vector<int> > &index,pair<int,int> &T){
	/* TODO 修正する必要あり？ */
	pair<int,int> tempT;
	tempT.first=INT_MAX;
	tempT.second=-1;
	int machine=-1;
	for(int m=0;m<mMachineNum;m++){
		for(int j=0;j<mJobNum;j++){
			if(tempT.first<index[m][j])
				continue;
			if(T.first>index[m][j])
				continue;
			if(findJobpairByMachineAndJob(m,j,NOWJOBPAIR)->isCheck())
				continue;
			tempT.first=index[m][j];
			tempT.second=j;
			machine=m;
		}
	}
	T=tempT;

	return machine;
}

/* checkConflict(int ,int ,const int)				 */
/* index,machineを引数とし、AS生成テーブルの中の同一 */
/* machineの中がコンフリクトを起こしているかを		 */
/* チェックする										 */
bool Gt::checkConflict(int index,int machine,pair<int,int> &T){
	vector<int> jobTable=mCreateTable[index][machine];
	vector<JobPair> sameT;
	vector<JobPair> differT;
	vector<JobPair> c;
	// 同じTをsameTに代入
	for(int i=0;i<jobTable.size();i++){
		if(findJobpairByMachineAndJob(machine,i,NOWJOBPAIR)->isCheck())
			continue;
		JobPair jp;
		jp.jobIndex=i;
		jp.time=jobTable[i];
		if(T.first==jp.time){
			sameT.push_back(jp);
		}else if(T.first<jp.time){
			differT.push_back(jp);
		}
	}

	// それぞれのsameTに対してコンフリクトを起こしているかを確認
	for(int j=0;j<differT.size();j++){
		if(findJobpairByMachineAndJob(machine,j,NOWJOBPAIR)->isCheck())
			continue;
		if(differT[j].time-
			findJobpairByMachineAndJob(
				machine,differT[j].jobIndex,NOWJOBPAIR)->time <T.first
			){
			c.push_back(differT[j]);
		}
	}
	c.insert(c.end(),sameT.begin(),sameT.end());
	if(c.size()>1){
		return true;
	}
	return false;
}

/* fixConflict(int,int,int)							 */
/* コンフリクトを起こしていた場合、修正する			 */
void Gt::fixConflict(int index,int machine,pair<int,int> &T){
	vector<int> jobTable=mCreateTable[index][machine];
	vector<JobPair> c;
	vector<JobPair> sameT;
	vector<JobPair> differT;
	// 同じTをsameTに代入
	for(int i=0;i<jobTable.size();i++){
		if(findJobpairByMachineAndJob(machine,i,NOWJOBPAIR)->isCheck())
			continue;
		JobPair jp;
		jp.jobIndex=i;
		jp.time=jobTable[i];
		if(T.first==jobTable[i]){
			sameT.push_back(jp);
		}else if(T.first<jp.time){
			differT.push_back(jp);
		}
	}

	// それぞれのsameTに対してコンフリクトを起こしているかを確認
	for(int j=0;j<differT.size();j++){
		if(findJobpairByMachineAndJob(machine,j,NOWJOBPAIR)->isCheck())
			continue;
		if(differT[j].time-
			findJobpairByMachineAndJob(
				machine,differT[j].jobIndex,NOWJOBPAIR)->time <T.first
			){
			c.push_back(differT[j]);
		}
	}
	c.insert(c.end(),sameT.begin(),sameT.end());


	int r=-1;
	if(!mFix){
		r=Util::getRand(0,c.size()-1);

	}else{
		/* TODO */
		/* matrixの中から未処理で先頭のものを一つとってきて、
		コンフリクトの中にあるか調べる。
		あった場合、それを選択して、なかった場合、コンフリクトの中にあるものを
		ランダムで選択し、matrixの順序を入れ替える。
		*/
		JobPair jp=getFirstNotCheckJobPairFromMatrix(machine);
		bool find=false;
		for(int i=0;i<c.size();i++){
			if(c[i].jobIndex!=jp.jobIndex)
				continue;
			r=i;
			find=true;
			break;
		}
		if(!find){
			r=Util::getRand(0,c.size()-1);
			changeOrderJobPairMatrix(machine,c[r].jobIndex,jp.jobIndex);
		}
	}
	// スケジュール済みの中から最大のTを探す
	int scheduledT=0;
	for(int j=0;j<mJobNum;j++){
		if(!findJobpairByMachineAndJob(machine,j,NOWJOBPAIR)->isCheck())
			continue;
		if(scheduledT<jobTable[j])
			scheduledT=jobTable[j];
	}
	//
	// 選択されたjobの前の完了時間を取得
	JobPair* beforeJP=findJobpairByMachineAndJob(machine,c[r].jobIndex,PREVJOBPAIR);
	int beforeT=0;
	if(beforeJP!=NULL){
		beforeT=mCreateTable[index][beforeJP->machine][c[r].jobIndex];
	}
	//
	T.first=max(beforeT,scheduledT)+findJobpairByMachineAndJob(machine,c[r].jobIndex,NOWJOBPAIR)->time;
	T.second=c[r].jobIndex;
	mCreateTable[index][machine][c[r].jobIndex]=T.first;
	for(int j=0;j<c.size();j++){
		if(j==r)
			continue;
		JobPair* prevJobPair;
		int prevT=0;
		int TT=0;
		prevJobPair=findJobpairByMachineAndJob(machine,c[j].jobIndex,PREVJOBPAIR);

		if(prevJobPair!=NULL){
			prevT=mCreateTable[index][prevJobPair->machine][c[j].jobIndex];
		}
		TT=max(prevT,T.first);
		mCreateTable[index][machine][c[j].jobIndex]=
			findJobpairByMachineAndJob(machine,c[j].jobIndex,NOWJOBPAIR)->time+TT;
	}
}

/* setNextJobpair(int,int,int)							 */
/* 作業iの技術的順序により、次に割り当てられるJobPair	 */
/* をASに記入する										 */
void Gt::setNextJobpair(int index,int machine,pair<int,int> T){
	int jobIndex=T.second;
	findJobpairByMachineAndJob(machine,jobIndex,NOWJOBPAIR)->check();
	if(mFix)
		findJobpairByMachineAndJobFromMatrix(machine,jobIndex).check();
	JobPair* jp=findJobpairByMachineAndJob(machine,jobIndex,NEXTJOBPAIR);
	if(jp==NULL)
		return;
	
	int emptyTime=0;
	int nextMachine=jp->machine;
	vector<int> nextJobTable=mCreateTable[index][nextMachine];
	
	for(int j=0;j<mJobNum;j++){
		if(!findJobpairByMachineAndJob(nextMachine,j,NOWJOBPAIR)->isCheck())
			continue;
		if(emptyTime<nextJobTable[j]){
			emptyTime=nextJobTable[j];
		}
	}
	int Ti=mCreateTable[index][machine][T.second];
	int Tk=emptyTime;
	int TT=max(Ti,Tk);
	nextJobTable[jobIndex]=TT+jp->time;
	mCreateTable[index][nextMachine]=nextJobTable;
}

/* addNextIndexTable(int)								 */
/* 次の生成テーブルを追加する							 */
void Gt::addNextIndexTable(int index){
	vector<vector<int> > nextIndex;
	nextIndex=mCreateTable[index];
	mCreateTable.push_back(nextIndex);
}

/* findJobpairByMachineAndJob(int,int,int)				 */
/* machineとjobにより設定テーブルよりJobPairを見つける	 */
/* orderを指定することにより、次の処理(NEXTJOBPAIR)や	 */
/* 前の処理(PREVJOBPAIR)と使い分ける					 */
JobPair* Gt::findJobpairByMachineAndJob(int machine,int jobIndex,int order){
	for(int o=0;o<mMachineNum;o++){
		if(mTable[jobIndex][o].machine!=machine)
			continue;
		if(o+order<0 || o+order>mMachineNum-1){
			return NULL;
		}
		return &mTable[jobIndex][o+order];
	}
}

/* getASTable()											 */
/* ASが生成完了した際の最終的なテーブルを取得する		 */
vector<vector<int> > Gt::getASTable(){
	int index=mCreateTable.size();
	return mCreateTable[index-1];
}

/* getMatrix()											 */
/* AStableをMatrixに変換								 */
/* 縦Machine,横Machineの投入順序の二次元配列			 */
vector<vector<int> > Gt::convertAStoMatrix(const vector<vector<int> > &AS){
	vector<vector<int> > Matrix=vector<vector<int> >(AS.size(),vector<int>(AS[0].size(),0));
	for(int m=0;m<AS.size();m++){
		int T=0;
		for(int j=0;j<AS[0].size();j++){
			int tempT=INT_MAX;
			int tempJ=-1;
			for(int k=0;k<AS[0].size();k++){
				if(tempT>AS[m][k] && T<AS[m][k]){
					tempT=AS[m][k];
					tempJ=k;
				}
			}
			Matrix[m][j]=tempJ;
			T=tempT;
		}
	}
	return Matrix;
}

vector<vector<int> > Gt::getMatrix(){
	vector<vector<int> > matrix;
	matrix=getASTable();
	matrix=convertAStoMatrix(matrix);
	return matrix;
}

vector<vector<int> > Gt::fixMatrix(vector<vector<int> > matrix){
	mFix=true;
	for(int i=0;i<matrix.size();i++){
		vector<JobPair> a;
		for(int j=0;j<matrix[0].size();j++){
			JobPair jp;
			jp.jobIndex=matrix[i][j];
			jp.machine=i;
			a.push_back(jp);
		}
		mMatrix.push_back(a);
	}
	execute();
	vector<vector<int> > table=getASTable();
	table=convertAStoMatrix(table);
	return table;
}

int Gt::getMakespan(){
	vector<vector<int> > ASTable=getASTable();
	int T=0;
	for(int i=0;i<ASTable.size();i++){
		for(int j=0;j<ASTable[0].size();j++){
			if(T<ASTable[i][j])
				T=ASTable[i][j];
		}
	}
	return T;
}

JobPair& Gt::findJobpairByMachineAndJobFromMatrix(int machine,int jobIndex){
	for(int i=0;i<mMatrix.size();i++){
		for(int j=0;j<mMatrix[0].size();j++){
			if(mMatrix[i][j].machine!=machine)
				continue;
			if(mMatrix[i][j].jobIndex!=jobIndex)
				continue;
			return mMatrix[i][j];
		}
	}
}

JobPair& Gt::getFirstNotCheckJobPairFromMatrix(int machine){
	for(int j=0;j<mMatrix[0].size();j++){
		if(mMatrix[machine][j].isCheck())
			continue;
		return mMatrix[machine][j];
	}
}

void Gt::changeOrderJobPairMatrix(int machine,int src,int dst){
	int p=-1,q=-1;
	for(int i=0;i<mMatrix[machine].size();i++){
		if(mMatrix[machine][i].jobIndex==src)
			p=i;
		if(mMatrix[machine][i].jobIndex==dst)
			q=i;
	}
	JobPair t=mMatrix[machine][p];
	mMatrix[machine][p]=mMatrix[machine][q];
	mMatrix[machine][q]=t;
}