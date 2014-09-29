#include <sstream>
#include "TFile.h"
#include "TTree.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TCanvas.h"
#include "TStyle.h"
#include <string>
#include <iostream>
#include <TMath.h>

using namespace std;

int main(int argc, char **argv)
{
	string fileName = "Rad_merged.root";
	string path = "plots/";
	string type = "rad";

	if(argc > 1)
		fileName = argv[1];
	if(argc > 3)
		path = argv[3];
	if(argc > 2)
		type = argv[2];


	TFile * f = TFile::Open(fileName.c_str());
	TTree * tree = (TTree *)f->Get("RadLengthColl/tree");
	if( f!=0 && tree !=0 ) cout << "Reading: " << fileName << endl;
	else cout << "File or tree not found" << endl;

	int nplanes = 11;
	TString select, var, namehisto, namefile;
	TCanvas * c = new TCanvas();
		
	TGraphErrors * cumul = new TGraphErrors();
	TGraphErrors * cumulZ = new TGraphErrors();
	TGraphErrors * p2p = new TGraphErrors();
	
	
	for(int i = 2; i < nplanes+1; i++)
	{
		c->SetLogy();
		select = Form("ID == %i",i);
		namehisto = Form("cum_lgh_%i",i);
		var = "cum"+type+"lgh>>"+namehisto;
		tree->Draw(var,select);
		TH1F * h1 = (TH1F *)gPad->GetPrimitive(namehisto);
		h1->GetXaxis()->SetTitle("n_{X0}^{tot}");
		h1->GetYaxis()->SetTitle("N_{evt}");
		cumul->SetPoint(i-1,i,h1->GetMean());
		cumul->SetPointError(i-1,0,h1->GetRMS()/TMath::Sqrt((double)h1->GetEntries()));
		namefile = path + namehisto + "_" + type + ".pdf";
		c->Print(namefile);
		
		namehisto = Form("Z_%i",i);
		var = "Zpos>>" + namehisto;
		tree->Draw(var,select);
		TH1F * hZ = (TH1F *)gPad->GetPrimitive(namehisto);
		cumulZ->SetPoint(i-1,hZ->GetMean(),h1->GetMean());
		cumulZ->SetPointError(i-1,hZ->GetRMS()/TMath::Sqrt((double)hZ->GetEntries()),h1->GetRMS()/TMath::Sqrt((double)h1->GetEntries()));
		

		namehisto = Form("p2p_lgh_%i",i);
		var = "p2p"+type+"lgh>>" + namehisto;
		tree->Draw(var,select);
		TH1F * h2 = (TH1F *)gPad->GetPrimitive(namehisto);
		h2->GetXaxis()->SetTitle("n_{X0}^{p2p}");
		h2->GetYaxis()->SetTitle("N_{evt}");
		p2p->SetPoint(i-1,i,h2->GetMean());
		p2p->SetPointError(i-1,0,h2->GetRMS()/TMath::Sqrt((double)h2->GetEntries()));
		namefile = path + namehisto + "_" + type + ".pdf";
		c->Print(namefile);

        /*
		c->SetLogy(0);
		namehisto << "eta_" << i;
		var << "eta>>" << namehisto.str() << "(80,1.5,5.5)";
		tree->Draw(var.str().c_str(),select.str().c_str());
		TH1F * h3 = (TH1F *)gPad->GetPrimitive(namehisto.str().c_str());
		h3->GetXaxis()->SetTitle("n_{X0}^{p2p}");
		h3->GetYaxis()->SetTitle("N_{evt}");
		h3->Draw();
		namefile << path << namehisto.str() << "_" << type << ".pdf";
		c->Print(namefile.str().c_str());
		*/

		c->SetLogy(0);
		gStyle->SetOptStat(0);
		namehisto = Form("length_prof_%i",i);
		var = "cum"+type+"lgh:eta:phi>>" + namehisto + "(80,-3.3,3.3,80,2.,5.)";
		tree->Draw(var,select,"profs");
		TH1 * hh = (TH1 *)gPad->GetPrimitive(namehisto);
		hh->GetXaxis()->SetTitle("#phi");
		hh->GetYaxis()->SetTitle("#eta");
		if(type=="inter") { c->SetLogz(); hh->SetMinimum(20); }
		hh->Draw("colz");
		namefile = path + namehisto + "_" + type + ".pdf";
		c->Print(namefile);

		gStyle->SetOptStat(0);
		namehisto = Form("etaphi_%i",i);
		var = "eta:phi>>" + namehisto;// + "(80,-3.3,3.3,80,2.,5.)";
		tree->Draw(var,select,"colz");
		TH1 * hhetaphi = (TH1 *)gPad->GetPrimitive(namehisto);
		hhetaphi->GetXaxis()->SetTitle("#phi");
		hhetaphi->GetYaxis()->SetTitle("#eta");
		namefile = path + namehisto + "_" + type + ".pdf";
		c->Print(namefile);
	}

	
	gStyle->SetOptStat(0);
	c->SetLogy();
	

	tree->Draw(("cum"+type+"lgh:eta>>hh1(100,2,5)").c_str(),"ID == 11","prof");
	TH1 *radlgh_eta = (TH1 *)gPad->GetPrimitive("hh1");
		

	cumul->GetXaxis()->SetTitle("ID plane");
	cumul->GetYaxis()->SetTitle("<n_{X0}^{tot}>");
	cumulZ->GetXaxis()->SetTitle("Z (mm)");
	cumulZ->GetYaxis()->SetTitle("<n_{X0}^{tot}>");
	p2p->GetXaxis()->SetTitle("ID plane");
	p2p->GetYaxis()->SetTitle("<n_{X0}^{p2p}>");
	radlgh_eta->GetXaxis()->SetTitle("#eta");
	radlgh_eta->GetYaxis()->SetTitle("<n_{X0}^{tot}>");
	
	cumul->SetMarkerStyle(22);
	cumul->SetMarkerSize(0.8);
	cumul->SetMarkerColor(1);
	cumulZ->SetMarkerStyle(22);
	cumulZ->SetMarkerSize(0.8);
	cumulZ->SetMarkerColor(1);
	p2p->SetMarkerStyle(22);
	p2p->SetMarkerSize(0.8);
	p2p->SetMarkerColor(1);
	radlgh_eta->SetMarkerStyle(22);
	radlgh_eta->SetMarkerSize(0.8);
	radlgh_eta->SetMarkerColor(1);
	
	p2p->Draw("AP");
	c->Print((path+"p2pRadLength"+type+".pdf").c_str());
	cumul->Draw("AP");
	c->Print((path+"cumRadLength"+type+".pdf").c_str());
	cumulZ->Draw("AP");
	c->Print((path+"cumRadLengthvsZ"+type+".pdf").c_str());
	radlgh_eta->Draw();
	c->Print((path+"cumRadLengthvseta"+type+".pdf").c_str());
	
	return 0;
}
