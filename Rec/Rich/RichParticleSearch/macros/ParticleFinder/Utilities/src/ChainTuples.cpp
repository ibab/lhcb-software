#include "ChainTuples.h"


ChainTuples::ChainTuples(std::string fileOfNTuples, std::string name)
{
	//return c1;
	ReadFile rf(fileOfNTuples);
	std::vector<std::string> VectorOfFiles = rf.Lines();
	
	
	ch = new TChain (name.c_str());
	//TChain ch("RichParticleSearch"+RICH+"TrackTuple");

	std::vector<std::string>::iterator it2;
	for ( it2 =VectorOfFiles.begin(); it2 < VectorOfFiles.end(); it2++){
		const char* f = (*it2).c_str();
		ch->Add(f);
		std::cout << "Adding file " << f << std::endl;
	}
//	int Entries = ch->GetEntries();
//	std::cout << Entries << std::endl;
}

TChain* ChainTuples::Chain()
{
	return ch;
}

TTree* ChainTuples::Tree()
{
//	TTree* tree = ch->GetTree();
//	std::cout << tree->GetEntries() << std::endl;
	return ch;
}
