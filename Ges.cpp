#include "Ges.h"
#include "FileReader.h"
#include "Gt.h"
#include "Util.h"
#include "NeighbourGenerator.h"
#include "MemoryManagement.h"
#include <algorithm>
#include <iostream>
#include <climits>
#include <stdexcept>
#include <cstdio>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

Ges::Ges(int argc,char **argv,int trial){
	m_Iter=0;
	m_MaxIter=100;
	m_IterRand=100;
	strcpy(m_FileName,"probrem/FT3.txt");
	m_kMax=3;
	m_stagLS=50;
	m_maxT=10;
	m_GESMode=1;
	fOut=stdout;
	char outName[256]="";
	char outArg[256]="";
	char outFile[256];
	int i=1;
	struct stat stat_buf;
	while(argc>i){
		if(argv[i][0]=='-'){
			const char *arg=&argv[i][2];
			switch(argv[i][1]){
				case 'f':
					sprintf(m_FileName,"probrem/%s",arg);
				break;
				case 'i':
					m_MaxIter=atoi(arg);
				break;
				case 'r':
					m_IterRand=atoi(arg);
				break;
				case 'k':
					m_kMax=atoi(arg);
				break;
				case 'G':
					m_GESMode=atoi(arg);
				break;
				case 'o':
					strcpy(outName,arg);
				continue;
				// case 's':
				// 	m_stagLS=atoi(arg);
				// break;
			}
			sprintf(outArg,"%s_%s",outArg,&argv[i][1]);
		}
		i++;
	}
	if(strcmp(outName,"")!=0){
		sprintf(outFile,"data/%s_%d.txt",outName,trial);
	}else{
		sprintf(outFile,"data/%s_%d.txt",outArg,trial);
	}
	if(stat("./data",&stat_buf)==-1){
		mkdir("data",0755);
	}
	fOut=fopen(outFile,"w");

	FileReader *fr=FileReader::getInstance(m_FileName);
	m_SettingTable=fr->getTable();

	for(int i=0;i<m_SettingTable.size()*m_SettingTable[0].size()+2;i++)
		m_Penalty.push_back(1);

	initialSolution();
}

void Ges::initialSolution(){
	Gt gt(m_FileName);
	gt.execute();
	vector<vector<int> > matrix=gt.getMatrix();

	m_Solution.resize(m_SettingTable[0].size());
	for(int m=0;m<matrix.size();m++){
		for(int i=0;i<matrix[m].size();i++){
			int job=matrix[m][i];
			for(int j=0;j<matrix[m].size();j++){
				if(m_SettingTable[job][j].machine==m){
					m_Solution[m].push_back(m_SettingTable[job][j]);
					break;
				}
			}
		}
	}
}

void Ges::execute(){
	vector<vector<JobPair> > _solution=m_Solution;
	while(m_Iter<m_MaxIter){
		m_Solution=_solution;
		Graph graph(m_Solution,m_SettingTable);
		graph.setLongestPath();
		int L=graph.getMakespan()-1;

		printf("L=%d\n",L);
		Routine(_solution,L);
	}

	printf("result\n");
	for(int i=0;i<m_Solution.size();i++){
		for(int j=0;j<m_Solution[i].size();j++){
			printf("(%d,%d,%d) ",m_Solution[i][j].jobIndex,m_Solution[i][j].machine,m_Solution[i][j].time);
		}
		printf("\n");
	}
	Graph g(m_Solution,m_SettingTable);
	g.setLongestPath();
	printf("makespan=%d\n",g.getMakespan());
}

