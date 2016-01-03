#ifndef _FILEREADER_H_
#define _FILEREADER_H_

#include <vector>
#include <map>
#include <string>
#include "JobPair.h"
#include "Gt.h"

using namespace std;

class FileReader{
public:
	vector<vector<JobPair> >& getTable();
	static FileReader* getInstance(const char *);
private:
	vector<vector<JobPair> > mTable;
	FileReader(const char *);
	vector<string> split(const string &str, char delim);
	static map<string,FileReader*> pool;
};

#endif