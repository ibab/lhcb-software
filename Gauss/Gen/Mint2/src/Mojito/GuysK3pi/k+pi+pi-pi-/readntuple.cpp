/*
 *  readntuple.C
 *  
 *
 *  Created by Vicente Jose Rives-Molina on 27/11/2009.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

// we need to include this piece of code:

#include "Mint/DalitzEvent.h"
#include "Mint/DalitzEventList.h"
#include "TTree.h"
#include <iostream>
#include "TVector3.h"
#include "Mint/DalitzEventPattern.h"
#include "Mint/CLHEPSystemOfUnits.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"


using namespace std;

bool readntuple()
{
	bool dbThis=false;
	if(dbThis) cout << "about to read ntuple" << endl;
	//if(0==output_ntuple) return false;
	//if(output_ntuple->GetEntries() <=0) return false;
	//if(dbThis) cout << " number of entries: " << output_ntuple->GetEntries() << endl;
	//if(dbThis) cout << " number of variables " << output_ntuple->GetNvar() << endl;
	bool success=true;
	
	DalitzEventPattern pat (421, 321, 211, -211, -211);
	if (dbThis) cout << "the pdg values have been introduced" << endl;
	DalitzEventList eventList; // checked, no need of arguments
	
	// Let's read the momenta from the ntuple
	
	TFile *f = TFile::Open("k3pi_3770_dat_sig_v2.root");
	
	if (dbThis) cout << "we have read the file" << endl;
	
	float K[3]; // momentum of the kaon
	float fpm[3]; // momentum of the first pi-
	float pp[3]; // momentum of the pi+
	float spm[3]; // momentum of the second pi-
	
	TTree *output_ntuple = (TTree*)f->Get("output_ntuple");
	
	// we are going to write output_ntuple because the pointer is called like that, not because of the "file"
	
	output_ntuple->SetBranchAddress("kpcpx",&K[0]); 
	output_ntuple->SetBranchAddress("kpcpy",&K[1]);
	output_ntuple->SetBranchAddress("kpcpz",&K[2]);

	
	output_ntuple->SetBranchAddress("kmcpx",&pp[0]);
	output_ntuple->SetBranchAddress("kmcpy",&pp[1]);
	output_ntuple->SetBranchAddress("kmcpz",&pp[2]);
	
	
	output_ntuple->SetBranchAddress("ppcpx",&fpm[0]);
	output_ntuple->SetBranchAddress("ppcpy",&fpm[1]);
	output_ntuple->SetBranchAddress("ppcpz",&fpm[2]);
	
	
	output_ntuple->SetBranchAddress("pmcpx",&spm[0]);
	output_ntuple->SetBranchAddress("pmcpy",&spm[1]);
	output_ntuple->SetBranchAddress("pmcpz",&spm[2]);
	
	
	int maxEvents = output_ntuple->GetEntries();
	
	TH1F *hist_k = new TH1F("hist_k", "momentum of the K+", 100, -1, 2);
	TH1F *hist_fpm = new TH1F("hist_fpm", "momentum of the first pi-", 100, -1, 2);
	TH1F *hist_pp = new TH1F("hist_pp", "momentum of the pi+", 100, -1, 2);
	TH1F *hist_spm = new TH1F("hist_spm", "momentum of the second pi-", 100, -1, 2);
	
    for(int i=0; i< maxEvents; i++)
    {
		if(dbThis)
		{
			cout << "DalitzEventList::fromNtuple "
			<< " getting " << i << " th entry" << endl;
		}
		output_ntuple->GetEntry(i);
		if(dbThis) cout << " got it, " << i << "th event, spm[0] = " << spm[0] << endl;
		
		// success &= evt.fromNtuple(output_ntuple);
		// success &= evt.fromTree(output_ntuple);
	  	
		TVector3 dv(K[0]+fpm[0]+pp[0]+spm[0],K[1]+fpm[1]+pp[1]+spm[1],K[2]+fpm[2]+pp[2]+spm[2]); // the momentum of D as a vector
		TVector3 kv(K[0], K[1], K[2]); // the momentum of the kaon as a vector
		TVector3 fpmv(fpm[0],fpm[1],fpm[2]); // the momentum of the 1st pi- as a vector
		TVector3 ppv(pp[0],pp[1],pp[2]); // the momentum of the pi+ as a vector
		TVector3 spmv(spm[0],spm[1],spm[2]); // the momentum of the 2nd pi- as a vector
		vector<TVector3> vectorOfvectors; // array of vectors
		
		if(dbThis) cout <<	"the 4-momenta have been defined" << endl;
		
		vectorOfvectors.push_back(dv*GeV); // we say the order of the vectors in the vectorOfvectors
		vectorOfvectors.push_back(kv*GeV); // we include the 'GeV' to get the correct units, need to include CLHEPSystemOfUnits.h
		vectorOfvectors.push_back(ppv*GeV);
		vectorOfvectors.push_back(fpmv*GeV);
		vectorOfvectors.push_back(spmv*GeV);
		
		if(dbThis) cout <<"we have fed the vectorOfvectors" << endl;
		
		if(dbThis) cout << "about to make event" << endl;
		
		DalitzEvent evt(pat, vectorOfvectors);
		
		if(dbThis) cout << "s12 by hand: " << ( evt.p(1) + evt.p(2) ).Mag2() << endl;
		if(dbThis) cout << "adding event " << evt << endl;
		eventList.Add(evt); // this fills the event list
		
		if(dbThis) cout << " made event" << endl;
		if(! success)
		{
			cout << "ERROR in DalitzEventList::fromNtuple"
			<< ", call to DalitzEvent::fromNtuple returned false!"
			<< endl;
			return false;
		}
		
		if(dbThis) cout << " added event" << endl;
		
		// Let's draw the momentum of the 4 particles
	
		hist_k->Fill(sqrt(kv[0]*kv[0] + kv[1]*kv[1] + kv[2]*kv[2]));
		hist_fpm->Fill(sqrt(fpmv[0]*fpmv[0] + fpmv[1]*fpmv[1] + fpmv[2]*fpmv[2]));
		hist_pp->Fill(sqrt(ppv[0]*ppv[0] + ppv[1]*ppv[1] + ppv[2]*ppv[2]));
		hist_spm->Fill(sqrt(spmv[0]*spmv[0] + spmv[1]*spmv[1] + spmv[2]*spmv[2]));

    }
	
    DalitzHistoSet datH = eventList.histoSet();
	datH.save("plotsFromEventList.root");
	datH.draw("K3pi_CF_Cleo_");
	
	TFile g("hist_k.root", "RECREATE");
	hist_k->Write();
	
	TFile h("hist_fpm.root", "RECREATE");
	hist_fpm->Write();
	
	TFile m("hist_pp.root", "RECREATE");
	hist_pp->Write();
	
	TFile n("hist_spm.root", "RECREATE");
	hist_spm->Write();
	
    eventList.save("K3pi_CF_CleoEvents.root");
	if(dbThis) cout << "DalitzEventList::fromNtuple worked!" << endl;
    return success;
	
}

int main(){
	cout << " main called " << endl;
	return ! readntuple(); // It returns what readntuple gives as output
}
