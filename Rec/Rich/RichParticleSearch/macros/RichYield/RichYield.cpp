/*
 * RichYield.cpp
 *
 *  Created on: Feb 24, 2011
 *      Author: mcoombes
 */

#include "RichYield.h"
using namespace RooFit;

//#include <boost/lexical_cast.hpp>

#include <string>
#include <vector>
#include <sstream>
#include "TPad.h"
#include "TString.h"
using namespace std;

RichYield::RichYield()
{
	// TODO Auto-generated constructor stub
}

RichYield::~RichYield() {
	//delete *can1;
	//delete *ReducedDataSet;
	// TODO Auto-generated destructor stub
}
string NumToString(int i)
{
	stringstream ss;//create a stringstream
	ss << i;//add number to the stream
	string NewStr = ss.str();//return a string with the contents of the stream
	return NewStr;
}

string NumToString(double i)
{
	stringstream ss;//create a stringstream
	ss << i;//add number to the stream
	string NewStr = ss.str();//return a string with the contents of the stream
	return NewStr;
}

vector<TString> CutsOnBeta()
{
	 vector<double> BetaCutsNum = NumCutsOnBeta();
	 vector<double>::iterator BetaCutsNumIt;
	 std::vector<TString> BetaCuts;
	 int counter = 0;
	 for (BetaCutsNumIt=BetaCutsNum.begin();BetaCutsNumIt<BetaCutsNum.end(); BetaCutsNumIt++)
	 {
		 counter++;
		 TString BetaLow2;
		 TString BetaHigh2;
		 if (BetaCutsNumIt==BetaCutsNum.begin())
		 {
			 TString BetaLow = NumToString(0.0);
			 BetaLow2 = BetaLow;
		 }
		 else
		 {
			 TString BetaLow = NumToString(*(BetaCutsNumIt-1));
			 BetaLow2 = BetaLow;
		 }
		 if (BetaCutsNumIt==BetaCutsNum.end())
		 {
			 TString BetaHigh = NumToString(1.0);
			 BetaHigh2 = BetaHigh;
		 }
		 else
		 {
			 TString BetaHigh = NumToString(*BetaCutsNumIt);
			 BetaHigh2 = BetaHigh;
		 }
//		 TString BetaHigh = NumToString(*BetaCutsNumIt);
		 TString BetaCut0 = "(beta) > "+BetaLow2+" && (beta) <= "+BetaHigh2;

		 BetaCuts.push_back(BetaCut0);
	 }

	 TString BetaCutALL = "(beta) > 0 && (beta) <=  99995";


	 BetaCuts.push_back(BetaCutALL);

	 return BetaCuts;
}
vector<double> NumCutsOnBeta()
{
	// Define cuts on beta for plotting
	 double BetaCut0 = 0.9995;
	 double BetaCut1 = 0.9996; // beta cut to look at how plots vary with beta
	 double BetaCut2 = 0.9997;
	 double BetaCut3 = 0.9998;
	 double BetaCut4 = 0.99985;
	 double BetaCut5 = 0.9999;
	 double BetaCut6 = 0.99995;
	 double BetaCutALL = 99995;

	 std::vector<double> BetaCuts;

	 BetaCuts.push_back(BetaCut0);
	 BetaCuts.push_back(BetaCut1);
	 BetaCuts.push_back(BetaCut2);
	 BetaCuts.push_back(BetaCut3);
	 BetaCuts.push_back(BetaCut4);
	 BetaCuts.push_back(BetaCut5);
	 BetaCuts.push_back(BetaCut6);
//	 BetaCuts.push_back(BetaCutALL);

	 return BetaCuts;
}