void Ges::Routine(vector<vector<JobPair> >& solution,int L){
	vector<vector<JobPair> > _solution=solution;
	while(!m_EP.empty())
		m_EP.pop();

	vector<vector<JobPair> > I;
	Ejection(_solution,I,L);

	vector<JobPair> candidate;
	candidate.clear();

	// I listが殻だった場合の例外処理
	try{
		// Iから一つJobPairを選択する
		candidate=selectEP(I);
	}catch(exception& err){
		printf("\nERROR\n\n");
		printf("%s\n",err.what());
		printf("L=%d\n",L);
		printf("--------------------");

		Graph graph(_solution,m_SettingTable);
		graph.setLongestPath();
		graph.print();

		printf("Ejection\n");
		Ejection(_solution,I,L);
		exit(0);
	}

	// 選択されたJobPairを抜出EPに入れる
	for(int i=0;i<candidate.size();i++){
		JobPair jp=candidate[i];
		int machine=jp.machine;

		vector<JobPair>::iterator it=_solution[machine].begin();
		for(;it!=_solution[machine].end();it++){
			if((*it)==jp){
				_solution[machine].erase(it);
				break;
			}
		}
		m_EP.push(jp);
	}

	Perturb(_solution,L);
	
	do{
		m_Iter++;
		Graph g(m_Solution,m_SettingTable);
		g.setLongestPath();
		fprintf(fOut,"%d,%d,%d\n",m_Iter,m_EP.size(),g.getMakespan());
		//cout<<"Iter="<<m_Iter<<endl;

		// GES-1のために解とEPを保持
		vector<vector<JobPair> > beforeSolution=_solution;
		stack<JobPair> beforeEP=m_EP;

		// EPより一つ作業を取得
		JobPair tarJob=m_EP.top();
		m_EP.pop();
		m_Penalty[tarJob.index]++;

		// 挿入可能な場所に挿入する
		vector<vector<vector<JobPair> > > solutionCandidates;
		int index;
		int count=0;
		int min=INT_MAX;
		for(int i=0;i<_solution[tarJob.machine].size()+1;i++){
			vector<vector<JobPair> > __solution=_solution;
			insertJob(__solution,tarJob,i);
			// TODO
			// 挿入した際cycleが生じたらその候補は捨てる
			g=Graph(__solution,m_SettingTable);
			try{
				g.setLongestPath();
			}catch(runtime_error& e){
				continue;
			}
			// feasible scheduleならば
			if(m_EP.empty()){
				LocalSearch(__solution);
			}
			g=Graph(__solution,m_SettingTable);
			try{
				g.setLongestPath();
			}catch(runtime_error& e){
				continue;
			}
			int makespan=g.getMakespan();
			if(makespan<min){
				min=makespan;
				index=count;
			}
			solutionCandidates.push_back(__solution);
			count++;
		}
		// makespanが最小のものを選択する
		// 候補が0ではない場合
		if(!solutionCandidates.empty())
			_solution=solutionCandidates[index];
		else
			m_EP.push(tarJob);
		g=Graph(_solution,m_SettingTable);
		g.setLongestPath();
		if(g.getMakespan()>L){
			vector<vector<JobPair> > I;
			Ejection(_solution,I,L);
			// Iが空だった場合
			if(I.empty()){
				if(m_GESMode==1){
					// GES-1
					_solution=beforeSolution;
					m_EP=beforeEP;
				}else if(m_GESMode==2){
					// GES-2
					excessiveEject(_solution,L);
				}
			}else{
				// Iから一つJobPairを選択する
				candidate.clear();
				candidate=selectEP(I);
				// 選択されたJobPairを抜出EPに入れる
				for(int i=0;i<candidate.size();i++){
					JobPair jp=candidate[i];
					int machine=jp.machine;

					vector<JobPair>::iterator it=_solution[machine].begin();
					for(;it!=_solution[machine].end();it++){
						if((*it)==jp){
							_solution[machine].erase(it);
							break;
						}
					}
					m_EP.push(jp);
				}
			}
		}
		Perturb(_solution,L);
		Graph gr(_solution,m_SettingTable);
		gr.setLongestPath();
	}while(!m_EP.empty() && m_Iter<m_MaxIter);
	solution=_solution;
}

vector<JobPair> Ges::selectEP(vector<vector<JobPair> >& I){
	int index=-1;
	int size=INT_MAX;

	if(I.empty()){
		throw exception();
	}

	for(int i=0;i<I.size();i++){
		int tSize=0;
		for(int j=0;j<I[i].size();j++){
			tSize+=m_Penalty[I[i][j].index];
		}
		if(size>tSize){
			size=tSize;
			index=i;
		}
	}
	return I[index];
}

void Ges::Ejection(vector<vector<JobPair> > _solution,vector<vector<JobPair> >& a_I,int L){
	Graph graph(_solution,m_SettingTable);
	graph.setLongestPath();
	deque<Node*> bottleneckNode;

	// ボトルネックノード抽出
	for(int i=0;i<graph.size();i++){
		if(graph[i]->m_R+graph[i]->m_Q-graph[i]->m_Jobpair->time>L){
			if(graph[i]->m_Jobpair->machine!=-1)
				bottleneckNode.push_back(graph[i]);
		}
	}
	vector<JobPair> candidates;
	Ejection(graph,bottleneckNode,candidates,a_I,0,L);
}

void Ges::Ejection(Graph graph,deque<Node*> bottleneckNode,vector<JobPair> a_candidates,vector<vector<JobPair> >& a_I,int count,int L){
	if(count>=m_kMax || bottleneckNode.empty()){
		return;
	}
	// 辞書順に抜き出しす
	while(!bottleneckNode.empty()){
		Node* node=bottleneckNode.front();
		bottleneckNode.pop_front();
		int index=node->getIndex();

		Graph _graph(graph);
		_graph.removeNode(index);
		a_candidates.push_back(*_graph[index]->m_Jobpair);

		if(_graph.getMakespan()<=L){
			a_I.push_back(a_candidates);
		}
		
		Ejection(_graph,bottleneckNode,a_candidates,a_I,count+1,L);
		a_candidates.pop_back();
	}
}

