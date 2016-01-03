#include "Graph.h"
#include <string>

Graph::Graph():output(false){
}

/* solution
	縦がmachine
	横が投入順序
*/
/* settingTable
	縦がjob
	横が技術的順序
 */
Graph::Graph(const vector<vector<JobPair> >& solution,const vector<vector<JobPair> >& settingTable,bool oflag){
	output=oflag;
	if(output){
		printf("Graph constructor\n");
	}
	Node* root=new Node(output);
	Node* leaf=new Node(output);
	Node* now=root;
	array.push_back(root);
	/*
		settingTableを頼りに技術的順序による
		グラフを作成する
	 */
	int index=0;
	root->setIndex(index);
	for(int i=0;i<settingTable.size();i++){
		now=root;
		for(int j=0;j<settingTable[i].size();j++){
			int machine=settingTable[i][j].machine;
			int insertIndex=settingTable[i][j].index;
			bool isExist=false;
			for(int k=0;k<solution[machine].size();k++){
				if(solution[machine][k].index!=insertIndex)
					continue;
				isExist=true;
				break;
			}
			if(!isExist)
				continue;
			Node* node=new Node(&settingTable[i][j],output);
			node->setIndex(++index);
			array.push_back(node);
			now->addNode(node);
			
			now=node;
		}
		now->addNode(leaf);
	}
	array.push_back(leaf);
	leaf->setIndex(++index);
	/*
		機械の投入順序をsolutionから調べ
		設定していく
	 */
	for(int i=1;i<array.size()-1;i++){
		int index=array[i]->m_Jobpair->index;
		int nextIndex=-1;
		for(int j=0;j<solution.size();j++){
			/* 最終列を検索する必要はない
				なぜなら最終列とヒットした場合、その後ろの機械は
				ないからである
			*/
			for(int k=0;k<solution[j].size()-1;k++){
				if(solution[j][k].index==index){
					nextIndex=solution[j][k+1].index;
					break;
				}
			}
			if(nextIndex!=-1)
				break;
		}
		if(nextIndex==-1)
			continue;
		for(int j=0;j<array.size();j++){
			if(array[j]->m_Jobpair->index==nextIndex){
				array[i]->addNode(array[j]);
				break;
			}
		}
	}
}

Graph::Graph(const Graph& graph):output(false){
	operator=(graph);
}

int Graph::size() const{
	return array.size();
}

Node* Graph::operator[](int n) const{
	return array[n];
}

Graph& Graph::operator=(const Graph& graph){
	for(int i=0;i<array.size();i++){
		delete array[i];
	}
	output=graph.output;
	array.resize(graph.size());
	for(int i=0;i<array.size();i++){
		Node *node=new Node(*graph[i]);
		array[i]=node;
	}
	for(int i=0;i<array.size();i++){
		for(int j=0;j<graph[i]->m_Prev.size();j++){
			int idx=graph[i]->m_Prev[j]->getIndex();
			for(int k=0;k<array.size();k++){
				if(idx==array[k]->getIndex()){
					array[i]->m_Prev.push_back(array[k]);
					break;
				}
			}
		}
	}
	for(int i=0;i<array.size();i++){
		for(int j=0;j<graph[i]->m_Next.size();j++){
			int idx=graph[i]->m_Next[j]->getIndex();
			for(int k=0;k<array.size();k++){
				if(idx==array[k]->getIndex()){
					array[i]->m_Next.push_back(array[k]);
					break;
				}
			}
		}
	}
}

Node* Graph::getNodeByIndex(int index){
	for(int i=0;i<array.size();i++){
		if(array[i]->m_Jobpair->index==index){
			return array[i];
		}
	}
	return NULL;
}

void Graph::setLongestPath(){
	topologicalSort();
	for(int i=0;i<array.size();i++){
		int max=0;
		for(int j=0;j<array[i]->m_Prev.size();j++){
			if(max<array[i]->m_Prev[j]->m_R){
				max=array[i]->m_Prev[j]->m_R;
			}
		}
		array[i]->m_R=max+array[i]->m_Jobpair->time;
	}
	for(int i=array.size()-1;i>=0;i--){
		int max=0;
		for(int j=0;j<array[i]->m_Next.size();j++){
			if(max<array[i]->m_Next[j]->m_Q){
				max=array[i]->m_Next[j]->m_Q;
			}
		}
		array[i]->m_Q=max+array[i]->m_Jobpair->time;
	}
}

