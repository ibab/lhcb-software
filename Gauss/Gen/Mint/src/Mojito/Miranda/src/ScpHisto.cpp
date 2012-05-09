#include "Mint/ScpHisto.h"
#include "Mint/ScpBinning.h"


unsigned int ScpHisto::NBins()
{
	return _scp->ndof()+1;
}

int ScpHisto::Diff_ofbin(unsigned int i)
{
	return _scp->Diff_ofBin(i);
}

int ScpHisto::LargestDiff()
{
	int largestDiff = 0;
	for (unsigned int i = 0; i < NBins(); ++i)
	{
		if (fabs(_scp->Diff_ofBin(i)) > largestDiff) largestDiff = fabs(_scp->Diff_ofBin(i));
	}
	return largestDiff;
}

void ScpHisto::MakeGraph()
{

	unsigned int nbins = NBins();
	Int_t x[nbins+1], y[nbins+1];

	for (unsigned int i = 0; i < nbins; ++i)
	{
		x[i] = i;
		y[i] = Diff_ofbin(i);
	}
	gr = new TGraph(nbins,x,y);
//	gr->Draw("");
}

void ScpHisto::SaveGraphPDF(const char* name, const char* Options)
{
	 TCanvas *c1 = new TCanvas("c1","A Simple Graph Example",200,10,700,500);
	 gr->Draw(Options);
	 c1->SaveAs(name);
	 delete c1;
}

void ScpHisto::Write(const char* fileName, const char* name)
{
	TFile* file = new TFile(fileName,"READ");
	gr->Write(name);
	delete file;
}