void RichYield::FitBeta(std::vector<double> FittedMean1)
{
	TH2F* BetaVNphotons		   = new TH2F("beta v photons"," beta v photons ",100,0.9994,1.0,100,10,35);
	std::vector<double> Beta = NumCutsOnBeta();
	int size = Beta.size();
	std::vector<double>::iterator it;

	//cout << FittedMean[0] << endl;
	for ( int i = 0; i < size; i++ )
	{
	//	BetaVNphotons->Fill(Beta[i],FitMean[i]);
		BetaVNphotons->Fill(Beta[i],FittedMean1[i]);
	}

	BetaVNphotons->Draw();
	BetaVNphotons->Write("betaVPhotons");

}



void RichYield::PlotNPhotonsRich1()
{

	TString RICH = "R1Gas";

	int start_range = 2;
	int end_range = 40;

	//Open ROOT file for writting histograms to
	TCanvas *can1 = new TCanvas("Can1","NPhotons",200,10,700,500);
	// Set up TPad
	TPad *pad1 = new TPad("pad1","",0,0,1,1);
	pad1->SetFillStyle(4000); //will be transparent
	pad1->SetFrameFillStyle(4000);
	pad1->Draw();
	pad1->cd();
	//TCanvas can1("Can1","NPhotons",200,10,700,500);
	/*
	 * Set Styles
	 */
	gStyle->SetOptStat(10);
	gStyle->SetOptFit(1111);

	// Get required data
	TFile * inputFile = TFile::Open("/Users/mcoombes/Documents/ComputerWork/HeavyParticleSearch/ParticleFinderTuple/Output/ParticleSearchTrack-R1Gas-MC10-sigYield.root");
  //  TH1* inputHisto = (TH1*) inputFile->Get("PhotonsPerTrackBeta1");
	TTree* inputTree = (TTree*)inputFile->Get("RichParticleSearchR1GasTrackTuple");
	//Define the observables and ranges over which the PDFs will be made
	RooRealVar NPhotons("TruePhotonsPerTrack","TruePhotonsPerTrack",start_range,end_range);
	RooRealVar beta("beta","beta",0.0,1.001);

	 //And create a RooDataSet object containing these observables:
	 //	For > 1 observable
   // RooDataSet* data = new RooDataSet("data","data",inputTree,RooArgList(NPhotons,beta));
	RooDataSet data("data","data",inputTree,RooArgList(NPhotons,beta));
	 //	For  1 observable
  //  RooDataHist* ReducedDataSet = new RooDataHist("data","data",NPhotons,inputHisto);
	//RooDataHist ReducedDataSet("data","data",NPhotons,inputHisto);
	 // =========================================================================================================================
	 // 													Cuts on Beta
	 // =========================================================================================================================

     std::vector<TString> BetaCuts = CutsOnBeta();

     vector<TString>::iterator it;

     int BetaCutCounter = 0;

	// TFile* outFile = new TFile("../output/Yield-"+RICH+".root", "RECREATE");

	 TFile outFile("../output/Yield-"+RICH+".root", "RECREATE");

	 //signal
	 RooRealVar sig_photons_mean("NPhotons","mean number of photons",28,10,32);
	// RooRealVar sig_photons_sigma("#sigma","width of gaussian",6,0,20);
	 //RooRealVar sig_photons_sigma("#sigma","width of gaussian",6.26);
	 RooRealVar sig_photons_sigma("#sigma","width of gaussian",10,0,26);

//	 RooRealVar sig_photons_tail("#tail","tail of gaussian",0,-20,20);
	 RooRealVar sig_photons_alpha("#alpha","alpha of gaussian",-2,-20,20);
	 RooAbsPdf* signal_NPhotons = new RooGenericPdf("Generic","Generic PDF","exp(-1*(((@0)-(@1))^(2))/(2*(@2)^(2)))*TMath::Freq(@3*(@0-@1)/@2)",
				 RooArgList(NPhotons,sig_photons_mean,sig_photons_sigma,sig_photons_alpha));


	 //RooAbsPdf* signal_NPhotons = new RooNovosibirsk("signal_photons","signal_photons",NPhotons,sig_photons_mean,sig_photons_sigma,sig_photons_tail);
//	 RooAbsPdf* signal_NPhotons = new RooGaussian("signal_photons","signal_photons",NPhotons,sig_photons_mean,sig_photons_sigma);

//	 RooAbsPdf* signal_NPhotons = new RooGenericPdf("Generic","Generic PDF","exp(-1*(((@1)-(@2))^(2))/(2*(@3)^(2)))*TMath::Freq((@4)*((@1)-(@2))/(@3))",
//			 RooArgList(NPhotons,sig_photons_mean,sig_photons_sigma,sig_photons_alpha));
//			 	 RooArgList(NPhotons,sig_photons_mean,sig_photons_tail));

//	 RooAbsPdf* signal_NPhotons = new RooCBShape("signal_photons","signal_photons",NPhotons,sig_photons_mean,sig_photons_sigma,sig_photons_tail,sig_photons_alpha);


	 // Background
  //  RooAbsPdf* bkg_photons = new RooPolynomial("Polynomial","O(1) Polynomial",NPhotons,RooArgList(bkg_photons_p0,bkg_photons_p1));
	 RooRealVar bkg_photons_a("a","a of exponential",-0.56);
	 RooAbsPdf* bkg_photons = new RooExponential("Exponential","Exponential",NPhotons,bkg_photons_a);
     // 	RooAbsPdf* bkg_photons = new RooPolynomial("Polynomial","O(1) Polynomial",NPhotons,RooArgList(bkg_photons_p0,bkg_photons_p1));

	 // Aerogel photons in RICH1
	 RooRealVar aero_photons_mean("AeroNPhotons","mean number of photons Aero",5,2,10);
	// RooRealVar aero_photons_sigma("#sigmaAero","width of gaussian Aero",4,0,7);
	 RooRealVar aero_photons_sigma("#sigmaAero","width of gaussian Aero",5.9);
	 RooRealVar aero_photons_alpha("aero#alpha","Aero alpha of gaussian",-2,-20,20);


	 // Define the signal Fit function
	 RooAbsPdf* aero_NPhotons = new RooGaussian("aero_photons","aero_photons",NPhotons,aero_photons_mean,aero_photons_sigma);
//	 RooAbsPdf* aero_NPhotons = new RooGenericPdf("Generic","Generic PDF","exp(-1*(((@0)-(@1))^(2))/(2*(@2)^(2)))*TMath::Freq(@3*(@0-@1)/@2)",
//					 RooArgList(NPhotons,aero_photons_mean,aero_photons_sigma,aero_photons_alpha));

//	 std::vector<std::vector<double> > FittedMeanPlusBeta;
//	 std::vector<std::vector<double>> FittedMeanPlusBetaIterator;
	 std::vector<double> FittedMean1;
//	 FittedMean1.push_back(2.0);


     for (it = BetaCuts.begin(); it < BetaCuts.end(); it++)
     {

    	cout << *it << endl;
	//	TCut BetaCut;
		TString BetaCut= *it;
    	BetaCutCounter++;
		RooDataSet* ReducedDataSet =  (RooDataSet*)data.reduce(BetaCut);
    	//RooDataSet ReducedDataSet = (RooDataSet)data.reduce(BetaCut);

		 // Draw data
		RooPlot* NphotonsPlot = NPhotons.frame();
		ReducedDataSet->plotOn(NphotonsPlot, Binning ((end_range-start_range)));
		NphotonsPlot->Draw();

		// =========================================================================================================================
		// Now Fitting
		// =========================================================================================================================

//		//Yields

		int NEntries = ReducedDataSet->numEntries();

		RooRealVar Nsig("Nsig","Nsig",0.0,NEntries);

		RooRealVar Nbkg("Nbkg","Nbkg",0.0,NEntries);

		RooRealVar Naero("Naero","Naero",0.0,NEntries);

		// Combine to make PDF
		RooAbsPdf *photon_pdf = new RooAddPdf("photon_pdf","photon_pdf",RooArgList(*signal_NPhotons,*bkg_photons,*aero_NPhotons),RooArgList(Nsig,Nbkg,Naero));
		//RooAbsPdf *photon_pdf = new RooAddPdf("photon_pdf","photon_pdf",RooArgList(*signal_NPhotons,*bkg_photons),RooArgList(Nsig,Nbkg));
		// Now Fit the PDF to the data
		RooFitResult *photon_result = photon_pdf->fitTo(*ReducedDataSet,Extended(),Save());

		//print result
		//photon_result->Print();

		/*
		 * Update the plot to show fit results
		 */
		photon_pdf->plotOn(NphotonsPlot);
		// Add parameters to Fit
		RooArgSet PlotParametersToPrint("PlotParametersToPrint");
		PlotParametersToPrint.add(sig_photons_mean);
		PlotParametersToPrint.add(sig_photons_sigma);
		double mean = sig_photons_mean.getVal() + sig_photons_sigma.getVal()*(sig_photons_alpha.getVal()/sqrt(1+sig_photons_alpha.getVal()*sig_photons_alpha.getVal()))*sqrt(2/3.142);

		FittedMean1.push_back(mean);


		//error on mean
		double errMean = sqrt(pow(sig_photons_mean.getError(),2.0)+pow(sig_photons_sigma.getError(),2.0));
		TString errString = NumToString( errMean );


		cout <<"mean: "<<  mean << endl;
		TString Ystr = NumToString( mean );
		Ystr = "NPhotons "+Ystr+ " #pm"+ errString;
		t = new TLatex ( .62, .89, Ystr );
		t->SetNDC(kTRUE);
		t->SetTextSize(0.045);
		t->SetTextFont(62);

	//	FitMean.push_back(sig_photons_mean);
		//The signal component only in dashed blue:
		photon_pdf->plotOn(NphotonsPlot,Components(*signal_NPhotons),LineColor(kBlue),LineStyle(kDashed));
		//	The background component only in dashed red:
		photon_pdf->plotOn(NphotonsPlot,Components(*bkg_photons),LineColor(kRed),LineStyle(kDashed));

		photon_pdf->plotOn(NphotonsPlot,Components(*aero_NPhotons),LineColor(kRed),LineStyle(kDashed));

	//	signal_NPhotons->paramOn(NphotonsPlot, Parameters(PlotParametersToPrint), Format("NELU", AutoPrecision(1)), Layout(0.60, 0.9, 0.9));
		// Draw histogram
		NphotonsPlot->Draw();
		t->Draw("SAME");
		NphotonsPlot->addObject(t);

		string betaCountStr = NumToString(BetaCutCounter);
		TString TbetaCountStr = "Rich1beta"+betaCountStr;
		TString TbetaCountStr2 = "../output/Rich1Gas/"+TbetaCountStr+".pdf";

		NphotonsPlot->Write(TbetaCountStr);
		pad1->Update();
		pad1->Modified();
		pad1->SaveAs(TbetaCountStr2);
		//Save as a .pdf

		//can1->Print(TbetaCountStr2);

		delete photon_pdf;
		//delete bkg_photons;
		//delete signal_NPhotons;
		//delete aero_NPhotons;
	//	delete ReducedDataSet;

     }
     delete bkg_photons;
     delete signal_NPhotons;
     delete aero_NPhotons;
     std::vector<double>::iterator it2;
	 for ( it2=FittedMean1.begin(); it2 < FittedMean1.end(); it2++ )
	 {
		cout << "HERE:" << *it2 << endl;
		//	BetaVNphotons->Fill(Beta[i],FitMean[i]);
	 //	BetaVNphotons->Fill(Beta[i],24);
	 }

     FitBeta(FittedMean1);
   //  outFile->Write();
     //delete data;
}


