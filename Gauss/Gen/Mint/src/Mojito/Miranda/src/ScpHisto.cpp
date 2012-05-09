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
		ey[i] = (double)Diff_ofbin(i)*Err_ofbin(i);
	}
	gr = new TGraphErrors(nbins,x,y,ex,ey);
	MakePull();
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
		ey[i] = (double)Diff_ofbin(i)*Err_ofbin(i);
	}
	gr = new TGraphErrors(nbins,x,y,ex,ey);
	MakeNormPull();
}

void ScpHisto::SaveAs(const char* name, const char* Options)
{
	TCanvas * c1 = new TCanvas("c1","SCP Plots");
	TPad*    upperPad = new TPad("upperPad", "upperPad",   .005, .2525, .995, .995);
	TPad*    lowerPad = new TPad("lowerPad", "lowerPad",   .005, .005,  .995, .2475);
	lowerPad->Draw();
	upperPad->Draw();
	upperPad->cd();
//	gr->SetMarkerStyle(15);
	gr->Draw(Options);
	lowerPad->cd();
	pulls->SetTitle("");
	pulls->GetXaxis()->SetLabelSize(0);
	pulls->GetXaxis()->SetTitle("");
	pulls->GetYaxis()->SetTitle("");
	pulls->GetYaxis()->SetLabelSize(0.1);
	pulls->GetYaxis()->SetRangeUser(-5,5);
//	pulls->SetMarkerStyle(15);
//	pulls->Draw("B");
	pulls->Draw(Options);
	c1->SaveAs(name);
	delete upperPad;
	delete lowerPad;
	delete c1;
}

void ScpHisto::Write(const char* fileName, const char* name)
{
	TFile* file = new TFile(fileName,"READ");
	gr->Write(name);
	delete file;
}

void ScpHisto::MakeNormPull()
{
	unsigned int bins = NBins();
	Double_t y[bins+1];
	Double_t x[bins+1];
	for (unsigned int i = 0; i < bins; ++i)
	{
		x[i] = (double)i;
		y[i] =  NormDiff_ofbin(i)/Err_ofbin(i);
	}
	pulls = new TGraph(bins,x,y);
//	return pull;
}

void ScpHisto::MakePull()
{
	unsigned int bins = NBins();
	Double_t y[bins+1];
	Double_t x[bins+1];
	for (unsigned int i = 0; i < bins; ++i)
	{
		x[i] = (Double_t)i;
		y[i] =  (Double_t)(Diff_ofbin(i))/Err_ofbin(i);
	}
	pulls = new TGraph(bins,x,y);
//	return pull;
}


