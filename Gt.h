#ifndef _GT_H_
#define _GT_H_

#include <vector>
#include <map>
#include "JobPair.h"

using namespace std;

class Gt{
public:
	typedef vector<vector<JobPair> > Table;
	
	Gt();
	Gt(const char*);
	void setTable(Table&);
	void execute();
	vector<vector<int> > getASTable();
	vector<vector<int> > convertAStoMatrix(const vector<vector<int> >&);
	vector<vector<int> > getMatrix();
	vector<vector<int> > fixMatrix(vector<vector<int> > );
	int getMakespan();
private:
	
	Table mTable;
	vector<vector<vector<int> > > mCreateTable;
	vector<vector<JobPair> > mMatrix;
	int mJobNum;
	int mMachineNum;
	bool mFix;

	int getMinTimeOverT(const vector<vector<int> > &,pair<int,int>&);
	bool checkConflict(int,int,pair<int,int>&);
	void fixConflict(int,int,pair<int,int>&);
	void setNextJobpair(int,int,pair<int,int>);
	void addNextIndexTable(int);
	JobPair* findJobpairByMachineAndJob(int,int,int);
	JobPair& findJobpairByMachineAndJobFromMatrix(int,int);
	JobPair& getFirstNotCheckJobPairFromMatrix(int);
	void changeOrderJobPairMatrix(int,int,int);
	enum {PREVJOBPAIR=-1,NOWJOBPAIR,NEXTJOBPAIR};
};

#endif