void RichYield::PlotNPhotonsRich2() {

	TString RICH = "R2Gas";
	int start_range = 5;
	int end_range = 40;
		//Open ROOT file for writting histograms to
	TCanvas *canR2 = new TCanvas("Rich2","transparent pad",200,10,700,500);
	canR2->SetFillStyle(4000);
	//TCanvas canR2("Rich2","NPhotonsRich2",200,10,700,500);
	/*
	 * Set Styles
	 */
	canR2->SetFrameFillStyle(4000);
	gStyle->SetOptStat(10);
    gStyle->SetOptFit(1111);

    TPad *pad2 = new TPad("pad2","",0,0,1,1);
    pad2->SetFillStyle(4000); //will be transparent
    pad2->SetFrameFillStyle(4000);
    pad2->Draw();
    pad2->cd();


    // Get required data
    TFile * inputFile = TFile::Open("/Users/mcoombes/Documents/ComputerWork/HeavyParticleSearch/ParticleFinderTuple/Output/ParticleSearchTrack-R2Gas-MC10-sigYield.root");
    //TH1* inputHisto = (TH1*) inputFile->Get("PhotonsPerTrack");
    TTree* inputTree = (TTree*)inputFile->Get("RichParticleSearchR2GasTrackTuple");
    //Define the observables and ranges over which the PDFs will be made
    RooRealVar NPhotons("PhotonsPerTrack","PhotonsPerTrack",start_range,end_range);


    // =========================================================================================================================
    // 													Cuts on Beta
    // =========================================================================================================================
    RooRealVar beta("beta","beta",0.0,2.0);

     //And create a RooDataSet object containing these observables:
     //	For > 1 observable
   // RooDataSet* data = new RooDataSet("data","data",inputTree,RooArgList(NPhotons,beta));
    RooDataSet data("data","data",inputTree,RooArgList(NPhotons,beta));

     //	For  1 observable
  //  RooDataHist* ReducedDataSet = new RooDataHist("data","data",NPhotons,inputHisto);
    //RooDataHist ReducedDataSet("data","data",NPhotons,inputHisto);
     // =========================================================================================================================
     // 													Cuts on Beta
     // =========================================================================================================================
     std::vector<TString> BetaCuts = CutsOnBeta();

     int BetaCutCounter = 0;

	 std::vector<double> FittedMean1;

	 TFile outFile("../output/Yield-"+RICH+".root", "RECREATE");

     for (it = BetaCuts.begin(); it < BetaCuts.end(); it++)
     {

    	cout << *it << endl;
		TString BetaCut = *it;
    	BetaCutCounter++;
		RooDataSet* ReducedDataSet = (RooDataSet*)data.reduce(BetaCut);
    	//RooDataSet ReducedDataSet = (RooDataSet)data.reduce(BetaCut);

		 // Draw data
		RooPlot* NphotonsPlot = NPhotons.frame();
		ReducedDataSet->plotOn(NphotonsPlot, Binning (end_range-start_range));
		NphotonsPlot->SetFillStyle(4000);
		NphotonsPlot->Draw();

		// =========================================================================================================================
		// Now Fitting
		// =========================================================================================================================

		//Signal

		RooRealVar sig_photons_mean("NPhotons","mean number of photons",27,20,30);
		RooRealVar sig_photons_sigma("#sigma","width of gaussian",5.8,2,9);

		 RooRealVar sig_photons_alpha("#alpha","alpha of gaussian",-0.004,-1,1);
		 RooAbsPdf* signal_NPhotons = new RooGenericPdf("Generic","Generic PDF","exp(-1*(((@0)-(@1))^(2))/(2*(@2)^(2)))*TMath::Freq(@3*(@0-@1)/@2)",
					 RooArgList(NPhotons,sig_photons_mean,sig_photons_sigma,sig_photons_alpha));
		// Define the signal Fit function
		//RooAbsPdf* signal_NPhotons = new RooGaussian("signal_photons","signal_photons",NPhotons,sig_photons_mean,sig_photons_sigma);

		//RooAbsPdf* signal_NPhotons = new RooPoisson("signal_photons","signal_photons",NPhotons,sig_photons_mean);
	   // RooAbsPdf* signal_NPhotons = new RooGenericPdf("Generic","Generic PDF","exp(-1*@1)*(@1)^(@0)/(TMath::Factorial(@0))",RooArgList(NPhotons,sig_photons_mean));
	  //  RooAbsPdf* signal_NPhotons = new RooPoisson("signal_photons","signal_photons",NPhotons,sig_photons_mean);

		// Define Yields from 0 to Nentries as we don't know the yield prior to fitting
		int NEntries = ReducedDataSet->numEntries();
	 //   RooRealVar Nsig("Nsig","Nsig",0.0,NEntries);
		RooRealVar Nsig("Nsig","Nsig",0.0,NEntries);

		//Background
		RooRealVar bkg_photons_a("a","a of exponential",-0.067, -0.15, 0);
//		RooRealVar bkg_photons_p0("p0","p0 of Polynomial",-0.02, -0.025, 0);
//		RooRealVar bkg_photons_p1("p1","p1 of Polynomial",0, 0,20);


		// Background function
	 //   RooAbsPdf* bkg_photons = new RooPolynomial("Polynomial","O(1) Polynomial",NPhotons,RooArgList(bkg_photons_p0,bkg_photons_p1));
	  //  bkg_photons->plotOn(NphotonsPlot);
		RooAbsPdf* bkg_photons = new RooExponential("Exponential","Exponential",NPhotons,bkg_photons_a);

		RooRealVar Nbkg("Nbkg","Nbkg",0.0,NEntries);

		// Combine to make PDF
		RooAbsPdf *photon_pdf = new RooAddPdf("photon_pdf","photon_pdf",RooArgList(*signal_NPhotons,*bkg_photons),RooArgList(Nsig,Nbkg));

		// Now Fit the PDF to the data
		RooFitResult *photon_result = photon_pdf->fitTo(*ReducedDataSet,Extended(),Save());

		//print result
		//photon_result->Print();

		/*
		 * Update the plot to show fit results
		 */
		photon_pdf->plotOn(NphotonsPlot);




		// Add parameters to Fit
		RooArgSet PlotParametersToPrint("PlotParametersToPrint");
//		PlotParametersToPrint.add(sig_photons_mean);
//		PlotParametersToPrint.add(sig_photons_sigma);
//		PlotParametersToPrint.add(bkg_photons_a);

		double mean = sig_photons_mean.getVal() + sig_photons_sigma.getVal()*(sig_photons_alpha.getVal()/sqrt(1+sig_photons_alpha.getVal()*sig_photons_alpha.getVal()))*sqrt(2/3.142);

		FittedMean1.push_back(mean);

		//error on mean

		double errMean = sqrt(pow(sig_photons_mean.getError(),2.0)+pow(sig_photons_sigma.getError(),2.0));
		TString errString = NumToString( errMean );


		cout <<"mean: "<<  mean << endl;
		TString Ystr = NumToString( mean );
		Ystr = "NPhotons "+Ystr+ " #pm"+ errString;
		t = new TLatex ( .62, .89, Ystr );
		t->SetNDC(kTRUE);
		t->SetTextSize(0.045);
		t->SetTextFont(62);

		//The signal component only in dashed blue:
		photon_pdf->plotOn(NphotonsPlot,Components(*signal_NPhotons),LineColor(kBlue),LineStyle(kDashed));
		//	The background component only in dashed red:
		photon_pdf->plotOn(NphotonsPlot,Components(*bkg_photons),LineColor(kRed),LineStyle(kDashed));

		//signal_NPhotons->paramOn(NphotonsPlot, Parameters(PlotParametersToPrint), Format("NELU", AutoPrecision(1)), Layout(0.60, 0.9, 0.9));

		// Draw histogram
		NphotonsPlot->SetFillStyle(4000);
		NphotonsPlot->Draw();
		t->Draw("SAME");
		NphotonsPlot->addObject(t);
		string betaCountStr = NumToString(BetaCutCounter);
		TString TbetaCountStr = "Rich2beta"+betaCountStr;
		TString TbetaCountStr2 = "../output/Rich2Gas/"+TbetaCountStr+".pdf";

		// Write to ROOT file
		NphotonsPlot->Write(TbetaCountStr);

		pad2->Update();
		pad2->Modified();
		pad2->SaveAs(TbetaCountStr2);
//		delete photon_pdf;
		delete bkg_photons;
		delete signal_NPhotons;

     }
   //  outFile->Write();
     //delete data;
     FitBeta(FittedMean1);
}

