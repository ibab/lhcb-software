// TxTool.C
//
// this root macro will make a histogram of the slopes of straight lines
// connecting two OT X hits on the same track
//
// author: Manuel Schiller
// date: 2006-11-16

#define TxTool_cxx
#include "TxTool.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void TxTool::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L TxTool.C
//      Root > TxTool t
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

   TFile f("TxTool.root", "Recreate");
   TH1D txgood("txgood", "txgood", 100, -1., 1.);
   TH1D txghost("txghost", "txghost", 100, -1., 1.);

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
      for (int nTrack = 0; nTrack < nSavedTracks; ++nTrack) {
	 // only tracks with 2 or more OT hits
	 if (Tracks_nTotalOT[nTrack] <= 1.)
		 continue;
	 // work out if we have a ghost or are missing MC truth due to NTuple
	 // limitation
	 bool isGhost = true;
	 bool mcIsMissing = false;
	 for (int k = 0; k < 10; ++k) {
	    if (-2. == Tracks_assocMCParts[nTrack][k]) {
	       mcIsMissing = true;
	       break;
	    }
	    if ((Tracks_assocMCParts[nTrack][k] >= 0.) &&
			    (Tracks_assocMCPartWeights[nTrack][k] >= 0.7)) {
	       isGhost = false;
	       break;
	    }
	 }
	 // make sure we skip tracks with missing MC information due to NTuple
	 // limitations
	 if (mcIsMissing)
		 continue;
	 double p = 1. / fabs(Tracks_firstState[nTrack][5]);
	 double pt = p * sqrt(1. - 1. /
			 (1. + pow(double(Tracks_firstState[nTrack][3]), 2.) +
			  pow(double(Tracks_firstState[nTrack][4]), 2.)));

	 // next line is just an example how to cut on pt
	 // if (pt < 1.) continue;

	 // look for hits in adjacent layers
	 for (int i = 0; i < 32; ++i) {
	    // check if hit there and X measurement
	    if (Tracks_OTLayer[nTrack][i] < 0.) continue;
	    if (Tracks_OTMeasType[nTrack][i] != 1.) continue;
	    for (int j = i; j < 32; ++j) {
	       // check if hit there and X measurement
	       if (Tracks_OTLayer[nTrack][j] < 0.) continue;
	       if (Tracks_OTMeasType[nTrack][j] != 1.) continue;
	       // we do not want to have hits inside the same module
	       // to enter in our histograms
	       if (fabs(Tracks_OTLayer[nTrack][i] - Tracks_OTLayer[nTrack][j]) != 1.)
		       continue;
	       // ok, not inside same module
	       double dx, dz;
	       dz = Tracks_OTz[nTrack][j] - Tracks_OTz[nTrack][i];
	       // if we have the true X information available, use it,
	       // otherwise, we put things into the "ghost" histogram
	       // and use the reconstructed X information...
	       if ((Tracks_OTTCxx[nTrack][i] != -1.) && (Tracks_OTTCxx[nTrack][j] != -1.))
		  dx = Tracks_OTTx[nTrack][j] - Tracks_OTTx[nTrack][i];
	       else
		  dx = Tracks_OTx[nTrack][j] - Tracks_OTx[nTrack][i];
	       if (isGhost)
		  txghost.Fill(dx/dz);
	       else
		  txgood.Fill(dx/dz);
	    }
	 }
      }
   }

   f.Write();
}
