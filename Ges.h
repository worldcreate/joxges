#ifndef _GES_H_
#define _GES_H_

#include <list>
#include <vector>
#include <stack>
#include <deque>
#include <stdio.h>
#include "JobPair.h"
#include "Node.h"
#include "Graph.h"

using namespace std;

class Ges{
public:
	Ges(int ,char **);
	void initialSolution();
	void setSolution(vector<vector<int> >&);
	vector<vector<int> > getSolution();
	void execute();
	~Ges();
private:
	void Routine(vector<vector<JobPair> >&,int);
	void Ejection(vector<vector<JobPair> >,vector<vector<JobPair> >&,int);
	void Ejection(Graph,deque<Node*>,vector<JobPair>,vector<vector<JobPair> >&,int,int);
	vector<JobPair> selectEP(vector<vector<JobPair> >&);
	void Perturb(vector<vector<JobPair> >&,int);
	void insertJob(vector<vector<JobPair> >&,JobPair &,int);
	void LocalSearch(vector<vector<JobPair> >&);
	bool tabuCheck(deque<vector<vector<JobPair> > >&,vector<vector<JobPair> >&,vector<vector<JobPair> >&);
	void addTabuList(deque<vector<vector<JobPair> > >&,vector<vector<JobPair> >&);
	void excessiveEject(vector<vector<JobPair> >&,int);
	void removeSolution(vector<vector<JobPair> >&,vector<Node*>&,vector<JobPair>&);
	void bottleneckSort(vector<Node*>&);

	int m_Iter;
	int m_MaxIter;
	int m_IterRand;
	int m_stagLS;
	int m_maxT;
	int m_GESMode;
	vector<int> m_Penalty;
	vector<vector<JobPair> > m_Solution;	// 縦Machine 横投入順序の配列
	stack<JobPair> m_EP;
	char m_FileName[256];
	vector<vector<JobPair> > m_SettingTable;	// 縦Job 横技術的順序
	int m_kMax;
	FILE *fOut;
};

#endif