void RichYield::PlotNPhotonsAerogel() {

	TString RICH = "Aerogel";
	int start_range = 2;
	int end_range = 40;
		//Open ROOT file for writting histograms to
	TCanvas *canR2 = new TCanvas("Rich2","transparent pad",200,10,700,500);
	canR2->SetFillStyle(4000);
	//TCanvas canR2("Rich2","NPhotonsRich2",200,10,700,500);
	/*
	 * Set Styles
	 */
	canR2->SetFrameFillStyle(4000);
	gStyle->SetOptStat(10);
    gStyle->SetOptFit(1111);

    TPad *pad2 = new TPad("pad2","",0,0,1,1);
    pad2->SetFillStyle(4000); //will be transparent
    pad2->SetFrameFillStyle(4000);
    pad2->Draw();
    pad2->cd();


    // Get required data
    TFile * inputFile = TFile::Open("/Users/mcoombes/Documents/ComputerWork/HeavyParticleSearch/ParticleFinderTuple/Output/ParticleSearchTrack-Aerogel-MC10-sigYield.root");
    //TH1* inputHisto = (TH1*) inputFile->Get("PhotonsPerTrack");
    TTree* inputTree = (TTree*)inputFile->Get("RichParticleSearchAerogelTrackTuple");
    //Define the observables and ranges over which the PDFs will be made
    RooRealVar NPhotons("PhotonsPerTrack","PhotonsPerTrack",start_range,end_range);


    // =========================================================================================================================
    // 													Cuts on Beta
    // =========================================================================================================================
    RooRealVar beta("beta","beta",0.0,2.0);

     //And create a RooDataSet object containing these observables:
     //	For > 1 observable
   // RooDataSet* data = new RooDataSet("data","data",inputTree,RooArgList(NPhotons,beta));
    RooDataSet data("data","data",inputTree,RooArgList(NPhotons,beta));

     // =========================================================================================================================
     // 													Cuts on Beta
     // =========================================================================================================================
     std::vector<TString> BetaCuts = CutsOnBeta();

     int BetaCutCounter = 0;
	 TFile outFile("../output/Yield-"+RICH+".root", "RECREATE");

     for (it = BetaCuts.begin(); it < BetaCuts.end(); it++)
     {

    	cout << *it << endl;
		TString BetaCut = *it;
    	BetaCutCounter++;
		RooDataSet* ReducedDataSet = (RooDataSet*)data.reduce(BetaCut);
    	//RooDataSet ReducedDataSet = (RooDataSet)data.reduce(BetaCut);

		 // Draw data
		RooPlot* NphotonsPlot = NPhotons.frame();
		ReducedDataSet->plotOn(NphotonsPlot, Binning (end_range-start_range));
		NphotonsPlot->SetFillStyle(4000);
		NphotonsPlot->Draw();

		// =========================================================================================================================
		// Now Fitting
		// =========================================================================================================================

		//Signal

		RooRealVar sig_photons_mean("NPhotons","mean number of photons",2,0,20);
		RooRealVar sig_photons_sigma("#sigma","width of gaussian",10,0,100);
		RooRealVar sig_photons_alpha("#alpha","alpha of gaussian",-2,-20,20);
	//	RooAbsPdf* signal_NPhotons = new RooGenericPdf("Generic","Generic PDF","exp(-1*(((@0)-(@1))^(2))/(2*(@2)^(2)))*TMath::Freq(@3*(@0-@1)/@2)",
	//			RooArgList(NPhotons,sig_photons_mean,sig_photons_sigma,sig_photons_alpha));

		// Define the signal Fit function
		RooAbsPdf* signal_NPhotons = new RooGaussian("signal_photons","signal_photons",NPhotons,sig_photons_mean,sig_photons_sigma);

		//RooAbsPdf* signal_NPhotons = new RooPoisson("signal_photons","signal_photons",NPhotons,sig_photons_mean);
	   // RooAbsPdf* signal_NPhotons = new RooGenericPdf("Generic","Generic PDF","exp(-1*@1)*(@1)^(@0)/(TMath::Factorial(@0))",RooArgList(NPhotons,sig_photons_mean));
	  //  RooAbsPdf* signal_NPhotons = new RooPoisson("signal_photons","signal_photons",NPhotons,sig_photons_mean);

		// Define Yields from 0 to Nentries as we don't know the yield prior to fitting
		int NEntries = ReducedDataSet->numEntries();
	 //   RooRealVar Nsig("Nsig","Nsig",0.0,NEntries);
		RooRealVar Nsig("Nsig","Nsig",0.0,NEntries);

		//Background
		RooRealVar bkg_photons_a("a","a of exponential",-0.1, -0.2, 0);
		RooRealVar bkg_photons_p0("p0","p0 of Polynomial",-0.02, -0.025, 0);
		RooRealVar bkg_photons_p1("p1","p1 of Polynomial",0, 0,20);


		// Background function
	 //   RooAbsPdf* bkg_photons = new RooPolynomial("Polynomial","O(1) Polynomial",NPhotons,RooArgList(bkg_photons_p0,bkg_photons_p1));
	  //  bkg_photons->plotOn(NphotonsPlot);
		RooAbsPdf* bkg_photons = new RooExponential("Exponential","Exponential",NPhotons,bkg_photons_a);

		RooRealVar Nbkg("Nbkg","Nbkg",0.0,NEntries);

		// Combine to make PDF
		RooAbsPdf *photon_pdf = new RooAddPdf("photon_pdf","photon_pdf",RooArgList(*signal_NPhotons,*bkg_photons),RooArgList(Nsig,Nbkg));

		// Now Fit the PDF to the data
		RooFitResult *photon_result = photon_pdf->fitTo(*ReducedDataSet,Extended(),Save());

		//print result
		//photon_result->Print();

		/*
		 * Update the plot to show fit results
		 */
		photon_pdf->plotOn(NphotonsPlot);

		// Add parameters to Fit
		RooArgSet PlotParametersToPrint("PlotParametersToPrint");
		PlotParametersToPrint.add(sig_photons_mean);
		PlotParametersToPrint.add(sig_photons_sigma);

		//The signal component only in dashed blue:
		photon_pdf->plotOn(NphotonsPlot,Components(*signal_NPhotons),LineColor(kBlue),LineStyle(kDashed));
		//	The background component only in dashed red:
		photon_pdf->plotOn(NphotonsPlot,Components(*bkg_photons),LineColor(kRed),LineStyle(kDashed));

		signal_NPhotons->paramOn(NphotonsPlot, Parameters(PlotParametersToPrint), Format("NELU", AutoPrecision(1)), Layout(0.60, 0.9, 0.9));
		// Draw histogram
		NphotonsPlot->SetFillStyle(4000);
		NphotonsPlot->Draw();

		string betaCountStr = NumToString(BetaCutCounter);
		TString TbetaCountStr = "Aerogel"+betaCountStr;
		TString TbetaCountStr2 = "../output/Aerogel/"+TbetaCountStr+".pdf";

		// Write to ROOT file
		NphotonsPlot->Write(TbetaCountStr);

		pad2->Update();
		pad2->Modified();
		pad2->SaveAs(TbetaCountStr2);

//		delete photon_pdf;
		delete bkg_photons;
		delete signal_NPhotons;

     }
   //  outFile->Write();
     //delete data;
}