void Ges::Perturb(vector<vector<JobPair> >& solution,int L){
	// 指定した回数だけランダムに解を遷移させる
	// ただし、遷移してL以下にならなかった場合は
	// 解を遷移させずにカウントする
	int cnt=0;
	while(m_IterRand>cnt){
		vector<vector<JobPair> > _solution=solution;
		int machineRand=Util::getRand(0,_solution.size()-1);
		if(_solution[machineRand].size()<2){
			continue;
		}
		int jobRand=Util::getRand(0,_solution[machineRand].size()-2);
		

		JobPair jp=_solution[machineRand][jobRand];
		_solution[machineRand][jobRand]=_solution[machineRand][jobRand+1];
		_solution[machineRand][jobRand+1]=jp;
		
		// TODO
		// 解を遷移した際、cycleが生じるとその解は捨てる
		Graph g(_solution,m_SettingTable);
		try{
			g.setLongestPath();
			cnt++;
		}catch(runtime_error& e){
			continue;
		}
		if(g.getMakespan()<=L){
			solution=_solution;
		}

	}
}

void Ges::insertJob(vector<vector<JobPair> >& solution,JobPair &jp,int index){
	solution[jp.machine].push_back(jp);

	for(int i=solution[jp.machine].size()-1;i>index;i--){
		JobPair tJob=solution[jp.machine][i];
		solution[jp.machine][i]=solution[jp.machine][i-1];
		solution[jp.machine][i-1]=tJob;
	}

}

void Ges::LocalSearch(vector<vector<JobPair> >& solution){
	deque<vector<vector<JobPair> > > tabuList;
	vector<vector<JobPair> > _solution=solution;
	vector<vector<JobPair> > minSolution=solution;
	int minMakespan;

	Graph g(_solution,m_SettingTable);
	g.setLongestPath();
	int notImprove=0;
	int prevMakespan=g.getMakespan();
	minMakespan=prevMakespan;
	do{
		NeighbourGenerator ng(_solution,m_SettingTable);
		ng.makeNeighbour();
		int size=ng.getNeighbourSize();
		Graph g(_solution,m_SettingTable);
		g.setLongestPath();
		int makespan=g.getMakespan();
		vector<vector<JobPair> > __solution;

		bool flag=false;
		for(int i=0;i<size;i++){
			__solution=ng.getNeighbour(i);
			if(tabuCheck(tabuList,__solution,_solution))
				continue;
			Graph _g(__solution,m_SettingTable);
			_g.setLongestPath();
			int _makespan=_g.getMakespan();
			if(_makespan>makespan)
				continue;
			_solution=__solution;
			addTabuList(tabuList,_solution);
			flag=true;
			break;
		}
		if(!flag){
			int minMakespan=INT_MAX;
			int index=-1;
			for(int i=0;i<size;i++){
				__solution=ng.getNeighbour(i);
				if(tabuCheck(tabuList,__solution,_solution))
					continue;
				Graph _g(__solution,m_SettingTable);
				_g.setLongestPath();
				int _makespan=_g.getMakespan();
				if(minMakespan>_makespan){
					minMakespan=_makespan;
					index=i;
				}
			}
			if(index!=-1){
				_solution=ng.getNeighbour(index);
				addTabuList(tabuList,_solution);
			}
		}
		g=Graph(_solution,m_SettingTable);
		g.setLongestPath();
		int _makespan=g.getMakespan();
		if(minMakespan>_makespan){
			minMakespan=_makespan;
			minSolution=_solution;
		}
		if(prevMakespan==minMakespan){
			notImprove++;
		}else{
			notImprove=0;
		}
		prevMakespan=minMakespan;
		solution=_solution;
	}while(notImprove<m_stagLS);
	solution=minSolution;
}

bool Ges::tabuCheck(deque<vector<vector<JobPair> > >& tabuList,vector<vector<JobPair> >& _solution,vector<vector<JobPair> >&solution){
	int machine=-1;
	for(int i=0;i<_solution.size();i++){
		for(int j=0;j<_solution[i].size();j++){
			if(solution[i][j].index!=_solution[i][j].index){
				machine=i;
				break;
			}
		}
		if(machine!=-1)
			break;
	}
	bool flag;
	for(int i=0;i<tabuList.size();i++){
		flag=true;
		for(int j=0;j<_solution[machine].size();j++){
			if(tabuList[i][machine][j].index!=_solution[machine][j].index)
				flag=false;
		}
		if(flag)
			break;
	}
	return flag;
}

