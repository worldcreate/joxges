#include "MemoryManagement.h"
#include <cstdio>

MemoryManagement* MemoryManagement::s_instance=NULL;

MemoryManagement::MemoryManagement(){
}

MemoryManagement* MemoryManagement::getInstance(){
	if(s_instance==NULL)
		s_instance=new MemoryManagement();
	return s_instance;
}

void MemoryManagement::releaseInstance(){
	if(s_instance!=NULL)
		delete s_instance;
}

void MemoryManagement::addAddress(void* p,string str){
	m_MemoryAddress.insert(map<void*,string>::value_type(p,str));
}

void MemoryManagement::removeAddress(void* p){
	if(m_MemoryAddress.erase(p)==0){
		printf("no matching!\n");
	}
}

int MemoryManagement::size(){
	return m_MemoryAddress.size();
}

void MemoryManagement::print(){
	printf("=========================================\n");
	map<void*,string>::iterator it=m_MemoryAddress.begin();
	for(;it!=m_MemoryAddress.end();it++){
		printf("%p:%s\n",(*it).first,(*it).second.c_str());
	}
	printf("=========================================\n");
}

MemoryManagement::~MemoryManagement(){
}