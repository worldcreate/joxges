#include "NeighbourGenerator.h"
#include "Graph.h"
#include "Node.h"

NeighbourGenerator::NeighbourGenerator(){
}

NeighbourGenerator::NeighbourGenerator(vector<vector<JobPair> >& a_solution,vector<vector<JobPair> >& a_SettingTable){
	m_solution=a_solution;
	m_SettingTable=a_SettingTable;
	m_CriticalPathList=pair<vector<Node*>,int>(vector<Node*>(),-1);
}

void NeighbourGenerator::makeNeighbour(){
	vector<Node*> bottleneck;
	Graph g(m_solution,m_SettingTable);
	g.setLongestPath();
	int L=g.getMakespan();
	for(int i=0;i<g.size();i++){
		if(g[i]->m_R+g[i]->m_Q-g[i]->m_Jobpair->time==L){
			bottleneck.push_back(g[i]);
		}
	}

	m_CriticalPathList=pair<vector<Node*>,int>(vector<Node*>(),-1);
	vector<Node*> criticalPath;
	findCriticalPath(g[0],g[g.size()-1],bottleneck,criticalPath,0);

	criticalPath.clear();
	criticalPath=m_CriticalPathList.first;
	for(int j=0;j<criticalPath.size()-1;j++){
		for(int k=j+1;k<criticalPath.size()-1;k++){
			if(criticalPath[j]->m_Jobpair->machine!=
				criticalPath[k]->m_Jobpair->machine)
				continue;
			JobPair *I=criticalPath[j]->m_Jobpair;
			JobPair *J=criticalPath[k]->m_Jobpair;
			JobPair *alphaI=findJobFromSetting(I,PREV);
			JobPair *gammaI=findJobFromSetting(I,NEXT);
			JobPair *alphaJ=findJobFromSetting(J,PREV);
			JobPair *gammaJ=findJobFromSetting(J,NEXT);

			for(int l=0;l<criticalPath.size();l++){
				// gammaJがCriticalPathに含まれていればforwardchangeする
				if(gammaJ!=NULL && gammaI!=NULL && gammaJ->index==criticalPath[l]->m_Jobpair->index &&
					g.getNodeByIndex(J->index)->m_Q-J->time>=g.getNodeByIndex(gammaI->index)->m_Q-gammaI->time){
					vector<vector<JobPair> > forwardSolution;
					forwardSolution=changeForward(m_solution,criticalPath[j]->m_Jobpair,criticalPath[k]->m_Jobpair);
					m_NeighbourList.push_back(forwardSolution);
					Graph forward(forwardSolution,m_SettingTable);
					forward.setLongestPath();
					if(g.getMakespan()>forward.getMakespan()){
					}else{
						if(forward.getNodeByIndex(J->index)->m_R-J->time<=g.getNodeByIndex(J->index)->m_R-J->time-I->time){
						}
						if(forward.getNodeByIndex(J->index)->m_Q<=g.getNodeByIndex(J->index)->m_Q+I->time){
						}
					}
				}
				
				// alphaIがCriticalPathに含まれていればbackwardchangeする
				if(alphaI!=NULL && alphaJ!=NULL && alphaI->index==criticalPath[l]->m_Jobpair->index &&
					g.getNodeByIndex(I->index)->m_R>=g.getNodeByIndex(alphaJ->index)->m_R){
					vector<vector<JobPair> > backwardSolution;
					backwardSolution=changeBackward(m_solution,criticalPath[j]->m_Jobpair,criticalPath[k]->m_Jobpair);
					m_NeighbourList.push_back(backwardSolution);

					Graph backward(backwardSolution,m_SettingTable);
					backward.setLongestPath();
					if(g.getMakespan()>backward.getMakespan()){
					}else{
						if(backward.getNodeByIndex(I->index)->m_Q<=g.getNodeByIndex(I->index)->m_Q-J->time){
						}
						if(backward.getNodeByIndex(I->index)->m_R-I->time<=g.getNodeByIndex(I->index)->m_R-I->time+J->time){
						}
					}
				}
			}
		}
	}
}

