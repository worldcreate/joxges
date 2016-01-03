#ifndef _FILEREADER_H_
#define _FILEREADER_H_

#include <vector>
#include <string>
#include "JobPair.h"

using namespace std;

class FileReader{
public:
	FileReader(const char *);
	vector<vector<JobPair> >& getTable();
private:
	vector<vector<JobPair> > mTable;
	vector<string> split(const string &str, char delim);
};

#endif