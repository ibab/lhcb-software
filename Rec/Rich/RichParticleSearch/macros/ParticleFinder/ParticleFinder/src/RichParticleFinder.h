#ifndef RICHPARTICLEFINDER_H
#define RICHPARTICLEFINDER_H

#include <iostream>
#include <string>
#include <sstream>
#include "ChainTuples.h"
#include "TTree.h"
#include "TChain.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

#include "TEntryList.h"
#include "ReadFile.h"
#include "RichParticle.h"

namespace MPC
{
class RichParticleFinder
{
public:
	RichParticleFinder(const char* cuts, TTree* tree);
	RichParticleFinder(const char* cuts, TChain* ch);
	RichParticleFinder(const char* cuts, ChainTuples* ct);

	
	~RichParticleFinder()
	{
		delete _oldTree;
		delete _tree;
		delete _file0;
	}
	double& operator[] (std::string);
	void Cuts(std::string cutString);
	void SetBranchAddress();
	void SetOutputBranchAddress(TTree* outTree);

	void CreateNtuple(const char* outputFileName, const char* tupleName);

	void SetVariables(std::string fileOfVars);

	void SetHistoLevel(int histoLevel);

	void Histograms();

	void FillHistograms();

	void WriteHistograms(const char* histFileName);

private:
	bool CreateNewTuple();
	std::map<std::string,double> vars;
	std::map<std::string,TH1F*> TH1Hist;
	std::map<std::string,TH2F*> TH2Hist;

	TTree* _oldTree;
	TTree* _tree;
	TFile* _file0;
	const char* _cuts;
	int _histoLevel;

	std::string Rich;
};


}






#endif