void Graph::topologicalSort() throw(runtime_error){
	stack<Node*> sort;
	for(int i=0;i<array.size();i++){
		if(!array[i]->isCheck() && !array[i]->istempCheck())
			if(!visit(array[i],sort)){
				throw runtime_error("ERROR! cycle graph");
			}
	}
	array.clear();
	while(!sort.empty()){
		array.push_back(sort.top());
		array[array.size()-1]->setIndex(array.size()-1);
		sort.pop();
	}
}

bool Graph::visit(Node* node,stack<Node*>& sort){
	if(node->istempCheck() && !node->isCheck()){
		return false;
	}
	if(node->isCheck())
		return true;
	node->checkTemp();
	for(int i=0;i<node->m_Next.size();i++){
		if(!visit(node->m_Next[i],sort))
			return false;
	}
	node->check();
	sort.push(node);
	return true;
}

int Graph::getMakespan(){
	return array[0]->m_Q;
}

void Graph::removeNode(int index){
	Node* tar=array[index];
	Node* prev=tar->m_Prev[0];
	Node* next=tar->m_Next[0];

	/* 技術的順序のつなぎ変え */
	for(int i=0;i<prev->m_Next.size();i++){
		if(prev->m_Next[i]==tar){
			prev->m_Next[i]=next;
			break;
		}
	}
	for(int i=0;i<next->m_Prev.size();i++){
		if(next->m_Prev[i]==tar){
			next->m_Prev[i]=prev;
			break;
		}
	}

	
	// 投入順序のつなぎ変え
	if(tar->m_Prev.size()==1){
		if(tar->m_Next.size()==2){
			// 投入順序の最初のノードの場合
			tar->m_Next[1]->m_Prev.pop_back();
		}
	}else if(tar->m_Next.size()==1){
		if(tar->m_Prev.size()==2){
			// 投入順序の最後のノードの場合
			tar->m_Prev[1]->m_Next.pop_back();
		}
	}else{
		prev=tar->m_Prev[1];
		next=tar->m_Next[1];
		prev->m_Next[1]=next;
		next->m_Prev[1]=prev;
	}

	// Rの更新
	for(int i=index+1;i<array.size();i++){
		int max=0;
		for(int j=0;j<array[i]->m_Prev.size();j++){
			if(max<array[i]->m_Prev[j]->m_R){
				max=array[i]->m_Prev[j]->m_R;
			}
		}
		array[i]->m_R=max+array[i]->m_Jobpair->time;
	}

	// Qの更新
	for(int i=index-1;i>=0;i--){
		int max=0;
		for(int j=0;j<array[i]->m_Next.size();j++){
			if(max<array[i]->m_Next[j]->m_Q){
				max=array[i]->m_Next[j]->m_Q;
			}
		}
		array[i]->m_Q=max+array[i]->m_Jobpair->time;
	}
}

void Graph::print(){
	printf("- R,Q,J,M,N,P\n");
	for(int i=0;i<array.size();i++){
		printf("%d %d,%d,%d,%d,",array[i]->getIndex(),array[i]->m_R,array[i]->m_Q,array[i]->m_Jobpair->jobIndex,array[i]->m_Jobpair->machine);
		printf("N(");
		for(int j=0;j<array[i]->m_Next.size();j++){
			printf("%d,",array[i]->m_Next[j]->getIndex());
		}
		printf("),P(");
		for(int j=0;j<array[i]->m_Prev.size();j++){
			printf("%d,",array[i]->m_Prev[j]->getIndex());
		}
		printf(")\n");
	}
}

void Graph::printForTsort(){
	for(int i=0;i<array.size();i++){
		for(int j=0;j<array[i]->m_Next.size();j++){
			printf("%d %d\n",array[i]->getIndex(),array[i]->m_Next[j]->getIndex());
		}
	}
}

Graph::~Graph(){
	if(output){
		printf("Graph destructor\n");
	}
	for(int i=0;i<array.size();i++){
		delete array[i];
	}
}