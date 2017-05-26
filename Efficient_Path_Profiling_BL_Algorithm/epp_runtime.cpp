#include <iostream>
#include <map>
#include "epp_runtime.h"
std::map<std::pair<int  , int > , int>  count;
std::map<std::pair<int , int> , int>::iterator c;
std::pair<int  , int> b;
std::map<int , int> r;

void init_path_reg(int loopId)
{
	r[loopId]=0;
}

void inc_path_reg(int loopId , int val)
{
	r[loopId]+= val;
}


void finalize_path_reg(int loopId)
{
	b = std::make_pair(loopId, r[loopId]);

	c= count.find(b);
	if (c != count.end())
		count[b]+=1;

	else
		count[b]=1;
}



void  dump_path_regs()
{
	std::cout << "\n";
	for(std::map<std::pair<int  , int> , int>::iterator i= count.begin() , e = count.end() ; i!=e ; ++i)
	{
		std::cerr  << (i->first).first << "       "<< (i->first).second << "           " << i->second <<"\n";
	}
}


