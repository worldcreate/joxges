#ifndef _UTIL_H_
#define _UTIL_H_

#include <vector>
#include <iostream>

using namespace std;

class Util{
public:
	static int getRand(int,int);
	static void setSeed(int );
	template <typename T>
	static bool includeVector(const vector<T>& vec,T value){
		for(int i=0;i<vec.size();i++){
			if(vec[i]==value){
				return true;
			}
		}
		return false;
	}
	template <typename T>
	static void removeVector(vector<T>& vec,int index){
		typename vector<T>::iterator it=vec.begin();
		for(int i=0;it!=vec.end();i++,it++){
			if(i!=index)
				continue;
			vec.erase(it);
			break;
		}
	}
};

#endif