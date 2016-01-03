#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include "FileReader.h"



using namespace std;

FileReader::FileReader(const char *file){
	ifstream ifs(file);
	string str;
	int j=0;
	int cnt=0;

	while(getline(ifs,str)){
		vector<JobPair> row;
		vector<string> array=split(str,':');

		for(int i=0;i<array.size();i++){
			vector<string> pair=split(array[i],',');
			const char *machine=pair[0].c_str();
			const char *time=pair[1].c_str();
			JobPair jp;
			jp.machine=atoi(machine);
			jp.time=atoi(time);
			jp.endTime=-1;
			jp.jobIndex=j;
			jp.technicalOrder=i;
			jp.index=cnt++;
			if(i!=0){
				jp.prev=row[i-1].machine;
				jp.prevIndex=row[i-1].index;
				row[i-1].next=jp.machine;
				row[i-1].nextIndex=jp.index;
			}
			row.push_back(jp);
		}
		mTable.push_back(row);
		j++;
	}
}

vector<vector<JobPair> >& FileReader::getTable(){
	return mTable;
}

vector<string> FileReader::split(const string &str, char delim){
	istringstream iss(str); string tmp; vector<string> res;
	while(getline(iss, tmp, delim)) res.push_back(tmp);
	return res;
}