void Ges::addTabuList(deque<vector<vector<JobPair> > >& tabuList,vector<vector<JobPair> >& solution){
	tabuList.push_back(solution);
	if(tabuList.size()>m_maxT){
		tabuList.pop_front();
	}
}

void Ges::excessiveEject(vector<vector<JobPair> > &solution,int L){
	// TODO
	// kmax個のボトルネックノードをp[i]が小さいものを優先して抜き出す
	// この作業をI(pi,L)が空で無くなるまで繰り返す
	// 最後は最小のものをIから選択肢solutionから抜き出す
	// また制約違反なく戻せるものは元の位置に戻す
	// 残ったものをEPに登録する
	vector<vector<JobPair> > _solution=solution;
	Graph graph(_solution,m_SettingTable);
	graph.setLongestPath();
	
	vector<Node*> bottleneckNode;
	vector<JobPair> removedJobpair;

	// ボトルネックノード抽出
	for(int i=0;i<graph.size();i++){
		if(graph[i]->m_R+graph[i]->m_Q-graph[i]->m_Jobpair->time>L){
			if(graph[i]->m_Jobpair->machine!=-1)
				bottleneckNode.push_back(graph[i]);
		}
	}
	
	// ボトルネックノードの並び替え
	bottleneckSort(bottleneckNode);
	
	vector<vector<JobPair> > I;
	do{
		removeSolution(_solution,bottleneckNode,removedJobpair);
		Ejection(_solution,I,L);
	}while(I.empty());
	vector<JobPair> candidate=selectEP(I);
	
	// 選択されたJobPairをsolutionから抜き出す
	for(int i=0;i<candidate.size();i++){
		JobPair jp=candidate[i];
		int machine=jp.machine;

		vector<JobPair>::iterator it=_solution[machine].begin();
		for(;it!=_solution[machine].end();it++){
			if((*it)==jp){
				removedJobpair.push_back((*it));
				_solution[machine].erase(it);
				break;
			}
		}
	}
	
	// 制約違反なく戻せるものは元の位置に戻す
	for(int i=0;i<removedJobpair.size();i++){
		int index=removedJobpair[i].index;
		int machine=removedJobpair[i].machine;
		int tarIndex=-1;
		for(int j=0;j<solution[machine].size();j++){
			if(solution[machine][j].index!=index)
				continue;
			tarIndex=j;
			break;
		}
		for(int j=0;j<_solution[machine].size();j++){
			int srcIndex=_solution[machine][j].index;
			bool isFind=false;
			int insertIndex=-1;
			for(int k=0;k<solution.size();k++){
				if(srcIndex!=solution[machine][k].index)
					continue;
				if(k>tarIndex){
					isFind=true;
					insertIndex=k-1;
					break;
				}
			}
			if(isFind){
				vector<vector<JobPair> > __solution=_solution;
				insertJob(__solution,removedJobpair[i],insertIndex);
				Graph g(__solution,m_SettingTable);
				try{
					g.setLongestPath();
				}catch(runtime_error &e){
					m_EP.push(removedJobpair[i]);
					break;
				}
				if(g.getMakespan()>L){
					m_EP.push(removedJobpair[i]);
					break;
				}
				_solution=__solution;
				break;
			}
		}
	}
	solution=_solution;
}

// bottleneckNodeのソート
void Ges::bottleneckSort(vector<Node*>& bottleneckNode){
	for(int i=0;i<bottleneckNode.size();i++){
		for(int j=i;j<bottleneckNode.size();j++){
			if(m_Penalty[bottleneckNode[i]->m_Jobpair->index]
				<=m_Penalty[bottleneckNode[j]->m_Jobpair->index])
				continue;
			Node* tmp=bottleneckNode[i];
			bottleneckNode[i]=bottleneckNode[j];
			bottleneckNode[j]=tmp;
		}
	}
}

void Ges::removeSolution(vector<vector<JobPair> > &solution,vector<Node*> &bottleneckNode,vector<JobPair> &removedJobpair){
	for(int i=0;i<m_kMax;i++){
		if(bottleneckNode.empty())
			break;
		Node* node=bottleneckNode[0];
		removedJobpair.push_back((*node->m_Jobpair));
		bottleneckNode.erase(bottleneckNode.begin());
		int machine=node->m_Jobpair->machine;
		for(vector<JobPair>::iterator it=solution[machine].begin();it!=solution[machine].end();it++){
			if((*it).index!=node->m_Jobpair->index)
				continue;
			solution[machine].erase(it);
			break;
		}
	}
}

Ges::~Ges(){
	fclose(fOut);
}