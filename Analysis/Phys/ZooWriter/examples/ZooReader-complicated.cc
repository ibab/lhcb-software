/*
 * @file ZooReader-complicated.cc
 *
 * Usage example, sligtly more complicated, and somewhat dated,
 * but still compiles and runs
 *
 * this stripped-down example is taken from what eventually grew
 * into the downstream-downstream K^0_S analysis which contributed
 * significantly to the very first LHCb publication on K^0_S
 * production with 2009 data; it gives a more realistic feeling of
 * what "Zoo code" is like
 *
 * this used to work with ../options/standalone-complicated.py, but
 * DaVinci changes so quickly that that option file probably needs
 * quite a bit of tweaking to make it run with DaVinci again.
 * however, the documentation of ZooWriter options in there is very
 * good.
 *
 * AZ SHM, PI 2008
 */

#include <iostream>
#include <boost/shared_ptr.hpp>

#include <TFile.h>
#include <TRandom.h>
#include <TTree.h>
#include <TClassTable.h>
#include <TH1F.h>
#include <TSystem.h>
#include <TCanvas.h>
#include <TChain.h>
#include <TROOT.h>
#include <TStyle.h>

#include "Zoo/Zoo.h"

typedef ROOT::Math::SVector<double, 7> Vec7;
typedef ROOT::Math::SVector<double, 3> Vec3;

bool selection(const ZooP* p)
{
    //Ks and Lambda mostly decay in two particles, so lets filter out anything
    //else
    if (2 != p->NCh()) return false;
    //now get the daughters
    const ZooP* Ch1 = (*p)[0];
    const ZooP* Ch2 = (*p)[1];
    //We want a momentum cut to filter out low momentum stuff
    if(Ch1->momentum().P() < 2000. || Ch2->momentum().P() < 2000.)
	return false;
    //Same for transverse momentum
    if(Ch1->momentum().Pt() < 50. || Ch2->momentum().Pt() < 50.)
	return false;
    //We want the particle to have a good vertex
    if(p->chi2()/p->nDoF() > 15.) return false;
    //And a reasonable flight distance
    if(p->fDistance() < 10.) return false;
    //We want the particle to have a good vertex
    if(Ch1->TrInfo()->chi2()/Ch1->TrInfo()->chi2() > 15.) return false;
    if(Ch2->TrInfo()->chi2()/Ch2->TrInfo()->chi2() > 15.) return false;
    
    return true;
}

int main(int argc, char** argv)
{
    //Define TChain that will later contain all the tuple files. By adding more
    //tuple files it is possible to read in more than one file, e.g. from
    //splitted sub-jobs
    boost::shared_ptr<TChain> chain(new TChain("Forest"));
    chain->Add("$ZOOROOT/options/Data.root");

    //Define Zoo objects that will later contain the event and the particle
    //branches. 
    //NB: Pointers have to be created as NULL pointers!
    ZooEv *pev = 0;
    ZooObjRefArray<ZooP> *myKs = 0;
    ZooObjRefArray<ZooP> *myLambda = 0;

    //Assigning branch names in the tuple to the pointers
    chain->SetBranchAddress("Event", &pev);
    chain->SetBranchAddress("MyKaons", &myKs);
    chain->SetBranchAddress("MyLambdas", &myLambda);

    TH1::AddDirectory(kFALSE);
    //define some aethetic options

    gROOT->SetStyle("Plain");
    gStyle->SetOptStat(1111111);
    gStyle->SetOptFit(1111);
    gStyle->SetPalette(1);
    gStyle->SetHistLineWidth(2);

    //define the histos to be filled
    TH1F *preselKs = new TH1F("ks_mass_presel","Ks mass after preselection;candidates;mass [MeV]",100,396.,596.);
    TH1F *preselL = new TH1F("lambda_mass_presel","Ks mass after preselection;candidates;mass [MeV]",100,1015,1215);
    TH1F *selKs = new TH1F("ks_mass_sel","Ks mass after preselection;candidates;mass [MeV]",100,396.,596.);
    TH1F *selL = new TH1F("lambda_mass_sel","Ks mass after preselection;candidates;mass [MeV]",100,1015,1215);
    TH1F *mcKs = new TH1F("ks_mass_mc","Ks mass after preselection;candidates;mass [MeV]",100,396.,596.);
    TH1F *mcL = new TH1F("lambda_mass_mc","Ks mass after preselection;candidates;mass [MeV]",100,1015,1215);


    int nMCKs = 0;
    int nMCLambdas = 0;
    //do we run over Monte Carlo, i.e., we want to try truth matching
    bool mc = false;

    //loop over all entries of our chain, i.e. over all events
    for(int i=0;i<chain->GetEntries();i++){
	//get event number i
	chain->GetEntry(i);

	//loop over all MC particles to find out how many there are
	for (int m=0; m < pev->NMCP(); m++){
	    const ZooMCP *mcp = pev->MCP(m);
	    if(mcp->pid() == 310) nMCKs++;
	    if(mcp->pid() == 3122) nMCLambdas++;
	}


	//loop over all entries in our myKs branch, i.e. all candidates which
	//come from the corresponding selection in the current event
	for(int m = 0 ;m < myKs->size(); ++m){
	    //get Ks candidate
	    const ZooP *candidate = (*myKs)[m];

	    if (mc) {
		//get the MC associated particle if there is one
		const ZooMCP* mcks = candidate->assocMCP();
		//if the ks is associated, plot the mass in a special histogram
		if (mcks && mcks->pid() == 310) {
		    mcKs->Fill(candidate->momentum().M());
		}
	    }

	    //plot the mass of all candidates
	    preselKs->Fill(candidate->momentum().M());
	    //apply a simple selection to reduce background a little
	    if (selection(candidate)) selKs->Fill(candidate->momentum().M());
	}

	//Do the same for the lambdas
	for(int m = 0; m < myLambda->size(); ++m){
	    
	    const ZooP *candidate=(*myLambda)[m];

	    if (mc) {
		const ZooMCP* mcl =  candidate->assocMCP();
		if (mcl && mcl->pid() == 3122) {
		    mcL->Fill(candidate->momentum().M());
		}
	    }

	    preselL->Fill(candidate->momentum().M());
	    if (selection(candidate)) selL->Fill(candidate->momentum().M());
	}

    }

    TCanvas *c1 = new TCanvas("c1","Sample Plots",200,10,700,900);
    c1->Divide(2,2);
    c1->cd(1);
    preselKs->Draw();
    c1->cd(2);
    preselL->Draw();
    c1->cd(3);
    selKs->Draw();
    c1->cd(4); 
    selL->Draw();
    c1->Print("plots.eps");

    return 0;
}

// vim:sw=4:tw=78:ft=cpp
