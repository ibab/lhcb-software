/** @file ZooReader
 *
 * read Zoo tuples and make a few plots (very simple example)
 *
 * see ../options/standalone-simple.py for an example of an option file
 * to go with this example
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2011-09-30
 */

#include <cmath>
#include <iostream>

#include <boost/shared_ptr.hpp>

#include <TChain.h>
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TStyle.h>

#include "Zoo/Zoo.h"

int main(int argc, char** argv)
{
    // Define TChain that will later contain all the tuple files. By adding more
    // tuple files it is possible to read in more than one file, e.g. from
    // splitted sub-jobs
    boost::shared_ptr<TChain> chain(new TChain("Forest"));
    chain->Add("MC.root");

    // Define Zoo objects that will later contain the event and the particle
    // branches. 
    // NB: Pointers have to be created as NULL pointers!
    ZooEv *pev = 0;
    ZooObjRefArray<ZooP> *myBs = 0;
    ZooObjRefArray<ZooP> *myDs = 0;
    ZooObjRefArray<ZooP> *myKaons = 0;

    // Assigning branch names in the tuple to the pointers
    chain->SetBranchAddress("Event", &pev);
    chain->SetBranchAddress("MyKaons", &myKaons);
    chain->SetBranchAddress("MyBs", &myBs);
    chain->SetBranchAddress("MyDs", &myDs);

    TH1::AddDirectory(kFALSE);

    // define some aethetic options
    gROOT->SetStyle("Plain");
    gStyle->SetOptStat(1111111);
    gStyle->SetOptFit(1111);
    gStyle->SetPalette(1);
    gStyle->SetHistLineWidth(2);

    // define the histos to be filled
    boost::shared_ptr<TH1D> mDs(new TH1D("mDs", "D_{s} mass;m [MeV];candidates", 300, 1800., 2100.));
    boost::shared_ptr<TH1D> pDs(new TH1D("pDs", "p of D_{s};p [GeV];candidates", 100, 0., 300.));
    boost::shared_ptr<TH1D> ptDs(new TH1D("ptDs", "p_{T} of D_{s};p_{T} [GeV];candidates", 100, 0., 50.));
    boost::shared_ptr<TH1D> mBs(new TH1D("mBs", "B_{s} mass;m [MeV];candidates", 300, 5000., 5600.));
    boost::shared_ptr<TH1D> pBs(new TH1D("pBs", "p of B_{s};p [GeV];candidates", 100, 0., 300.));
    boost::shared_ptr<TH1D> ptBs(new TH1D("ptBs", "p_{T} of B_{s};p_{T} [GeV];candidates", 100, 0., 50.));
    boost::shared_ptr<TH1D> KDLLKPi(new TH1D("KDLLKPi", "Kaon DLL(K-#pi);DLL(K-#pi);entries", 100, -100., 100.));

    // count Bs in MC
    unsigned long nTrueBs = 0;

    // loop over all entries of our chain, i.e. over all events
    const unsigned long nevents = chain->GetEntries();
    for(unsigned long i = 0; i < nevents; ++i) {
	// get event number i
	chain->GetEntry(i);
	if (0ul == (i % 1000ul))
	    std::cout << "Read event " << i << "/" << nevents << std::endl;

	// loop over all MC particles to find out how many there are
	for (unsigned j = 0; j < pev->NMCP(); ++j) {
	    const ZooMCP *mcp = pev->MCP(j);
	    if (531 == std::abs(mcp->pid())) ++nTrueBs;
	}

	// loop over Bs to do a mass, p, pt plots
	for (unsigned j = 0; j < myBs->size(); ++j) {
	    const ZooP *bs = (*myBs)[j];
	    mBs->Fill(bs->measuredMass());
	    ROOT::Math::PxPyPzEVector p = bs->momentum();
	    pBs->Fill(p.P() / 1e3);
	    ptBs->Fill(p.Pt() / 1e3);
	}

	// loop over Ds to do a mass, p, pt plots
	for (unsigned j = 0; j < myDs->size(); ++j) {
	    const ZooP *ds = (*myDs)[j];
	    mDs->Fill(ds->measuredMass());
	    ROOT::Math::PxPyPzEVector p = ds->momentum();
	    pDs->Fill(p.P() / 1e3);
	    ptDs->Fill(p.Pt() / 1e3);
	}

	// loop over all Kaons to make a DLL(K-pi) plot
	for (unsigned j = 0; j < myKaons->size(); ++j) {
	    const ZooP *k = (*myKaons)[j];
	    const ZooDLL *dll = k->DLL();
	    if (0 != dll)
		KDLLKPi->Fill(dll->DLLk());
	}
    } // end of loop over events

    std::cout << nTrueBs << " true Bs found in MC truth." << std::endl;

    // plot and save to eps
    boost::shared_ptr<TCanvas> c(new TCanvas);
    mBs->Draw("E1"); c->Print("mBs.eps");
    pBs->Draw("E1"); c->Print("pBs.eps");
    ptBs->Draw("E1"); c->Print("ptBs.eps");
    mDs->Draw("E1"); c->Print("mDs.eps");
    pDs->Draw("E1"); c->Print("pDs.eps");
    ptDs->Draw("E1"); c->Print("ptDs.eps");
    KDLLKPi->Draw("E1"); c->Print("KDLLKPi.eps");

    return 0;
}

// vim:sw=4:tw=78:ft=cpp
