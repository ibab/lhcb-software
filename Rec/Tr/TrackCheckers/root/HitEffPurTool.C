// HitEffPurTool.C
//
// root macro to read in a TrackCheckerNT NTuple and produce histograms
// for hit efficiency, hit purity and ghost rate as a function of various
// quantities
//
// author: Manuel Schiller
// date: 2006-11-16

#define HitEffPurTool_cxx
#include "HitEffPurTool.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TProfile.h>
#include <TLegend.h>

#include <set>
#include <map>
#include <string>
#include <iostream>
#include <sstream>

void HitEffPurTool::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L HitEffPurTool.C
//      Root > HitEffPurTool t
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

   TFile f("HitEffPurTool.root", "Recreate");

   // define a mapping between track type numbers and their name - please
   // make sure that this matches what Brunel it using...
   std::map<int, std::string> trackTypes;
   trackTypes.insert(make_pair(  0, "Unknown"));
   trackTypes.insert(make_pair(  1, "Velo"));
   trackTypes.insert(make_pair(  2, "VeloR"));
   trackTypes.insert(make_pair(  3, "Long"));
   trackTypes.insert(make_pair(  4, "Upstream"));
   trackTypes.insert(make_pair(  5, "Downstream"));
   trackTypes.insert(make_pair(  6, "Ttrack"));
   trackTypes.insert(make_pair(100, "All"));

   // we can plot hit efficiencies, purities and ghost rates for these
   // subdetectors
   std::set<std::string> detectors;
   detectors.insert("Velo");
   detectors.insert("TT");
   detectors.insert("IT");
   detectors.insert("OT");
   detectors.insert("T");
   detectors.insert("All");
   
   // ok, set of variables against which we plot stuff
   std::set<std::string> measTypes;
   measTypes.insert("p");
   measTypes.insert("pt");
   measTypes.insert("VeloOccupancy");
   measTypes.insert("TTOccupancy");
   measTypes.insert("ITOccupancy");
   measTypes.insert("OTOccupancy");
   measTypes.insert("TOccupancy");
   measTypes.insert("Occupancy");

   // ok, define maps to map the x axis quantity to histogram binning info
   // we use variable size bins so we can get useful plots by binning in
   // such a way that the number of entries per bin does not get too low
   // this makes it possible to get more or less useful plots even for lower
   // statistics
   std::map<std::string, const double*> hbins;
   std::map<std::string, int> nbins;
   // histogram bins for momentum from .5 GeV up to 75 GeV
   const double pbins[] = {
	0.0, 0.5, 1., 1.5, 2., 2.5, 3., 3.5, 4., 5., 6., 7.5,
	10., 12.5 , 15., 20., 25., 30., 35., 40., 45.,
	50., 60., 75., 1e42
   };
   hbins["p"] = pbins; nbins["p"] = sizeof(pbins) / sizeof(double) - 2;
   // similar for pt
   const double ptbins[] = {
	0.0, 0.0, 0.25, 0.5, 0.75, 1., 1.25, 1.5, 1.75,
	     2., 2.5, 3., 3.5, 4., 4.5,
	     5., 6., 7.5, 10., 12.5 , 15., 20., 25., 1e42
   };
   hbins["pt"] = ptbins; nbins["pt"] = sizeof(ptbins) / sizeof(double) - 2;
   // or alternatively, from 0.5 to 15 percent OT occupancy
   const double oOTbins[] = {
	.000, .000, .005, .010, .015, .020, .025, .030, .035, .040, .045,
	      .050, .055, .060, .065, .070, .075, .080, .085, .090, .095,
	      .100, .105, .110, .115, .120, .125, .130, .135, .140, .145,
	.150, 1.
   };
   hbins["OTOccupancy"] = oOTbins;
   nbins["OTOccupancy"] = sizeof(oOTbins) / sizeof(double) - 2;
   // the other detectors are generally much lower in occupancy, so we can
   // use a common binning here
   const double obins[] = {
	.000, .000, .001, .002, .003, .004, .005, .006, .007, .008, .009,
	      .010, .011, .012, .013, .014, .015, .016, .017, .018, .019,
	      .020, .021, .022, .023, .024, .025, .026, .027, .028, .029,
	      .030, .040, .050, .060, 1.
   };
   hbins["ITOccupancy"] = obins; nbins["ITOccupancy"] = sizeof(obins) / sizeof(double) - 2;
   hbins["TTOccupancy"] = obins; nbins["TTOccupancy"] = sizeof(obins) / sizeof(double) - 2;
   hbins["TOccupancy"] = obins; nbins["TOccupancy"] = sizeof(obins) / sizeof(double) - 2;
   hbins["VeloOccupancy"] = obins; nbins["VeloOccupancy"] = sizeof(obins) / sizeof(double) - 2;
   hbins["Occupancy"] = obins; nbins["Occupancy"] = sizeof(obins) / sizeof(double) - 2;

   // make binwidth weighting histograms
   // (one may wish to look at the relative track density per unit X axis
   // interval, so one needs to account for the different widths of the
   // histogram bins)
   std::map<std::string, TH1D*> binw;
   for (std::map<std::string, int>::const_iterator it = nbins.begin();
		   nbins.end() != it; ++it) {
     const std::string& mt = it->first;
     double minwidth = 1e42;
     std::string str = "binw" + mt;
     binw[mt] = new TH1D(str.c_str(), str.c_str(), nbins[mt], hbins[mt]);
     for (int i = 1; i < nbins[mt]; ++i) {
       double w = hbins[mt][i + 1] - hbins[mt][i];
       binw[mt]->Fill(.5 * (hbins[mt][i + 1] + hbins[mt][i]), w);
       if ((0. != w) && (minwidth > w)) minwidth = w;
     }
     // scale the histogram in such a way that the bin with the smallest
     // width will remain unaltered later (when scaling with the bin widths)
     binw[mt]->Scale(1. / minwidth);
   }

   // book histograms to be used:
   // hmap[12]d		track averaged quantities
   // hmape[12]d	event averaged quantities
   // hmapt[12]d	temporary histograms used to calculate the
   // 			quantities above
   // we're doing this in a loop to make sure we get consistent names
   // and setting for the number of histograms we produce
   std::map<std::string, std::map<std:: string, std::map<std::string, TH2D*> > > hmap2d;
   std::map<std::string, std::map<std:: string, std::map<std::string, TH2D*> > > hmape2d;
   std::map<std::string, std::map<std:: string, std::map<std::string, TH2D*> > > hmapt2d;
   std::map<std::string, std::map<std:: string, std::map<std::string, TH1D*> > > hmap1d;
   std::map<std::string, std::map<std:: string, std::map<std::string, TH1D*> > > hmape1d;
   std::map<std::string, std::map<std:: string, std::map<std::string, TH1D*> > > hmapt1d;
   // loop over the different histogram maps to fill them
   for (int kt = 0 ; kt < 3; ++kt) {
      // ok, make sure we operate on the right map when we loop over
      // track-averaged, event-averaged and temporary counter histos...
      std::string navtype = (0 == kt)?"TrAvg":((1 == kt)?"EvAvg":"TMP");
      std::string tavtype = (0 == kt)?" (tr-avg)":((1 == kt)?" (ev-avg)":" (TMP)");
      std::map<std::string, std::map<std::string, std::map<std::string, TH2D*> > >&
	      hm2d = (0 == kt)?hmap2d:((1 == kt)?hmape2d:hmapt2d);
      std::map<std::string, std::map<std::string, std::map<std::string, TH1D*> > >&
	      hm1d = (0 == kt)?hmap1d:((1 == kt)?hmape1d:hmapt1d);
      // loop over track types
      for (std::map<int, std::string>::const_iterator it = trackTypes.begin();
		      it != trackTypes.end(); ++it) {
	 // loop over variables against which to plot
	 for (std::set<std::string>::const_iterator lt = measTypes.begin();
			 measTypes.end() != lt; ++lt) {
	    const std::string& m = *lt;
	    // loop over detector components
	    for (std::set<std::string>::const_iterator jt = detectors.begin();
			    jt != detectors.end(); ++jt) {
	       std::string he(it->second), hp(it->second),
		       hen("Hit efficiency for "), hpn("Hit purity for ");
	       he += "HitEffIn" + *jt;
	       hen += it->second +  " tracks in detector " + *jt;
	       hp += "HitPurIn" + *jt;
	       hpn += it->second + " tracks in detector " + *jt;
	       
	       hm2d[it->second][m][he.substr(it->second.length())] =
		       new TH2D((m + he + navtype).c_str(), (hen + tavtype).c_str(),
				       nbins[m], hbins[m], 33, 0., 1.);
	       hm2d[it->second][m][hp.substr(it->second.length())] =
		       new TH2D((m + hp + navtype).c_str(), (hpn + tavtype).c_str(),
				       nbins[m], hbins[m], 33, 0., 1.);
	       hm2d[it->second][m][he.substr(it->second.length())] ->
		       GetXaxis()->SetTitle((m + ((("p" == m) || ("pt" == m))?" [GeV/c]":"")).c_str());
	       hm2d[it->second][m][hp.substr(it->second.length())] ->
		       GetXaxis()->SetTitle((m + ((("p" == m) || ("pt" == m))?" [GeV/c]":"")).c_str());
	    } // end loop over detector components
	    // ghost rates don't make sense per detector component
	    // (at least not with what we do below to identify ghosts)
	    std::string gr(it->second), n(it->second),
		    grn("Ghost rate for "), nn("# of tracks for ");
	    gr += "Ghost"; grn += it->second + " tracks";
	    n += "N"; nn += it->second + " tracks";
	    hm1d[it->second][m]["Ghost"] = new TH1D((m + gr + navtype).c_str(),
			    (grn + tavtype).c_str(), nbins[m], hbins[m]);
	    hm1d[it->second][m]["N"] = new TH1D((m + n + navtype).c_str(),
			    (nn + tavtype).c_str(), nbins[m], hbins[m]);
	    hm1d[it->second][m]["N"]->Sumw2();
	    hm1d[it->second][m]["Ghost"]->Sumw2();
	    hm1d[it->second][m]["N"]->GetXaxis()->SetTitle((m + ((("p" == m) || ("pt" == m))?" [GeV/c]":"")).c_str());
	    hm1d[it->second][m]["Ghost"]->GetXaxis()->SetTitle((m + ((("p" == m) || ("pt" == m))?" [GeV/c]":"")).c_str());
	 } // end loop over variables against which to plot
      } // end loop over track types
   } // end loop over average type (track-avg, event-avg, ...)

   // ******************************************************************
   // ok, below, comes the event processing loop
   // ******************************************************************
   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      
      // ok, figure out occupancy of detector components
      // start by counting the total number of hits per subdetector
      // (we need a loop over MCParticles for that...)
      double tot_Velo, tot_TT, tot_IT, tot_OT;
      tot_Velo = nVeloHits; tot_TT = nTTHits;
      tot_IT = nITHits; tot_OT = nOTHits;
      // calculate occupancies from that
      // the magiacal constants in the four lines below come from asking
      // each detector in Brunel v30r9 for the number of channels it has
      double occVelo = tot_Velo / 180224.;
      double occTT = tot_TT / 143360.;
      double occIT = tot_IT / 129024.;
      double occOT = tot_OT / 53760.;
      // these should probably per-detector-component averages and not
      // just channel averages, irrespective of detector component
      // at least to me, this seems the more sensible thing to do
      double occT = .5 * (occIT + occOT);
      double occ = .25 * (occVelo + occTT + occIT + occOT);

      // ok, loop over all tracks
      for (int nTrack = 0; nTrack < nSavedTracks; ++nTrack) {
	  // work out p and pt
	  double p = 1. / fabs(Tracks_firstState[nTrack][5]);
	  double pt = p * sqrt(1. - 1. /
			  (1. + pow(double(Tracks_firstState[nTrack][3]), 2.) +
			   pow(double(Tracks_firstState[nTrack][4]), 2.)));
	  
	  // work out if we have a ghost or are missing MC truth due to NTuple
	  // limitations (the last one should not happen)
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
	  
	  const std::string& type = trackTypes[(int) Tracks_type[nTrack]];

	  // define C preprocessor macros which fill things into our histos
#define _FILL1D(map, mt, which, cnt) { \
	  map[type][mt][which]->Fill(cnt); \
	  map["All"][mt][which]->Fill(cnt); }
