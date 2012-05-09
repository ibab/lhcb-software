#ifndef SCPHISTO_HH
#define SCPHISTO_HH

#include "Mint/ScpBinning.h"
#include "TH1F.h"
#include "TGraph.h"
#include "TGraphErrors.h"

class ScpHisto
{
private:
	ScpBinning* _scp;
	TGraphErrors* gr;
	TGraph* pulls;

public:
	ScpHisto(ScpBinning* scp)
	{
		_scp = scp;
	}
	~ScpHisto()
	{
		delete gr;
		delete pulls;
	}
	unsigned int NBins();
	int Diff_ofbin(unsigned int i);
	void MakeGraph();
	int LargestDiff();
	void SaveGraphPDF(const char* name, const char* Options = "AP");
	void Write(const char* fileName, const char* name);
	void MakeNormGraph();
	double NormDiff_ofbin(unsigned int i);
	double Err_ofbin(unsigned int i);
	void MakePull();
	void MakeNormPull();



};


#endif
