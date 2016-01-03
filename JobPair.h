#ifndef _JOBPAIR_H_
#define _JOBPAIR_H_

#include <iostream>
#include <cstdio>

using namespace std;

class JobPair{
public:
	JobPair(){
		machine=-1;
		time=-1;	// 加工時刻
		endTime=-1;	// 加工終了時刻
		jobIndex=-1;
		checked=false;
		prev=-1;
		prevIndex=-1;
		next=-1;
		nextIndex=-1;
		technicalOrder=-1;
		index=-1;	// uniqueの番号
	}
	JobPair& operator=(const JobPair& dst){
		machine=dst.machine;
		time=dst.time;
		endTime=dst.endTime;
		jobIndex=dst.jobIndex;
		checked=dst.checked;
		prev=dst.prev;
		prevIndex=dst.prevIndex;
		next=dst.next;
		nextIndex=dst.nextIndex;
		technicalOrder=dst.technicalOrder;
		index=dst.index;
		return *this;
	}
	bool operator==(const JobPair& dst){
		return index==dst.index;
	}
	void print(){
		printf("(%d,%d) ",machine,jobIndex);
	}
	int machine;
	int prev;
	int prevIndex;
	int next;
	int nextIndex;
	int time;
	int endTime;
	int jobIndex;
	int technicalOrder;
	int index;
	bool isCheck(){return checked;}
	void check(){checked=true;}
private:
	bool checked;
};

#endif