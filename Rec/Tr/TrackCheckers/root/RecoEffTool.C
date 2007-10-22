/** RecoEffTool.C
 * 
 * calculate Track reconstruction efficiencies for all track types, as
 * functions of p, pt, subdetector occupancy (both event- and track-averaged)
 *
 * the Loop method below does a pretty sophisticated processing of the NTuple
 * data
 *
 * especially, it's possible to put inefficiencies where they belong, e.g. take
 * an MCParticle which is Long reconstructible but the Velo part of the track
 * has not been found by the reconstruction. then, it's unfair to say that we
 * have a long tracking inefficiency, and we should probably call that a Velo
 * tracking inefficiency, and that's what the code does at the moment. the
 * behaviour of the code for such cases can be modified by tweaking the values
 * of checks, skipchecks and provides, see below...
 *
 * author: Manuel Schiller
 * date: 2006-11-16
 */

#define RecoEffTool_cxx
#include "RecoEffTool.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TLegend.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <set>
#include <map>
#include <list>
#include <cmath>

static bool isB(int PID)
{
  PID = abs(PID);
  if (PID < 500) return false;
  if (PID >= 600) return false;
  return true;
}

void RecoEffTool::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L RecoEffTool.C
//      Root > RecoEffTool t
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

   TFile f("RecoEffTool.root", "Recreate");

   // ok, pack all Track types into a map
   std::map<int, std::string> trackTypes;
   trackTypes.insert(std::make_pair(  0, "Unknown"));
   trackTypes.insert(std::make_pair(  1, "Velo"));
   trackTypes.insert(std::make_pair(  2, "VeloR"));
   trackTypes.insert(std::make_pair(  3, "Long"));
   trackTypes.insert(std::make_pair(  4, "Upstream"));
   trackTypes.insert(std::make_pair(  5, "Downstream"));
   trackTypes.insert(std::make_pair(  6, "Ttrack"));
   trackTypes.insert(std::make_pair(100, "All"));

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
   // the other detectors are generally much lower in occupancy
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

   // ok, pack all similar histos into histmap (track averaged), histemap
   // (event averaged) or histtmap (temporary histos used to fill the other
   // two maps)
   std::map<std::string, std::map<std::string, std::map<std::string, TH1D*> > >
	   histmap, histtmap, histemap;
   //  track avg, tmp histos, event avg
   // loop over track types, booking histograms on our way
   for (std::map<int, std::string>::const_iterator it = trackTypes.begin();
       it != trackTypes.end(); ++it) {
      const std::string &t = it->second;
      for (std::set<std::string>::const_iterator mt = measTypes.begin();
	   mt != measTypes.end(); ++ mt) {
	const std::string &m = *mt;
	// ok, do a small loop to fill histemap, histtmap and histmap at once
	for (int i = 0; i < 3; ++i) {
	  std::string avgtype = // average type string for the title
		  (0 == i)?"(tr-avg)":((1 == i)?"(ev-avg)":"(TMP)");
	  std::string at = // average type string for the name
		  (0 == i)?"TrAvg":((1 == i)?"EvAvg":"TMP");
	  // histogram map to work on...
	  std::map<std::string, std::map<std::string,
		  std::map<std::string, TH1D*> > > &hmap =
			  (0 == i)?histmap:((1 == i)?histemap:histtmap);
	  
	  // pre-construct large parts of the title first
	  std::string str("rel. number of reconstructible MC particles over ");
	  str += m + " for " + t + " tracks " + avgtype;
	  hmap[t][m]["n"] = new TH1D(("n" + t + m + at).c_str(),
				str.c_str(), nbins[m], hbins[m]);
	  hmap[t][m]["n"]->Sumw2();
	  
	  str = "efficiency over " + m + " for " + t + " tracks " + avgtype;
	  hmap[t][m]["eff"] = new TH1D(("eff" + t + m + at).c_str(),
				  str.c_str(), nbins[m], hbins[m]);
	  hmap[t][m]["eff"]->Sumw2();
	  
	  str = "B: fraction of reconstructible MC particles over " +
		  m + " for " + t + " tracks " + avgtype;
	  hmap[t][m]["nB"] = new TH1D(("nB" + t + m + at).c_str(),
				  str.c_str(), nbins[m], hbins[m]);
	  hmap[t][m]["nB"]->Sumw2();
	  
	  str = "B: efficiency over " + m + " for " + t + "tracks " + avgtype;
	  hmap[t][m]["effB"] = new TH1D(("effB" + t + m + at).c_str(),
				  str.c_str(), nbins[m], hbins[m]);
	  hmap[t][m]["effB"]->Sumw2();
	  
	  str = "duplicate fraction over " + m + " for " + t + "tracks " + avgtype;
	  hmap[t][m]["dup"] = new TH1D(("dup" + t + m + at).c_str(),
				  str.c_str(), nbins[m], hbins[m]);
	  hmap[t][m]["dup"]->Sumw2();
      
	  str = "B: duplicate fraction over " + m + " for " + t + "tracks " + avgtype;
	  hmap[t][m]["dupB"] = new TH1D(("dupB" + t + m + at).c_str(),
				  str.c_str(), nbins[m], hbins[m]);
	  hmap[t][m]["dupB"]->Sumw2();
	}
      }
   }

   std::cerr << "RecoEffTool: Standard histograms booked." << std::endl;

   // track types to look at (order is important, if a track of one type is
   // found, it won't be found for a diffenent type, All must be first,
   // Unknown last)
   std::list<std::string> checks;
   checks.push_back("All");
   checks.push_back("Long");
   checks.push_back("Upstream");
   checks.push_back("Velo");
   checks.push_back("VeloR");
   checks.push_back("Downstream");
   checks.push_back("Ttrack");
   checks.push_back("Unknown");
   
   // it is sensible to skip some checks if a track of a certain type
   // has been found, for others not (depends on the order of track types
   // in checks above), e.g. after a Long track, we don't need to look
   // for any of the other track types, whereas for a Velo track, we may
   // still be able to find a Ttrack (if it should have been reconstructed
   // as a Long track, but was not for some reason)
   //
   // note that this depends on the order of the checks defined above
   std::map<std::string, std::set<std::string> > skipchecks;
   std::set<std::string> tmpset;
   // no restrictions on All and Unknown
   skipchecks.insert(make_pair("All", tmpset));
   skipchecks.insert(make_pair("Unknown", tmpset));
   // Long tracks skip these
   tmpset.insert("Upstream"); tmpset.insert("Downstream"); tmpset.insert("Velo"); tmpset.insert("VeloR"); tmpset.insert("Ttrack"); tmpset.insert("Unknown");
   skipchecks.insert(make_pair("Long", tmpset));
   // Upstream tracks skip these
   tmpset.clear(); tmpset.insert("Downstream"); tmpset.insert("Velo"); tmpset.insert("VeloR"); tmpset.insert("Unknown");
   skipchecks.insert(make_pair("Upstream", tmpset));
   // Velo tracks skip these
   tmpset.clear(); tmpset.insert("VeloR"); tmpset.insert("Unknown");
   skipchecks.insert(make_pair("Velo", tmpset));
   // Downstream skips these
   tmpset.clear(); tmpset.insert("Ttrack"); tmpset.insert("Unknown");
   skipchecks.insert(make_pair("Downstream", tmpset));
   // VeloR and Ttrack skip only Unknown
   tmpset.clear(); tmpset.insert("Unknown");
   skipchecks.insert(make_pair("VeloR", tmpset));
   skipchecks.insert(make_pair("Ttrack", tmpset));
   
   // it does also make sense to ask for dependencies between the different
   // track types
   // especially, it's nice to see which track type provides what kind of
   // hits
   std::map<std::string, std::set<std::string> > provides;
   // Long provide these
   tmpset.clear(); tmpset.insert("Velo"); tmpset.insert("T"); //tmpset.insert("VeloR");
   provides.insert(make_pair("Long", tmpset));
   // Upstream tracks provide these
   tmpset.clear(); tmpset.insert("Velo"); tmpset.insert("TT"); //tmpset.insert("VeloR");
   provides.insert(make_pair("Upstream", tmpset));
   // Downstream provide these
   tmpset.clear(); tmpset.insert("TT"); tmpset.insert("T");
   provides.insert(make_pair("Downstream", tmpset));
   // Velo tracks provide these
   tmpset.clear(); tmpset.insert("Velo"); //tmpset.insert("VeloR");
   provides.insert(make_pair("Velo", tmpset));
   // VeloR and Ttrack provide themselves
   tmpset.clear(); tmpset.insert("VeloR");
   provides.insert(make_pair("VeloR", tmpset));
   tmpset.clear(); tmpset.insert("T");
   provides.insert(make_pair("Ttrack", tmpset));
   // All and Unknown do by definition not provide anything
   tmpset.clear();
   provides.insert(make_pair("All", tmpset));
   provides.insert(make_pair("Unknown", tmpset));
   
   Long64_t nbytes = 0, nb = 0, event = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;

      std::cerr << "RecoEffTool: Processing Event " << ++event << ", MCParticle " << std::setw(5) << " ";
      // ok, figure out occupancy of detector components
      // start by counting the total number of hits per subdetector
      long tot_Velo = nVeloHits, tot_TT = nTTHits,
	   tot_IT = nITHits, tot_OT = nOTHits;
      // calculate occupancies from that
      // the magiacal constants in the four lines below come from asking
      // each detector in Brunel v30r9 for the number of channels it has
      double occVelo = double(tot_Velo) / 180224.;
      double occTT = double(tot_TT) / 143360.;
      double occIT = double(tot_IT) / 129024.;
      double occOT = double(tot_OT) / 53760.;
      // these should probably per-detector-component averages and not
      // just channel averages, irrespective of detector component
      // at least to me, this seems the more sensible thing to do
      double occT = .5 * (occIT + occOT);
      double occ = .25 * (occVelo + occTT + occIT + occOT);
      
      // ok, loop over all saved MCParticles
      for (int mcPart = 0; mcPart <= nSavedMCParticles; ++mcPart) {
	if ((mcPart % 32) == 0)
		std::cerr << "\b\b\b\b\b" << std::setw(5) << (mcPart + 1);
	// get momentum
	double p = MCParticles_momentum[mcPart];
	double pt = sqrt(p * p - pow(double(MCParticles_4momentum[mcPart][3]), 2.));

	// a cheap cut to make is to check if the particle comes from
	// primary vertex region, if not, discard it
	if (fabs(MCParticles_origVtxPos[mcPart][2]) > 300.)
		continue;
	
	// set a flag for tracks coming from B mesons
	bool haveB = false;
	// walk up the decay tree until we can walk no further or have a
	// B meson on our hands...
	for (int mother = mcPart; mother > 0;
	    mother = (int) MCParticles_mother[mother])
	  if ((haveB = isB((int) MCParticles_PID[mother])))
	    break;
	
	// set a few flag to determine if the track should be reconstructible
	// in a certain subdetector
	bool isVelo =
	  ((MCParticles_nVeloRHits[mcPart] >= 3) && (MCParticles_nVeloPhiHits[mcPart] >= 3));
	bool isITOT = 
	  (MCParticles_nTXStations[mcPart] >= 3) && (MCParticles_nTStStations[mcPart] >= 3);
	bool isTT = (MCParticles_nTTHits[mcPart] >= 3);

	int nTX = (int) (MCParticles_nITXHits[mcPart] + MCParticles_nOTXHits[mcPart]);
	int nTSt = (int) (MCParticles_nITUHits[mcPart] + MCParticles_nITVHits[mcPart] +
			MCParticles_nOTUHits[mcPart] + MCParticles_nOTVHits[mcPart]);
	double px = MCParticles_4momentum[mcPart][1];
	double py = MCParticles_4momentum[mcPart][2];
	double pz = MCParticles_4momentum[mcPart][3];
	double x = MCParticles_origVtxPos[mcPart][0];
	double y = MCParticles_origVtxPos[mcPart][1];
	double z = MCParticles_origVtxPos[mcPart][2];
	
	bool isLong = isVelo && isITOT;
	bool isUpstream = isVelo && isTT;
	bool isDownstream = isTT && isITOT;
	bool isVeloR = (MCParticles_nVeloRHits[mcPart] >= 3);
	bool isUnknown = isLong || isDownstream || isUpstream || isVeloR || isVelo || isITOT;
	bool isAll = isLong || isUpstream || isVelo || isITOT;

	// we don't need to run if the particle is not reconstructible at all
	if (!isUnknown)
		continue;
	
	// feed a map with references to the reconstructible criterion
	std::map<std::string, bool> reconstructible;
	reconstructible.insert(std::make_pair("Velo", isVelo));
	reconstructible.insert(std::make_pair("VeloR", isVeloR));
	reconstructible.insert(std::make_pair("Long", isLong));
	reconstructible.insert(std::make_pair("Upstream", isUpstream));
	reconstructible.insert(std::make_pair("Downstream", isDownstream));
	reconstructible.insert(std::make_pair("Ttrack", isITOT));
	reconstructible.insert(std::make_pair("All", isAll));
	reconstructible.insert(std::make_pair("Unknown", isUnknown));
	
	std::set<std::string> assocTracks;
	// loop over associated Tracks and find those that are associated
	// with this MC particle
	for (int i = 0; i < 10; ++i) {
	  // check if the entry is valid
	  if (MCParticles_assocTracks[mcPart][i] < 0.) {
	    if (MCParticles_assocTracks[mcPart][i] == -2.) {
		// couldn't save associated Track to our NTuple - make the
		// particle unreconstructible to avoid polluting the
		// statistics
		reconstructible.clear();
	    }
	    continue;
	  }
	  int track = (int) MCParticles_assocTracks[mcPart][i];
	  // ok, loop over MCParticles associated with track to look for a
	  // match
	  bool found = false;
	  for (int j = 0; j < 10; ++j) {
	     if (Tracks_assocMCParts[track][j] == mcPart) {
		     // check weight
		     if (Tracks_assocMCPartWeights[track][j] < 0.7)
			     continue;
		     found = true;
		     break;
	     }
	     if (Tracks_assocMCParts[track][j] == -2.) {
		// guard against missing MCParticles
		reconstructible.clear();
		break;
	     }	     
	  }
	  if (!found)
		  continue;
	  // ok, reconstructed the track
	  assocTracks.insert(trackTypes[(int) Tracks_type[track]]);
	} // end loop over associated Tracks
	
	// a reconstructed track should always appear in the "All"
	// statistics
	if (!assocTracks.empty())
	   assocTracks.insert("All");
	// if we missed a few associations due to limitations in the
	// underlying NTuple, we throw the particle away
	if (reconstructible.empty())
		continue;
	
	// ok, loop over all different track types defined in check
	// we fill the temporary histos first and do the adding up later
	std::list<std::string> checksleft(checks);
	while (!checksleft.empty()) {
	   // ok, get the check to be done
	   std::string it(checksleft.front());
	   // and pop it off the list
	   checksleft.pop_front();
	   // the track can't be reconstructed as it track, so check next track type
	   if (!reconstructible[it]) continue;
	   
	   // check if the algorithm reconstructed the particle
	   bool ineff = (assocTracks.find(it) == assocTracks.end());
	   // in case of an inefficiency, we need to ask if it really is
	   // the fault of the algorithm we're looking at
	   if (ineff) {
	     // if the track can be reconstructed as it track, but was not at
	     // least found in one of the track categories listed in
	     // provides[it], we won't count it (the reason is that it's not
	     // fair to claim e.g. a long track inefficiency if we could not at
	     // least find a Velo seed for it)
	     std::set<std::string> needs(provides[it]);
	     for (std::set<std::string>::const_iterator jt = assocTracks.begin();
		jt != assocTracks.end(); ++jt) {
		   // remove any need met by one of the associated Tracks
		   for (std::set<std::string>::const_iterator kt = provides[*jt].begin();
			kt != provides[*jt].end(); ++kt)
			   needs.erase(*kt);
	     }
	     // ok, skip this one because one of the other algorithms did not
	     // do their work properly
	     // (if there are other algorithms this one depends on - implemented
	     // in the ...size() > 1 condition)
	     if (!needs.empty() && (provides[it].size() > 1)) continue;
	   }

	   // the preprocessor will save us a lot of typing
#define FILLIT(what) { \
	histtmap[it]["p"][what]->Fill(p); \
	histtmap[it]["pt"][what]->Fill(pt); \
	histtmap[it]["VeloOccupancy"][what]->Fill(occVelo); \
	histtmap[it]["TTOccupancy"][what]->Fill(occTT); \
	histtmap[it]["ITOccupancy"][what]->Fill(occIT); \
	histtmap[it]["OTOccupancy"][what]->Fill(occOT); \
	histtmap[it]["TOccupancy"][what]->Fill(occT); \
	histtmap[it]["Occupancy"][what]->Fill(occ); }
	   
	   // ok, efficient or not, it's this algorithm's fault
	   FILLIT("n");
	   if (haveB) FILLIT("nB");
	   // check if an *it type track was found in reconstruction
	   if (!ineff) {
	     FILLIT("eff");
	     if (haveB) FILLIT("effB");
	     // check for duplicates (i.e. particles with more than one
	     // associated track type)
	     if (assocTracks.size() > 2) {
		// "All" is a dummy track type assigned to all reconstructed
		// tracks, we ignore that here, so the number above must be
		// two and not one as one may think
		FILLIT("dup");
		if (haveB) FILLIT("dupB");
	     }
	     // remove checks from our todo-list if they are marked skippable
	     // in skipchecks
	     for (std::set<std::string>::const_iterator skip = skipchecks[it].begin();
		  skip != skipchecks[it].end(); ++skip)
	        checksleft.remove(*skip);
	   }
#undef FILLIT
	} // ok, we're done with the loop over track types
      } // end loop over MCParticles
      std::cerr << "\b\b\b\b\b" << std::setw(5) << nSavedMCParticles << ", done." << std::endl;

      // now comes the adding up work
      for (std::set<std::string>::const_iterator mt = measTypes.begin();
		      mt != measTypes.end(); ++mt) {
	const std::string &m = *mt;
        // for event-averaged quantities, we need to divide everything by
        // the number of particles in the event
        double N = histtmap["All"][m]["n"]->Integral();
        // make sure we don't screw things up when dividing by N or NB
        if (N <= 0.) N = 1.;
        for (std::map<int, std::string>::const_iterator it = trackTypes.begin();
			it != trackTypes.end(); ++it) {
	  const std::string &t = it->second;
	  // add up for track averaged quantities
	  histmap[t][m]["n"]->Add(histtmap[t][m]["n"]);
	  histmap[t][m]["nB"]->Add(histtmap[t][m]["nB"]);
	  histmap[t][m]["eff"]->Add(histtmap[t][m]["eff"]);
	  histmap[t][m]["effB"]->Add(histtmap[t][m]["effB"]);
	  histmap[t][m]["dup"]->Add(histtmap[t][m]["dup"]);
	  histmap[t][m]["dupB"]->Add(histtmap[t][m]["dupB"]);
	  // scale for event averaged ones
	  histtmap[t][m]["n"]->Scale(1. / N);
	  histtmap[t][m]["nB"]->Scale(1. / N);
	  histtmap[t][m]["eff"]->Scale(1. / N);
	  histtmap[t][m]["effB"]->Scale(1. / N);
	  histtmap[t][m]["dup"]->Scale(1. / N);
	  histtmap[t][m]["dupB"]->Scale(1. / N);
	  // add up for event averaged quantities
	  histemap[t][m]["n"]->Add(histtmap[t][m]["n"]);
	  histemap[t][m]["nB"]->Add(histtmap[t][m]["nB"]);
	  histemap[t][m]["eff"]->Add(histtmap[t][m]["eff"]);
	  histemap[t][m]["effB"]->Add(histtmap[t][m]["effB"]);
	  histemap[t][m]["dup"]->Add(histtmap[t][m]["dup"]);
	  histemap[t][m]["dupB"]->Add(histtmap[t][m]["dupB"]);
	  // ok, clear temporary histos (i.e. "zero counters")
	  histtmap[t][m]["n"]->Reset();
	  histtmap[t][m]["nB"]->Reset();
	  histtmap[t][m]["eff"]->Reset();
	  histtmap[t][m]["effB"]->Reset();
	  histtmap[t][m]["dup"]->Reset();
	  histtmap[t][m]["dupB"]->Reset();
	}
      }
   } // end loop over Events

   // ok, delete all temporary histograms - we don't need them any longer
   for (std::map<int, std::string>::const_iterator it = trackTypes.begin();
		   trackTypes.end() != it; ++it) {
     for (std::set<std::string>::const_iterator jt = measTypes.begin();
		     measTypes.end() != jt; ++jt) {
	for (std::map<std::string, TH1D*>::const_iterator kt = histtmap[it->second][*jt].begin();
			histtmap[it->second][*jt].end() != kt; ++kt) {
	   delete kt->second;
	}
     }
   }
      
   // the log file is for integrated quantities...
   ofstream log("RecoEffTool.log", ios::trunc);
   
   // first comes a loop over everything in our set of measurements, e.g.
   // p, pt, Velo occupancy, ...
   for (std::set<std::string>::const_iterator mt = measTypes.begin();
		   measTypes.end() != mt; ++mt) {
     const std::string &m = *mt;
     // ok, now comes an ugly for loop with two iterations, one pass for
     // event averaged quantities, one for track averaged ones
     for (int frutz = 0; frutz < 2; ++frutz) {
       // ok, we need a reference to the histogram map we handling now
       std::map<std::string, std::map<std::string, std::map<std::string, TH1D*> > >
	       &hmap = (frutz)?(histemap):(histmap);
       // and we also need name and title suffixes to distinguish the two
       std::string titsfx(m); titsfx.append((frutz)?" event averaged quantities":" track averaged quantities");
       std::string namsfx(m); namsfx.append((frutz)?"EvAvg":"TrAvg");
       
       // ok, calculate efficiencies
       // eff = # tracks found correctly / # reconstructible tracks
       // we scale the # of tracks per momentum bin so that both histograms
       // can be drawn simultaneously
       // we normalize nB to n as well so we can compare the
       // fraction of B decays

       // ok, do the integrated statistics for the log file
       log << "******************** " << titsfx << " ********************" << endl;
       log << setw(12) << right << "Track type";
       log << setw(10) << right << "#";
       log << setw(10) << right << "# good";
       log << setw(10) << right << "# dup";
       log << setw(10) << right << "eff [%]";
       log << setw(10) << right << "B [%]";
       log << setw(10) << right << "dup [%]";
       log << setw(10) << right << "# B";
       log << setw(10) << right << "# good B";
       log << setw(10) << right << "# dup B";
       log << setw(10) << right << "B-eff [%]";
       log << setw(10) << right << "good B [%]";
       log << setw(10) << right << "B dup [%]" << endl;
       for (std::list<std::string>::const_iterator it = checks.begin();
		       it != checks.end(); ++it) {
         // do intrgrated quatities
	 log << setw(12) << right << *it;
	 log << setw(10) << right << hmap[*it][m]["n"]->Integral();
	 log << setw(10) << right << hmap[*it][m]["eff"]->Integral();
	 log << setw(10) << right << hmap[*it][m]["dup"]->Integral();
	 log << setw(10) << right << 100. * hmap[*it][m]["eff"]->Integral() / hmap[*it][m]["n"]->Integral();
	 log << setw(10) << right << 100. * hmap[*it][m]["nB"]->Integral() / hmap[*it][m]["n"]->Integral();
	 log << setw(10) << right << 100. * hmap[*it][m]["dup"]->Integral() / hmap[*it][m]["n"]->Integral();
	 log << setw(10) << right << hmap[*it][m]["nB"]->Integral();
	 log << setw(10) << right << hmap[*it][m]["effB"]->Integral();
	 log << setw(10) << right << hmap[*it][m]["dupB"]->Integral();
	 log << setw(10) << right << 100. * hmap[*it][m]["effB"]->Integral() / hmap[*it][m]["nB"]->Integral();
	 log << setw(10) << right << 100. * hmap[*it][m]["effB"]->Integral() / hmap[*it][m]["eff"]->Integral();
	 log << setw(10) << right << 100. * hmap[*it][m]["dupB"]->Integral() / hmap[*it][m]["nB"]->Integral();
	 log << endl;
       }
       // now, we manipulate the histos
       for (std::list<std::string>::const_iterator it = checks.begin();
		       it != checks.end(); ++it) {
	 // build the legend for later use
	 TLegend *leg = new TLegend(0.65, 0.80, 0.98, 0.98);
	 ostringstream os;
	 os << "number of MCParticles [arb. units]: " << hmap[*it][m]["n"]->Integral();
	 leg->AddEntry(hmap[*it][m]["n"], os.str().c_str(), "FP");
	 os.str(""); os << "fraction of B daughters: " << hmap[*it][m]["nB"]->Integral() / hmap[*it][m]["n"]->Integral();
	 leg->AddEntry(hmap[*it][m]["nB"], os.str().c_str(), "FP");
	 os.str(""); os << "fraction of duplicates: " << hmap[*it][m]["dup"]->Integral() / hmap[*it][m]["n"]->Integral();
	 leg->AddEntry(hmap[*it][m]["dup"], os.str().c_str(), "FP");
	 os.str(""); os << "fraction of duplicates among B daughters: " << hmap[*it][m]["dupB"]->Integral() / hmap[*it][m]["nB"]->Integral();
	 leg->AddEntry(hmap[*it][m]["dupB"], os.str().c_str(), "FP");
	 os.str(""); os << "efficiency: " << hmap[*it][m]["eff"]->Integral() / hmap[*it][m]["n"]->Integral();
	 leg->AddEntry(hmap[*it][m]["eff"], os.str().c_str(), "FP");
	 os.str(""); os << "efficiency among B daughters: " << hmap[*it][m]["effB"]->Integral() / hmap[*it][m]["nB"]->Integral();
	 leg->AddEntry(hmap[*it][m]["effB"], os.str().c_str(), "FP");
	 // do p,pt,occupancy-dependent calculations
	 hmap[*it][m]["eff"]->Divide(hmap[*it][m]["n"]);
	 hmap[*it][m]["effB"]->Divide(hmap[*it][m]["nB"]);
	 hmap[*it][m]["dup"]->Divide(hmap[*it][m]["n"]);
	 hmap[*it][m]["dupB"]->Divide(hmap[*it][m]["nB"]);
	 hmap[*it][m]["nB"]->Divide(hmap[*it][m]["n"]);
	 hmap[*it][m]["n"]->Scale(1. / hmap[*it][m]["n"]->Integral());
	 if (("p" == m) || ("pt" == m)) hmap[*it][m]["n"]->Divide(binw[m]);
	 // a few quirks to make things plot nicely
	 hmap[*it][m]["n"]->SetMinimum(0.);
	 hmap[*it][m]["n"]->SetMaximum(1.);
	 hmap[*it][m]["nB"]->SetMinimum(0.);
	 hmap[*it][m]["nB"]->SetMaximum(1.);
	 hmap[*it][m]["eff"]->SetMinimum(0.);
	 hmap[*it][m]["eff"]->SetMaximum(1.);
	 hmap[*it][m]["effB"]->SetMinimum(0.);
	 hmap[*it][m]["effB"]->SetMaximum(1.);
	 hmap[*it][m]["dup"]->SetMinimum(0.);
	 hmap[*it][m]["dup"]->SetMaximum(1.);
	 hmap[*it][m]["dupB"]->SetMinimum(0.);
	 hmap[*it][m]["dupB"]->SetMaximum(1.);
	 
	 hmap[*it][m]["n"]->SetLineColor(kBlack);
	 hmap[*it][m]["nB"]->SetLineColor(kBlue);
	 hmap[*it][m]["eff"]->SetLineColor(kRed);
	 hmap[*it][m]["effB"]->SetLineColor(kGreen);
	 hmap[*it][m]["dup"]->SetLineColor(kMagenta);
	 hmap[*it][m]["dupB"]->SetLineColor(kCyan);
       
	 hmap[*it][m]["n"]->SetStats(kFALSE);
	 hmap[*it][m]["nB"]->SetStats(kFALSE);
	 hmap[*it][m]["eff"]->SetStats(kFALSE);
	 hmap[*it][m]["effB"]->SetStats(kFALSE);
	 hmap[*it][m]["dup"]->SetStats(kFALSE);
	 hmap[*it][m]["dupB"]->SetStats(kFALSE);
	 
	 // make title for x axis
	 std::string axtit(m);
	 if (("p" == m) || ("pt" == m)) axtit.append(" [GeV/c]");
	 hmap[*it][m]["eff"]->GetXaxis()->SetTitle(axtit.c_str());
	 
	 TCanvas *c = new TCanvas(std::string(*it).append(namsfx).c_str(),
			 std::string(*it).append(" - ").append(titsfx).c_str());
	 hmap[*it][m]["eff"]->Draw("9,E1");
	 hmap[*it][m]["effB"]->Draw("9,E1,Same");
	 hmap[*it][m]["n"]->Draw("9,E1,Same");
	 hmap[*it][m]["nB"]->Draw("9,E1,Same");
	 hmap[*it][m]["dup"]->Draw("9,E1,Same");
	 hmap[*it][m]["dupB"]->Draw("9,E1,Same");
	 leg->Draw("Same");
	 c->Write();
	 delete c;
	 delete leg;
       }
     }
   }
   
   f.Write();
}