void NeighbourGenerator::findCriticalPath(Node* node,Node* leaf,vector<Node*>& bottleneck,vector<Node*>& criticalPath,int length){
	if(node==leaf){
		criticalPath.push_back(leaf);
		if(m_CriticalPathList.second<length){
			m_CriticalPathList=pair<vector<Node*>,int>(criticalPath,length);
		}
		return;
	}
	for(int i=0;i<node->m_Next.size();i++){
		vector<Node*> _criticalPath=criticalPath;
		for(int j=0;j<bottleneck.size();j++){
			if(node->m_Next[i]->m_Jobpair->index==bottleneck[j]->m_Jobpair->index){
				_criticalPath.push_back(node);
				findCriticalPath(node->m_Next[i],leaf,bottleneck,_criticalPath,length+node->m_Jobpair->time);
				break;
			}
		}
	}
}

vector<vector<JobPair> > NeighbourGenerator::changeBackward(const vector<vector<JobPair> >& solution,JobPair* I,JobPair* J){
	// iの前にj
	vector<vector<JobPair> >_solution=solution;
	int machine=I->machine;
	int iIndex=-1,jIndex=-1;
	for(int i=0;i<solution[machine].size();i++){
		if(solution[machine][i].index==I->index){
			iIndex=i;
		}
		if(solution[machine][i].index==J->index){
			jIndex=i;
		}
	}

	insertBefore(_solution[machine],iIndex,jIndex);
	return _solution;
}

vector<vector<JobPair> > NeighbourGenerator::changeForward(const vector<vector<JobPair> >& solution,JobPair* I,JobPair* J){
	// iをjの後に
	vector<vector<JobPair> >_solution=solution;
	int machine=I->machine;
	int iIndex=-1,jIndex=-1;
	for(int i=0;i<solution[machine].size();i++){
		if(solution[machine][i].index==I->index){
			iIndex=i;
		}
		if(solution[machine][i].index==J->index){
			jIndex=i;
		}
	}

	insertAfter(_solution[machine],jIndex,iIndex);
	return _solution;
}

vector<vector<JobPair> > NeighbourGenerator::getNeighbour(int index){
	return m_NeighbourList[index];
}

int NeighbourGenerator::getNeighbourSize(){
	return m_NeighbourList.size();
}

void NeighbourGenerator::insertBefore(vector<JobPair>& solution,int tar,int src){
	vector<JobPair>::iterator it;
	JobPair jp;
	int i;
	for(i=0,it=solution.begin();it!=solution.end();it++,i++)
		if(i==src){
			jp=solution[i];
			break;
		}
	solution.erase(it);
	
	for(i=0,it=solution.begin();it!=solution.end();it++,i++)
		if(i==tar){
			break;
		}
	solution.insert(it,jp);

}

void NeighbourGenerator::insertAfter(vector<JobPair>& solution,int tar,int src){
	vector<JobPair>::iterator it;
	JobPair jp;
	int i;
	for(i=0,it=solution.begin();it!=solution.end();it++,i++)
		if(i==src){
			jp=solution[i];
			break;
		}
	solution.erase(it);
	
	for(i=0,it=solution.begin();it!=solution.end();it++,i++)
		if(i==tar){
			break;
		}
	solution.insert(it,jp);
}

JobPair* NeighbourGenerator::findJobFromSetting(JobPair* jp,enum ORDER order){
	int jobIndex=jp->jobIndex;
	for(int i=0;i<m_SettingTable[jobIndex].size();i++){
		if(m_SettingTable[jobIndex][i].index!=jp->index)
			continue;
		if(i==0 && order==PREV){
			return NULL;
		}
		if(i==m_SettingTable[jobIndex].size()-1 && order==NEXT){
			return NULL;
		}
		return &m_SettingTable[jobIndex][i+order];
	}
}