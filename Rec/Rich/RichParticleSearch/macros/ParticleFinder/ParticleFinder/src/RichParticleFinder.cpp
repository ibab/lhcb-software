#include "RichParticleFinder.h"

namespace MPC
{

RichParticleFinder::RichParticleFinder(const char* cuts, TTree* tree)
:_cuts(cuts),_histoLevel(1)
{
	_oldTree = tree;
	_tree = new TTree();
	_file0 = new TFile();

}
RichParticleFinder::RichParticleFinder(const char* cuts, TChain* ch)
:_cuts(cuts),_histoLevel(1)
{
	_oldTree = ch->GetTree();
	_tree = new TTree();
	_file0 = new TFile();

}

RichParticleFinder::RichParticleFinder(const char* cuts, ChainTuples* ct)
: _cuts(cuts),_histoLevel(1)
{
	_oldTree = ct->Tree();
	//	std::cout << "entries " << _oldTree->GetEntries() << std::endl;
	_tree = new TTree();
	_file0 = new TFile();
}


bool RichParticleFinder::CreateNewTuple()
{
	TEntryList* elist=0;
	_oldTree->SetBranchStatus("*",1);

	std::cout << "cuts " << _cuts << std::endl;

	if (_cuts&&strcmp(_cuts,"")!=0) {
		_oldTree->Draw(">>elist",_cuts,"entryList");
		elist = (TEntryList*)gDirectory->Get("elist");
		if (!elist)
		{
			std::stringstream msg;
			msg << "Failed to retrieve TEntryList for _cuts " << _cuts;
		}
		_oldTree->SetEntryList(elist);
	}

	Long64_t nentries = !elist?_oldTree->GetEntries():elist->GetN();
	if (nentries==0) {
		std::cout << "No entries after _cuts " << _cuts << std::endl;
	}

	// Creat new file for tuple with _cuts
	std::string newFilename = "TestFile";
	_file0 = TFile::Open(newFilename.c_str(), "RECREATE");

	if(0 == _file0) return false;
	_file0->cd();
	std::cout << "Applying _cuts: " << _cuts << std::endl;
	_tree = _oldTree->CopyTree(_cuts, "");

	_tree->Write();
	return true;
}

double& RichParticleFinder::operator[] (std::string varName)
{
	return vars[varName];
}

void RichParticleFinder::SetBranchAddress()
{
	std::map<std::string, double>::iterator it;
	for (it = vars.begin(); it != vars.end(); it++)
	{
		std::cout << (*it).first << std::endl;
		_oldTree->SetBranchAddress(((*it).first).c_str(),&vars[(*it).first]);
	}
}

void RichParticleFinder::SetOutputBranchAddress(TTree* outTree)
{

	std::map<std::string, double>::iterator it;
	for (it = vars.begin(); it != vars.end(); it++)
	{
		std::cout << (*it).first << std::endl;
		outTree->Branch(((*it).first).c_str(),&vars[(*it).first],(((*it).first)+"/D").c_str());
	}
}

void RichParticleFinder::SetVariables(std::string fileOfVars)
{
	ReadFile rf(fileOfVars);
	std::vector<std::string> vecOfVars = rf.Lines();
	std::vector<std::string>::iterator it; 
	for (it = vecOfVars.begin(); it != vecOfVars.end(); it++)
	{
		vars[(*it)];
	}
}

void RichParticleFinder::CreateNtuple(const char* outputFileName, const char* tupleName)
{
	CreateNewTuple();
	std::map<std::string, double>::iterator it;

	TTree *outTree = new TTree(tupleName,tupleName);

	SetOutputBranchAddress(outTree);
	int NEntries = _tree->GetEntries();

	Histograms();
	for (int i=0; i<=NEntries; i++)
	{
		_tree->GetEntry(i); // Get Entry
		outTree->Fill();
		FillHistograms();
	}
	TFile* outputFile = new TFile(outputFileName,"RECREATE");

	outTree->Write();
//	WriteHistograms("Test.root");
	delete outputFile;
}

void RichParticleFinder::SetHistoLevel(int histoLevel)
{
	_histoLevel = histoLevel;
}

void RichParticleFinder::Histograms()
{
	if (_histoLevel > 0)
	{
		TH1Hist["MassHisto"] = new TH1F("Mass_Per_Track","Average Mass of Particle Calculated per Track ",16000,0,200000);
		TH1Hist["InverseMassHisto"] = new TH1F("Inverse_Mass_Per_Track","Average Inverse Mass of Particle Calculated per Track ",2000,0.0,0.008);
	}
	if (_histoLevel > 1)
	{
		TH2Hist["MomCK"]		   = new TH2F("Momentum v CK angle "," Momentum V CK angle ",4000,0,200000,1000,0,0.053);
		TH2Hist["MomCK2"]		   = new TH2F("1/Momentum^{2} v CK2 angle "," 1/Momentum2 V CK2 angle ",9000,0,1/(100000),1000,0,0.3*0.3);
	}
}

void RichParticleFinder::FillHistograms()
{
	if (_histoLevel > 0)
	{
		TH1Hist["MassHisto"]->Fill(vars["averageTrackMass"]);
		TH1Hist["InverseMassHisto"]->Fill(1/vars["averageTrackMass"]);
//		std::cout << "HERE " << vars["averageTrackMass"] << std::endl;
	}
	if (_histoLevel > 1)
	{
		TH2Hist["MomCK"]->Fill(vars["momentum"],vars["avThetaRec"]);
		TH2Hist["MomCK2"]->Fill(1/(vars["momentum"]*vars["momentum"]),vars["avThetaRec"]*vars["avThetaRec"]);
	}
}

void RichParticleFinder::WriteHistograms(const char* histFileName)
{
	TFile* histFile = new TFile(histFileName,"RECREATE");
	std::map<std::string,TH1F*>::iterator it1;
	for (it1 = TH1Hist.begin(); it1 != TH1Hist.end(); it1++)
	{
		std::cout << (*it1).first << std::endl;
		((*it1).second)->Write(((*it1).first).c_str());
	}
	std::map<std::string,TH2F*>::iterator it2;
	for (it2 = TH2Hist.begin(); it2 != TH2Hist.end(); it2++)
	{
//		((*it2).second)->Draw();
		((*it2).second)->Write(((*it2).first).c_str());
	}
	delete histFile;
}

}
