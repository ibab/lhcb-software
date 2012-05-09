#ifndef SCPHISTO_HH
#define SCPHISTO_HH

#include "Mint/ScpBinning.h"
#include "TH1F.h"
#include "TGraph.h"

class ScpHisto
{
private:
	ScpBinning* _scp;
	TGraph* gr;

public:
	ScpHisto(ScpBinning* scp)
	{
		_scp = scp;
	}
	~ScpHisto()
	{
		delete gr;
	}
	unsigned int NBins();
	int Diff_ofbin(unsigned int i);
	void MakeGraph();
	int LargestDiff();
	void SaveGraphPDF(const char* name, const char* Options = "");
	void Write(const char* fileName, const char* name);

};


#endif
