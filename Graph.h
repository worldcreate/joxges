#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <vector>
#include <stack>
#include <stdexcept>
#include "Node.h"
#include "JobPair.h"

using namespace std;

class Graph{
public:
	Graph();
	Graph(const vector<vector<JobPair> >&,const vector<vector<JobPair> > &,bool oflag=false);
	Graph(const Graph&);
	int size() const;
	Node* operator[](int n) const;
	Graph& operator=(const Graph&);
	void print();
	void printForTsort();
	~Graph();
	int getMakespan();
	void removeNode(int);
	Node* getNodeByIndex(int);
	void setLongestPath();
	bool output;
private:
	void topologicalSort()throw(runtime_error);
	bool visit(Node*,stack<Node*>&);
	vector<Node*> array;
};

#endif