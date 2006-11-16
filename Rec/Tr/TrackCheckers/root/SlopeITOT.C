// SlopeITOT.C
//
// this is a simple example of a script that draws histograms of the
// slope (tx) of tracks that have hits in both IT and OT
// 
// author: Manuel Schiller
// date: 2006-11-16

#define SlopeITOT_cxx
#include "SlopeITOT.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TProfile.h>

#include <set>
#include <map>
#include <string>
#include <iostream>

void SlopeITOT::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L SlopeITOT.C
//      Root > SlopeITOT t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   TFile f("SlopeITOT.root", "Recreate");
   
   TH1D angleRest("angleRest", "angular distribution of \"Rest\" tracks with hits in both IT and OT",
       100, -M_PI / 2., M_PI / 2.);
   TH1D angleLong("angleLong", "angular distribution of Long tracks with hits in both IT and OT",
       100, -M_PI / 2., M_PI / 2.);

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;

      // ok, loop over all tracks
      for (int nTrack = 0; nTrack < nSavedTracks; ++nTrack) {
	double p = 1. / fabs(Tracks_firstState[nTrack][5]);
	// now only tracks with p >= 2 GeV
	if (p < 2.) continue;
	// ok, find tracks that hit both IT and OT, skip the rest
	if ((Tracks_nTotalIT[nTrack] > 0.) &&
	    (Tracks_nTotalOT[nTrack] > 0.))
	  continue;

	// ok, work out the angle for all Tracks that have hits in both
	// IT and OT
	// we start with the average tx
	int ntx = 0;
	double tx = 0;
	for (int i = 0; i < 20; ++i)
	  if (Tracks_ITCxx[nTrack][i] > 0.)
	    ++ntx, tx += Tracks_ITtx[nTrack][i];
	for (int i = 0; i < 32; ++i)
	  if (Tracks_OTCxx[nTrack][i] > 0.)
	    ++ntx, tx += Tracks_OTtx[nTrack][i];
	tx /= (double) ntx;

	if (3 == Tracks_type[nTrack])
	  angleLong.Fill(atan(tx));
	else
	  angleRest.Fill(atan(tx));
      }
   }

   f.Write();
}
