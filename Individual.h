#ifndef _INDIVIDUAL_H_
#define _INDIVIDUAL_H_

#include <vector>

using namespace std;

class Individual{
public:
	Individual();
	Individual(int,int);
	void initGene(const char*);
	void fixGene(const char*);
	void print();
	void print(vector<vector<int> >&);
	int getJobNum();
	int getMachineNum();
	int getFitness();
	vector<int>& operator[](int);
	bool operator<(Individual& );
	bool operator>(Individual& );
	bool operator==(Individual&);
	bool operator!=(Individual&);
	static bool less(Individual*,Individual*);
	~Individual();
private:
	void check();
	vector<vector<int> > mGene;
	int mFitness;
};

#endif