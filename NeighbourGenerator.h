#ifndef _NEIGHBOURGENERATOR_H_
#define _NEIGHBOURGENERATOR_H_

#include "JobPair.h"
#include <vector>
#include <utility>

using namespace std;

class Node;

class NeighbourGenerator{
public:
	NeighbourGenerator();
	NeighbourGenerator(vector<vector<JobPair> >&,vector<vector<JobPair> >&);
	void makeNeighbour();
	vector<vector<JobPair> > getNeighbour(int);
	int getNeighbourSize();
private:
	enum ORDER{PREV=-1,CENTER,NEXT};
	vector<vector<JobPair> > m_solution;
	vector<vector<JobPair> > m_SettingTable;
	pair<vector<Node*>,int> m_CriticalPathList;
	vector<vector<vector<JobPair> > > m_NeighbourList;
	void findCriticalPath(Node*,Node*,vector<Node*>&,vector<Node*>&,int);
	vector<vector<JobPair> > changeBackward(const vector<vector<JobPair> >&,JobPair*,JobPair*);
	vector<vector<JobPair> > changeForward(const vector<vector<JobPair> >&,JobPair*,JobPair*);
	void insertBefore(vector<JobPair>&,int,int);
	void insertAfter(vector<JobPair>&,int,int);
	JobPair *findJobFromSetting(JobPair*,enum ORDER);
};

#endif