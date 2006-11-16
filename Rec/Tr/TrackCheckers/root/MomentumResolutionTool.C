// MomentumResolutionTool.C
//
// this root macro should produce histograms of the resolution obtained
// for different track parameters, separately for each track type and
// position in the detector (the available positions depend on what
// you write to your NTuple)
//
// author: Manuel Schiller
// date: 2006-11-16

#define MomentumResolutionTool_cxx
#include "MomentumResolutionTool.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

#include <sstream>
#include <string>
#include <map>
#include <set>

static bool isB(int PID)
{
	PID = abs(PID);
	if (PID < 500) return false;
	if (PID >= 600) return false;
	return true;
}

void MomentumResolutionTool::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L MomentumResolutionTool.C
//      Root > MomentumResolutionTool t
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

   TFile f("MomentumResolutionTool.root", "Recreate");

   // ok, pack all Track types into a map
   std::map<int, std::string> trackTypes;
   trackTypes.insert(make_pair(  0, "Unknown"));
   trackTypes.insert(make_pair(  1, "Velo"));
   trackTypes.insert(make_pair(  2, "VeloR"));
   trackTypes.insert(make_pair(  3, "Long"));
   trackTypes.insert(make_pair(  4, "Upstream"));
   trackTypes.insert(make_pair(  5, "Downstream"));
   trackTypes.insert(make_pair(  6, "Ttrack"));
   trackTypes.insert(make_pair(100, "All"));
   
   // ok, we need to map FixedZ positions to names
   std::map<int, std::string> mapFixedZ;
   mapFixedZ[0] = "Vertex";
   mapFixedZ[1] = "BegRich1";
   mapFixedZ[2] = "EndRich1";
   mapFixedZ[3] = "BegRich2";
   mapFixedZ[4] = "EndRich2";

   // locations in the detector to monitor - note that you still have
   // to write the code that monitors the location (see below), this is
   // just for booking the histograms conveniently
   std::set<std::string> locations;
   //locations.push_back("FirstState");
   locations.insert("Vertex");
   locations.insert("BegRich1");
   locations.insert("EndRich1");
   locations.insert("BegRich2");
   locations.insert("EndRich2");

   const double pbins[] = {
	   0.5, 0.5, 1., 1.5, 2., 2.5, 3., 3.5, 4., 5., 6., 7.5,
	   10., 12.5 , 15., 20., 25., 30., 35., 40., 45.,
	   50., 60., 75., 75.
   };
   const int npbins = sizeof(pbins) / sizeof(double) - 2;

   // ok, make a map mapping histogram names to histograms
   // e.g. for Long tracks, there can be histograms called dp, x, y,
   // xpull, ypull, etc.
   std::map<std::string, std::map<std::string, std::map<std::string, TH1D*> > > map1d;
   std::map<std::string, std::map<std::string, std::map<std::string, TH2D*> > > map2d;
   for (std::map<int, std::string>::const_iterator tt = trackTypes.begin();
	tt != trackTypes.end(); ++tt) {
     for (std::set<std::string>::const_iterator lo = locations.begin();
	  lo != locations.end(); ++lo) {
       // we also need a loop to do things once for all and once for B tracks
       for (int B = 0; B < 2; ++B) {
	 // make sure we don't have to "deference" the iterator every time
	 std::string t(tt->second);
	 std::string l(*lo);
	 // build name pre- and title postfixes
	 std::string npfx(t); npfx.append(*lo);
	 if (B) npfx.insert(0, "B");
	 std::string tpfx(" for ");
	 tpfx.append(t).append((B)?(" B "):("")).append(" tracks at ").append(*lo);
	 // dprel histos are much broader for All, Ttrack, Upstream, Velo
	 // and VeloR tracks because we can't measure track curvature
	 // accurately, so we have to account for that
	 double dprelrange = 0.05;
	 if (("All" == t) || ("Ttrack" == t) || ("Velo" == t) ||
			 ("VeloR" == t) || ("Upstream" == t))
		 dprelrange = 1.;
	 // also, extrapolating Ttracks to the Vertex (or Velo Tracks to
	 // RICH2) is likely to have a larger uncertainty and should therefore
	 // have other limits for dx, dy and dtx (dty is not affected so much
	 // because the magnetic field does less in that direction)
	 double dxyrange = 1.;
	 double dtxrange = 0.005;
	 if (((("All" == t) || ("Velo" == t) || ("VeloR" == t) ||
	      ("Upstream" == t)) && (("BegRich2" == l) || ("EndRich2" == l)))
	     || ((("Ttrack" == t) || ("Downstream" == t))
		 && ("Vertex" == l)))
		 dxyrange = 5., dtxrange = 0.05;
	 // patch the track type for the B histos once everything is set up
	 // we wait up to this point because we'd have to compare a lot more
	 // if our track type was already "enhanced" with a "B" in front
	 if (B) t.insert(0, "B");
	        
	 // ok, allocate the histograms
	 map2d[t][l].insert(make_pair("dpreloverp",
		new TH2D(std::string(npfx).append("dpreloverp").c_str(),
			 std::string("dp/p over p").append(tpfx).c_str(),
			 npbins, pbins, 100, -dprelrange, dprelrange)));
	 map2d[t][l].insert(make_pair("dppulloverp",
		new TH2D(std::string(npfx).append("dppulloverp").c_str(),
			 std::string("dp pull over p").append(tpfx).c_str(),
			 npbins, pbins, 100, -3., 3.)));
       
	 map1d[t][l].insert(make_pair("dprel",
		new TH1D(std::string(npfx).append("dprel").c_str(),
			 std::string("dp/p").append(tpfx).c_str(),
			 100, -dprelrange, dprelrange)));
	 map1d[t][l].insert(make_pair("dppull",
		new TH1D(std::string(npfx).append("dppull").c_str(),
			 std::string("dp pull").append(tpfx).c_str(),
			 100, -3., 3.)));
       
	 map1d[t][l].insert(make_pair("dx",
		new TH1D(std::string(npfx).append("dx").c_str(),
			 std::string("dx").append(tpfx).c_str(),
			 100, -dxyrange, dxyrange)));
	 map1d[t][l].insert(make_pair("dxpull",
		new TH1D(std::string(npfx).append("dxpull").c_str(),
			 std::string("dx pull").append(tpfx).c_str(),
			 100, -3., 3.)));
       
	 map1d[t][l].insert(make_pair("dy",
		new TH1D(std::string(npfx).append("dy").c_str(),
			 std::string("dy").append(tpfx).c_str(),
			 100, -dxyrange, dxyrange)));
	 map1d[t][l].insert(make_pair("dypull",
		new TH1D(std::string(npfx).append("dypull").c_str(),
			 std::string("dy pull").append(tpfx).c_str(),
			 100, -3., 3.)));
       
	 map1d[t][l].insert(make_pair("dtx",
		new TH1D(std::string(npfx).append("dtx").c_str(),
			 std::string("dtx").append(tpfx).c_str(),
			 100, -dtxrange, dtxrange)));
	 map1d[t][l].insert(make_pair("dtxpull",
		new TH1D(std::string(npfx).append("dtxpull").c_str(),
			 std::string("dtx pull").append(tpfx).c_str(),
			 100, -3., 3.)));
       
	 map1d[t][l].insert(make_pair("dty",
		new TH1D(std::string(npfx).append("dty").c_str(),
			 std::string("dty").append(tpfx).c_str(),
			 100, -.005, .005)));
	 map1d[t][l].insert(make_pair("dtypull",
		new TH1D(std::string(npfx).append("dtypull").c_str(),
			 std::string("dty pull").append(tpfx).c_str(),
			 100, -3., 3.)));
       }
     }
   }

   Long64_t nbytes = 0, nb = 0;
   // loop over events
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;

      // ok, we need to loop over all tracks...
      for (int nTrack = 0; nTrack < nSavedTracks; ++nTrack) {
	// we can't use Tracks without MC information
	if (Tracks_assocMCParts[nTrack][0] < 0.) continue;
	if (Tracks_assocMCPartWeights[nTrack][0] < 0.7) continue;

	// check if our track comes from a B daughter particle
	bool isBdaughter = false;
	for (int mcPart = (int) Tracks_assocMCParts[nTracks][0]; mcPart >= 0;
	     mcPart = (int) MCParticles_mother[mcPart]) {
	   if (isB((int) MCParticles_PID[mcPart])) {
	      isBdaughter = true;
	      break;
	   }
	}
	
	// ok, association to MC Truth is valid
	// this means that we can rely on the availability of MC truth
	// information in the NTuple without further checks, we just
	// need to check that our Measurements are in fact there
	
	// we do a loop here, filling B histos as well (if applicable)
	// by fiddeling with isBdaughtersTurn, see below
	bool isBdaughtersTurn = isBdaughter;
	do {
	  // get track type now
	  std::string type = trackTypes[(int) Tracks_type[nTrack]];
	  // the "All tracks" wildcard type
	  std::string atyp("All");
	  // and patch for B daughters, if applicable
	  if (isBdaughtersTurn) {
	    type.insert(0, "B");
	    atyp.insert(0, "B");
	  }
	  
	  // do the FixedZ positions
	  for (int i = 0; i < 10; ++i) {
	    if (Tracks_FixedZCxx[nTrack][i] < 0.)
		    continue;
	    if (Tracks_FixedZTCxx[nTrack][i] < 0.)
		    continue;
	    // use only known locations
	    if (mapFixedZ.end() == mapFixedZ.find(i))
		    continue;
	    std::string loc(mapFixedZ[i]);
	    // ok, do our thing at the Vertex
	    // we start by reading the variables
	    double dx = Tracks_FixedZx[nTrack][i] - Tracks_FixedZTx[nTrack][i];
	    double dy = Tracks_FixedZy[nTrack][i] - Tracks_FixedZTy[nTrack][i];
	    double dtx = Tracks_FixedZtx[nTrack][i] - Tracks_FixedZTtx[nTrack][i];
	    double dty = Tracks_FixedZty[nTrack][i] - Tracks_FixedZTty[nTrack][i];
	    double p = 1. / fabs(Tracks_FixedZqp[nTrack][i]);
	    double Tp = 1. / fabs(Tracks_FixedZTqp[nTrack][i]);
	    double dpp = 1. - Tp/p;	// this is dp/p !
	    double cxx = Tracks_FixedZCxx[nTrack][i];
	    double cyy = Tracks_FixedZCyy[nTrack][i];
	    double ctxtx = Tracks_FixedZCtxtx[nTrack][i];
	    double ctyty = Tracks_FixedZCtyty[nTrack][i];
	    double cpp = p * p * p * p * Tracks_FixedZCqpqp[nTrack][i];

	    // ok, fill histos now
	    map2d[type][loc]["dpreloverp"]->Fill(fabs(p), dpp);
	    map2d[atyp][loc]["dpreloverp"]->Fill(fabs(p), dpp);
	    map2d[type][loc]["dppulloverp"]->Fill(fabs(p), p * dpp / sqrt(cpp));
	    map2d[atyp][loc]["dppulloverp"]->Fill(fabs(p), p * dpp / sqrt(cpp));

	    map1d[type][loc]["dprel"]->Fill(dpp);
	    map1d[atyp][loc]["dprel"]->Fill(dpp);
	    map1d[type][loc]["dppull"]->Fill(p * dpp / sqrt(cpp));
	    map1d[atyp][loc]["dppull"]->Fill(p * dpp / sqrt(cpp));

	    map1d[type][loc]["dx"]->Fill(dx);
	    map1d[atyp][loc]["dx"]->Fill(dx);
	    map1d[type][loc]["dxpull"]->Fill(dx / sqrt(cxx));
	    map1d[atyp][loc]["dxpull"]->Fill(dx / sqrt(cxx));

	    map1d[type][loc]["dy"]->Fill(dy);
	    map1d[atyp][loc]["dy"]->Fill(dy);
	    map1d[type][loc]["dypull"]->Fill(dy / sqrt(cyy));
	    map1d[atyp][loc]["dypull"]->Fill(dy / sqrt(cyy));

	    map1d[type][loc]["dtx"]->Fill(dtx);
	    map1d[atyp][loc]["dtx"]->Fill(dtx);
	    map1d[type][loc]["dtxpull"]->Fill(dtx / sqrt(ctxtx));
	    map1d[atyp][loc]["dtxpull"]->Fill(dtx / sqrt(ctxtx));

	    map1d[type][loc]["dty"]->Fill(dty);
	    map1d[atyp][loc]["dty"]->Fill(dty);
	    map1d[type][loc]["dtypull"]->Fill(dty / sqrt(ctyty));
	    map1d[atyp][loc]["dtypull"]->Fill(dty / sqrt(ctyty));
	  }
	  // ok, make sure we go a second time through the loop body if
	  // we wrote the B histos just now, this time writing to the
	  // general ones
	  if (isBdaughtersTurn) {
	    isBdaughtersTurn = false;
	    continue;
	  }
	  // ok, we break out of the loop once the B daughter histos have been written
	} while (isBdaughtersTurn);
      } // end track loop
   } // end loop events

   f.Write();
}