#define _FILL2D(map, mt, which, cnt, cnt2) { \
	  map[type][mt][which]->Fill(cnt, cnt2); \
	  map["All"][mt][which]->Fill(cnt, cnt2); }
	  // zero occupancy should probably mean that we don't record the
	  // event because no hits in the detector will also mean no
	  // hit purity, no hit efficiency and no ghost rate that can be
	  // defined in a sensible way
#define _FILL1DO(map, mt, which, cnt) \
	  if (cnt > 0.) _FILL1D(map, mt, which, cnt)
#define _FILL2DO(map, mt, which, cnt, cnt2) \
	  if (cnt > 0.) _FILL2D(map, mt, which, cnt, cnt2)
#define FILL1D(map, type, which) { \
	  _FILL1D(map, "p", which, p); \
	  _FILL1D(map, "pt", which, pt); \
	  _FILL1D(map, "VeloOccupancy", which, occVelo); \
	  _FILL1D(map, "TTOccupancy", which, occTT); \
	  _FILL1D(map, "ITOccupancy", which, occIT); \
	  _FILL1D(map, "OTOccupancy", which, occOT); \
	  _FILL1D(map, "TOccupancy", which, occT); \
	  _FILL1D(map, "Occupancy", which, occ); }
#define FILL2D(map, type, which, cnt2) { \
	  _FILL2D(map, "p", which, p, cnt2); \
	  _FILL2D(map, "pt", which, pt, cnt2); \
	  _FILL2D(map, "VeloOccupancy", which, occVelo, cnt2); \
	  _FILL2D(map, "TTOccupancy", which, occTT, cnt2); \
	  _FILL2D(map, "ITOccupancy", which, occIT, cnt2); \
	  _FILL2D(map, "OTOccupancy", which, occOT, cnt2); \
	  _FILL2D(map, "TOccupancy", which, occT, cnt2); \
	  _FILL2D(map, "Occupancy", which, occ, cnt2); }
	  
	  // ok, fill the temporary versions first
	  FILL1D(hmapt1d, type, "N");
	
	  // skip tracks without MC truth
	  if (isGhost) {
	    FILL1D(hmapt1d, type, "Ghost");
	    continue;
	  }
	   
	  // while filling the histos, we multiply the denominators by 1+epsilon
	  // (a small epsilon > 0) to make sure that we stay below 1.0 when
	  // filling the histograms (1.0 would end up in the overflow bin
	  // otherwise) - the error we introduce is negligible
	  const double fudge = 1. + 1e-5;
	   
	  // statistics for Velo
	  if ((Tracks_nMCTotalVelo[nTrack] *= fudge) > 0.)
		  FILL2D(hmapt2d, type, "HitEffInVelo", Tracks_nGoodVelo[nTrack] / Tracks_nMCTotalVelo[nTrack]);
	  if ((Tracks_nTotalVelo[nTrack] *= fudge) > 0.)
		  FILL2D(hmapt2d, type, "HitPurInVelo", Tracks_nGoodVelo[nTrack] / Tracks_nTotalVelo[nTrack]);
	  	  
	  // same for TT
	  if ((Tracks_nMCTotalTT[nTrack] *= fudge) > 0.)
		  FILL2D(hmapt2d, type, "HitEffInTT", Tracks_nGoodTT[nTrack] / Tracks_nMCTotalTT[nTrack]);
	  if ((Tracks_nTotalTT[nTrack] *= fudge) > 0.)
		  FILL2D(hmapt2d, type, "HitPurInTT", Tracks_nGoodTT[nTrack] / Tracks_nTotalTT[nTrack]);
	  
	  // same for IT
	  if ((Tracks_nMCTotalIT[nTrack] *= fudge) > 0.)
		  FILL2D(hmapt2d, type, "HitEffInIT", Tracks_nGoodIT[nTrack] / Tracks_nMCTotalIT[nTrack]);
	  if ((Tracks_nTotalIT[nTrack] *= fudge) > 0.)
		  FILL2D(hmapt2d, type, "HitPurInIT", Tracks_nGoodIT[nTrack] / Tracks_nTotalIT[nTrack]);

	  // same for OT
	  if ((Tracks_nMCTotalOT[nTrack] *= fudge) > 0.)
		  FILL2D(hmapt2d, type, "HitEffInOT", Tracks_nGoodOT[nTrack] / Tracks_nMCTotalOT[nTrack]);
	  if ((Tracks_nTotalOT[nTrack] *= fudge) > 0.)
		  FILL2D(hmapt2d, type, "HitPurInOT", Tracks_nGoodOT[nTrack] / Tracks_nTotalOT[nTrack]);
	   
	  // now T stations combined - add up first
	  float nMCTotalT = ((Tracks_nMCTotalOT[nTrack] > 0.)?Tracks_nMCTotalOT[nTrack]:0.) +
	   ((Tracks_nMCTotalIT[nTrack] > 0.)?Tracks_nMCTotalIT[nTrack]:0.);
	  float nTotalT = ((Tracks_nTotalOT[nTrack] > 0.)?Tracks_nTotalOT[nTrack]:0.) +
	   ((Tracks_nTotalIT[nTrack] > 0.)?Tracks_nTotalIT[nTrack]:0.);
	  float nGoodT = ((Tracks_nGoodOT[nTrack] > 0.)?Tracks_nGoodOT[nTrack]:0.) +
	   ((Tracks_nGoodIT[nTrack] > 0.)?Tracks_nGoodIT[nTrack]:0.);
	  if (nMCTotalT > 0.)
		  FILL2D(hmapt2d, type, "HitEffInT", nGoodT / nMCTotalT);
	  if (nTotalT > 0.)
		  FILL2D(hmapt2d, type, "HitPurInT", nGoodT / nTotalT);
	   
	  // overall statistics
	  if ((Tracks_nMCTotalHits[nTrack] *= fudge) > 0.)
		  FILL2D(hmapt2d, type, "HitEffInAll", Tracks_nGoodHits[nTrack] / Tracks_nMCTotalHits[nTrack]);
	  if ((Tracks_nTotalHits[nTrack] *= fudge) > 0.)
		  FILL2D(hmapt2d, type, "HitPurInAll", Tracks_nGoodHits[nTrack] / Tracks_nTotalHits[nTrack]);
	  // we don't need the C preprocessor macros any longer
