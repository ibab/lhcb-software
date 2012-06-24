#ifndef CHAINTUPLES_H
#define CHAINTUPLES_H

#include <iostream>
#include <vector>
#include <fstream>
#include "TChain.h"
#include "TTree.h"
#include "ReadFile.h"

class ChainTuples
{
public:
	ChainTuples(std::string FileOfTuples,std::string name);
	~ChainTuples()
	{
		delete ch;
	}
	
	TChain* Chain();
	TTree* Tree();
private:
	TChain* ch;
};


#endif
