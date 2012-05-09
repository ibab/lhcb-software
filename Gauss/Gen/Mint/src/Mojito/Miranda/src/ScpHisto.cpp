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

double ScpHisto::NormDiff_ofbin(unsigned int i)
{
	return _scp->NormDiff_ofBin(i);
}

double ScpHisto::Err_ofbin(unsigned int i)
{
	return _scp->Err_ofBin(i);
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
	Double_t x[nbins+1], y[nbins+1];
	Double_t ex[nbins+1];
	Double_t ey[nbins+1];

	for (unsigned int i = 0; i < nbins; ++i)
	{
		x[i] = (double)i;
		y[i] = (double)Diff_ofbin(i);
		ex[i] = 0;
		ey[i] = Err_ofbin(i);
	}
	gr = new TGraphErrors(nbins,x,y,ex,ey);
}

void ScpHisto::MakeNormGraph()
{

	unsigned int nbins = NBins();
	Double_t x[nbins+1];
	Double_t y[nbins+1];
	Double_t ex[nbins+1];
	Double_t ey[nbins+1];

	for (unsigned int i = 0; i < nbins; ++i)
	{
		x[i] = (double)i;
		y[i] = NormDiff_ofbin(i);
		ex[i] = 0;
		ey[i] = Err_ofbin(i);
	}
	gr = new TGraphErrors(nbins,x,y,ex,ey);
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