#undef _FILL1D
#undef _FILL2D
#undef _FILL1DO
#undef _FILL2DO
#undef FILL1D
#undef FILL2D
      } // end loop over tracks

      // add the temp histos to the event-/track-averaged ones
      for (std::set<std::string>::const_iterator mt = measTypes.begin();
		      measTypes.end() != mt; ++mt) {
	 const std::string& m = *mt;
	 double N = hmapt1d["All"][m]["N"]->Integral();
	 // make sure we don't divide by zero
	 if (N <= 0.) N = 1.;
	 for (std::map<int, std::string>::const_iterator it = trackTypes.begin();
			 it != trackTypes.end(); ++it) {
	    std::string type(it->second);
	    // handle the 1d ones
	    for (std::map<std::string, TH1D*>::const_iterator jt = hmapt1d[type][m].begin();
			    jt != hmapt1d[type][m].end(); ++jt) {
	       std::string j(jt->first);
	       hmap1d[type][m][j]->Add(hmapt1d[type][m][j]);
	       hmapt1d[type][m][j]->Scale(1. / N);
	       hmape1d[type][m][j]->Add(hmapt1d[type][m][j]);
	       hmapt1d[type][m][j]->Reset();
	    }
	    // handle the 2d ones
	    for (std::map<std::string, TH2D*>::const_iterator jt = hmapt2d[type][m].begin();
			    jt != hmapt2d[type][m].end(); ++jt) {
	       std::string j(jt->first);
	       hmap2d[type][m][j]->Add(hmapt2d[type][m][j]);
	       hmapt2d[type][m][j]->Scale(1. / N);
	       hmape2d[type][m][j]->Add(hmapt2d[type][m][j]);
	       hmapt2d[type][m][j]->Reset();
	    }
	 }
      }
   } // end loop over events

   // ok, we don't need the temp histos any longer, so delete them
   for (std::map<int, std::string>::const_iterator
	it = trackTypes.begin();
	it != trackTypes.end(); ++it) {
     const std::string& type = it->second;
     for (std::set<std::string>::const_iterator mt = measTypes.begin();
		     measTypes.end() != mt; ++mt) {
       const std::string& m = *mt;
       // handle the 1d ones
       for (std::map<std::string, TH1D*>::const_iterator
		       jt = hmapt1d[type][m].begin();
		       jt != hmapt1d[type][m].end(); ++jt) {
	       delete jt->second;
       }
       // handle the 2d ones
       for (std::map<std::string, TH2D*>::const_iterator
		       jt = hmapt2d[type][m].begin();
		       jt != hmapt2d[type][m].end(); ++jt) {
	       delete jt->second;
       }
     }
   }

   // now comes a stupid little loop to do the drawing for both event-
   // and track-averaged quantities
   // we do this for all variables against which we plot
   //
   // note: not all of the plots we save to the root file make sense in
   // all situations - make sure you understand the plot you're using
   // also, you may need to have different plots available to suit your
   // needs - feel free to adapt the code as needed
   for (std::set<std::string>::const_iterator mt = measTypes.begin();
		   measTypes.end() != mt; ++mt) {
      const std::string& m = *mt;
      for (int frutz = 0; frutz < 2; ++frutz) {
	 // ok, we need a reference to the histogram map we'll be handling now
	 std::map<std::string, std::map<std::string, std::map<std::string, TH1D*> > >
		 &hm1d = (frutz)?(hmape1d):(hmap1d);
	 std::map<std::string, std::map<std::string, std::map<std::string, TH2D*> > >
		 &hm2d = (frutz)?(hmape2d):(hmap2d);
	 // and we also need name and title suffixes to distinguish the two
	 std::string titsfx((frutz)?"event averaged quantities":"track averaged quantities");
	 std::string namsfx(((frutz)?"EvAvg":"TrAvg")+m);
	 
	 // ok, make the drawings
	 for (std::map<int, std::string>::const_iterator
			 it = trackTypes.begin(); it != trackTypes.end(); ++it) {
	    // how many tracks are there
	    double N = hm1d[it->second][m]["N"]->Integral();
	    hm1d[it->second][m]["N"]->Scale(1. + 1e-6);
	    // ghost rate = # ghost tracks / # tracks
	    hm1d[it->second][m]["Ghost"]->Divide(hm1d[it->second][m]["N"]);
	    hm1d[it->second][m]["Ghost"]->SetLineColor(kRed);
	    // ok, do the binwidth weighting
	    hm1d[it->second][m]["N"]->Divide(binw[m]);
	    // normalize "N" histo
	    hm1d[it->second][m]["N"]->Scale(1. / hm1d[it->second][m]["N"]->Integral());
	    
	    TCanvas *c = new TCanvas((it->second + namsfx).c_str(),
			    (it->second + " - " + titsfx).c_str());
	    
	    hm1d[it->second][m]["N"]->SetMinimum(0.);
	    hm1d[it->second][m]["N"]->SetMaximum(1.);
	    hm1d[it->second][m]["N"]->Draw();
	    hm1d[it->second][m]["Ghost"]->Draw("Same");
	    
	    TProfile *eff = hm2d[it->second][m]["HitEffInAll"]->ProfileX();
	    TProfile *pur = hm2d[it->second][m]["HitPurInAll"]->ProfileX();
	    
	    eff->SetLineColor(kGreen);
	    pur->SetLineColor(kBlue);
	    eff->Draw("Same");
	    pur->Draw("Same");
	    
	    TLegend *leg = new TLegend(0.65, 0.40, 0.98, 0.60);
	    std::ostringstream os;
	    os << "Number of tracks: " << N;
	    leg->AddEntry(hm1d[it->second][m]["N"], os.str().c_str(), "FP");
	    double intghost = hm1d[it->second][m]["Ghost"]->Integral();
	    intghost /= (double) hm1d[it->second][m]["Ghost"]->GetNbinsX();
	    os.str(""); os << "Ghost rate: " << intghost;
	    leg->AddEntry(hm1d[it->second][m]["Ghost"], os.str().c_str(), "FP");
	    os.str(""); os << "Hit Efficiency: " << (eff->Integral() / (double) eff->GetNbinsX());
	    leg->AddEntry(eff, os.str().c_str(), "FP");
	    
	    os.str(""); os << "Hit Purity: " << (pur->Integral() / (double) pur->GetNbinsX());
	    leg->AddEntry(pur, os.str().c_str(), "FP");
	    leg->Draw();
	    
	    c->Write();
	    delete c;
	    delete leg;
	    
	    // the following makes only sense for plots against p
	    if ("p" != m) continue;
	    // ok, do the efficiency/purity distributions for both p below and
	    // p above 5 GeV
	    int p0 = 1;
	    int p1 = hm1d[it->second][m]["N"]->FindBin(5.);
	    int p2 = hm1d[it->second][m]["N"]->GetNbinsX();
	    os.str(""); os << hm2d[it->second][m]["HitEffInAll"]->GetName() << "_pylo";
	    TH1D *elo = hm2d[it->second][m]["HitEffInAll"]->ProjectionY(os.str().c_str(), p0, p1 - 1);
	    os.str(""); os << hm2d[it->second][m]["HitEffInAll"]->GetName() << "_pyhi";
	    TH1D *ehi = hm2d[it->second][m]["HitEffInAll"]->ProjectionY(os.str().c_str(), p1, p2);
	    os.str(""); os << hm2d[it->second][m]["HitPurInAll"]->GetName() << "_pylo";
	    TH1D *plo = hm2d[it->second][m]["HitPurInAll"]->ProjectionY(os.str().c_str(), p0, p1 - 1);
	    os.str(""); os << hm2d[it->second][m]["HitPurInAll"]->GetName() << "_pyhi";
	    TH1D *phi = hm2d[it->second][m]["HitPurInAll"]->ProjectionY(os.str().c_str(), p1, p2);
	    
	    // normalize
	    elo->Scale(1. / elo->Integral());
	    ehi->Scale(1. / ehi->Integral());
	    plo->Scale(1. / plo->Integral());
	    phi->Scale(1. / phi->Integral());
	    
	    // prepare to draw the stuff
	    elo->SetLineColor(kBlack);
	    ehi->SetLineColor(kRed);
	    plo->SetLineColor(kBlack);
	    phi->SetLineColor(kRed);
	    double max = 0.;
	    if (max < elo->GetMaximum()) max = elo->GetMaximum();
	    if (max < ehi->GetMaximum()) max = ehi->GetMaximum();
	    elo->SetMinimum(0.);
	    elo->SetMaximum(max);
	    max = 0.;
	    if (max < plo->GetMaximum()) max = plo->GetMaximum();
	    if (max < phi->GetMaximum()) max = phi->GetMaximum();
	    plo->SetMinimum(0.);
	    plo->SetMaximum(max);
	    
	    os.str(""); os << hm2d[it->second][m]["HitEffInAll"]->GetName() << "Distrib";
	    c = new TCanvas((os.str() + namsfx).c_str(),
			    (os.str() + " - " + titsfx).c_str());
	    elo->Draw();
	    ehi->Draw("Same");
	    leg = new TLegend(0.0, 0.40, 0.35, 0.60);
	    leg->AddEntry(elo, "p < 5GeV", "FP");
	    leg->AddEntry(ehi, "p >= 5GeV", "FP");
	    leg->Draw();
	    c->Write();
	    delete c;
	    delete leg;
	    
	    os.str(""); os << hm2d[it->second][m]["HitPurInAll"]->GetName() << "Distrib";
	    c = new TCanvas((os.str() + namsfx).c_str(),
			    (os.str() + " - " + titsfx).c_str());
	    plo->Draw("");
	    phi->Draw("Same");
	    leg = new TLegend(0.0, 0.40, 0.35, 0.60);
	    leg->AddEntry(plo, "p < 5GeV", "FP");
	    leg->AddEntry(phi, "p >= 5GeV", "FP");
	    leg->Draw();
	    c->Write();
	    delete c;
	    delete leg;
	 }
      }
   }
   
   f.Write();
}
