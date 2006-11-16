// TrackTypeTool.C
//
// this root macro will produce histograms showing how many hits in each
// subdetector are produced by a track's associated MCParticle for different
// track types
//
// author: Manuel Schiller
// date: 2006-11-16

#define TrackTypeTool_cxx
#include "TrackTypeTool.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH1D.h>

#include <string>
#include <vector>
#include <map>
#include <iostream>

void TrackTypeTool::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L TrackTypeTool.C
//      Root > TrackTypeTool t
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

   TFile f("TrackTypeTool.root", "Recreate");

   // fill containers with the strings used to build histogram names
   std::vector<std::string> TrackTypes;	// track types
   TrackTypes.push_back("TypeUnknown"); TrackTypes.push_back("Velo");
   TrackTypes.push_back("VeloR");TrackTypes.push_back("Long");
   TrackTypes.push_back("Upstream"); TrackTypes.push_back("Downstream");
   TrackTypes.push_back("Ttrack");
   std::vector<std::string> Variables;	// variables to monitor per track type
   // Velo
   Variables.push_back("VeloStations"); Variables.push_back("VeloRStations");
   Variables.push_back("VeloPhiStations"); Variables.push_back("VeloHits");
   Variables.push_back("VeloRHits"); Variables.push_back("VeloPhiHits");
   // TT
   Variables.push_back("TTStations"); Variables.push_back("TTHits");
   Variables.push_back("TTXStations"); Variables.push_back("TTXHits");
   Variables.push_back("TTUStations"); Variables.push_back("TTUHits");
   Variables.push_back("TTVStations"); Variables.push_back("TTVHits");
   Variables.push_back("TTStereoStations"); Variables.push_back("TTStereoHits");
   // IT
   Variables.push_back("ITStations"); Variables.push_back("ITHits");
   Variables.push_back("ITXStations"); Variables.push_back("ITXHits");
   Variables.push_back("ITUStations"); Variables.push_back("ITUHits");
   Variables.push_back("ITVStations"); Variables.push_back("ITVHits");
   Variables.push_back("ITStereoStations"); Variables.push_back("ITStereoHits");
   // OT
   Variables.push_back("OTStations"); Variables.push_back("OTHits");
   Variables.push_back("OTXStations"); Variables.push_back("OTXHits");
   Variables.push_back("OTUStations"); Variables.push_back("OTUHits");
   Variables.push_back("OTVStations"); Variables.push_back("OTVHits");
   Variables.push_back("OTStereoStations"); Variables.push_back("OTStereoHits");
   // T = IT + OT
   Variables.push_back("TStations"); Variables.push_back("THits");
   Variables.push_back("TXStations"); Variables.push_back("TXHits");
   Variables.push_back("TUStations"); Variables.push_back("TUHits");
   Variables.push_back("TVStations"); Variables.push_back("TVHits");
   Variables.push_back("TStereoStations"); Variables.push_back("TStereoHits");

   // fill a map with histograms so that filling things in is easier
   std::map<int, map<std::string, TH1D*> > histmap;
   for (int tr = 0; tr < TrackTypes.size(); ++tr) {
     for (int vars = 0; vars < Variables.size(); ++vars) {
       std::string id(TrackTypes[tr]);
       std::string &v = Variables[vars];
       id.append("_");
       id.append(v);
       histmap[tr][v] = new TH1D(id.c_str(), id.c_str(), 50, 0., 50.);
     }
   }
   
   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;

      // ok, loop over all tracks
      for (int nTrack = 0; nTrack < nSavedTracks; ++nTrack) {
	// to keep things simple, we'll just check those particles with
	// correct MC truth...
	if (Tracks_assocMCParts[nTrack][0] < 0.) continue;
	if (Tracks_assocMCPartWeights[nTrack][0] < 0.7) continue;

	int type = (int) Tracks_type[nTrack];
	int mcPart = (int) Tracks_assocMCParts[nTrack][0];
	
	// ok, fill in hit statistics for the different detector parts
	histmap[type]["VeloStations"]->Fill(MCParticles_nVeloStations[mcPart]);
	histmap[type]["VeloRStations"]->Fill(MCParticles_nVeloRStations[mcPart]);
	histmap[type]["VeloPhiStations"]->Fill(MCParticles_nVeloPhiStations[mcPart]);
	histmap[type]["VeloHits"]->Fill(MCParticles_nVeloHits[mcPart]);
	histmap[type]["VeloRHits"]->Fill(MCParticles_nVeloRHits[mcPart]);
	histmap[type]["VeloPhiHits"]->Fill(MCParticles_nVeloPhiHits[mcPart]);
	
	histmap[type]["TTStations"]->Fill(MCParticles_nTTStations[mcPart]);
	histmap[type]["TTHits"]->Fill(MCParticles_nTTHits[mcPart]);
	histmap[type]["TTXStations"]->Fill(MCParticles_nTTXStations[mcPart]);
	histmap[type]["TTXHits"]->Fill(MCParticles_nTTXHits[mcPart]);
	histmap[type]["TTUStations"]->Fill(MCParticles_nTTUStations[mcPart]);
	histmap[type]["TTUHits"]->Fill(MCParticles_nTTUHits[mcPart]);
	histmap[type]["TTVStations"]->Fill(MCParticles_nTTVStations[mcPart]);
	histmap[type]["TTVHits"]->Fill(MCParticles_nTTVHits[mcPart]);
	histmap[type]["TTStereoStations"]->Fill(MCParticles_nTTStStations[mcPart]);
	histmap[type]["TTStereoHits"]->Fill(MCParticles_nTTVHits[mcPart] + MCParticles_nTTUHits[mcPart]);
	
	histmap[type]["ITStations"]->Fill(MCParticles_nITStations[mcPart]);
	histmap[type]["ITHits"]->Fill(MCParticles_nITHits[mcPart]);
	histmap[type]["ITXStations"]->Fill(MCParticles_nITXStations[mcPart]);
	histmap[type]["ITXHits"]->Fill(MCParticles_nITXHits[mcPart]);
	histmap[type]["ITUStations"]->Fill(MCParticles_nITUStations[mcPart]);
	histmap[type]["ITUHits"]->Fill(MCParticles_nITUHits[mcPart]);
	histmap[type]["ITVStations"]->Fill(MCParticles_nITVStations[mcPart]);
	histmap[type]["ITVHits"]->Fill(MCParticles_nITVHits[mcPart]);
	histmap[type]["ITStereoStations"]->Fill(MCParticles_nITStStations[mcPart]);
	histmap[type]["ITStereoHits"]->Fill(MCParticles_nITVHits[mcPart] + MCParticles_nITUHits[mcPart]);
	
	histmap[type]["OTStations"]->Fill(MCParticles_nOTStations[mcPart]);
	histmap[type]["OTHits"]->Fill(MCParticles_nOTHits[mcPart]);
	histmap[type]["OTXStations"]->Fill(MCParticles_nOTXStations[mcPart]);
	histmap[type]["OTXHits"]->Fill(MCParticles_nOTXHits[mcPart]);
	histmap[type]["OTUStations"]->Fill(MCParticles_nOTUStations[mcPart]);
	histmap[type]["OTUHits"]->Fill(MCParticles_nOTUHits[mcPart]);
	histmap[type]["OTVStations"]->Fill(MCParticles_nOTVStations[mcPart]);
	histmap[type]["OTVHits"]->Fill(MCParticles_nOTVHits[mcPart]);
	histmap[type]["OTStereoStations"]->Fill(MCParticles_nOTStStations[mcPart]);
	histmap[type]["OTStereoHits"]->Fill(MCParticles_nOTVHits[mcPart] + MCParticles_nOTUHits[mcPart]);
	
	histmap[type]["TStations"]->Fill(MCParticles_nTStations[mcPart]);
	histmap[type]["THits"]->Fill(MCParticles_nOTHits[mcPart] + MCParticles_nITHits[mcPart]);
	histmap[type]["TXStations"]->Fill(MCParticles_nTXStations[mcPart]);
	histmap[type]["TXHits"]->Fill(MCParticles_nOTXHits[mcPart] + MCParticles_nITXHits[mcPart]);
	histmap[type]["TUStations"]->Fill(MCParticles_nTUStations[mcPart]);
	histmap[type]["TUHits"]->Fill(MCParticles_nOTUHits[mcPart] + MCParticles_nITUHits[mcPart]);
	histmap[type]["TVStations"]->Fill(MCParticles_nTVStations[mcPart]);
	histmap[type]["TVHits"]->Fill(MCParticles_nOTVHits[mcPart] + MCParticles_nITVHits[mcPart]);
	histmap[type]["TStereoStations"]->Fill(MCParticles_nTStStations[mcPart]);
	histmap[type]["TStereoHits"]->Fill(MCParticles_nOTVHits[mcPart] + MCParticles_nOTUHits[mcPart] + MCParticles_nITVHits[mcPart] + MCParticles_nITUHits[mcPart]);
      }
   }

   f.Write();
}
