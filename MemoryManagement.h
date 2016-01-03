#ifndef _MEMORYMANAGEMENT_H_
#define _MEMORYMANAGEMENT_H_

#include <list>
#include <map>
#include <string>

using namespace std;

class MemoryManagement{
public:
	static MemoryManagement* getInstance();
	static void releaseInstance();
	void addAddress(void*,string);
	void removeAddress(void*);
	int size();
	void print();
private:
	MemoryManagement();
	~MemoryManagement();
	static MemoryManagement* s_instance;
	map<void*,string> m_MemoryAddress;
};

#endif