#ifndef evtsel_h
#define evtsel_h
 
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <map>
#include <string>
#include <assert.h>
#include "TString.h"
#include "TObject.h"
#include "TObjString.h"
#include "TLeaf.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TDirectory.h"
#include "TRandom3.h"
#include "TEnv.h"
#include "TVector3.h"
#include "Rtypes.h"
#include "TRegexp.h"
#include "TList.h"
#include "histograms.h"
#include "selvars.h"

#include "JetIDCut.h"

using namespace std;

class evtsel {

 public:

  evtsel(){};
  ~evtsel(){}

 private:

  TTree *_intree; //TTree

  selvars *_vars; //this will handle all variables used in the evtsel
  TString _type;

  // histograms //
  histograms *noselection;
  histograms *NeutMuqq;
  histograms *IsoMuon15, *JetID, *DiMuonHighPt;
  histograms *StripWmuHighPt_JetID;
/*
  histograms *signal_band_2jet_pretag;
  histograms *signal_band_2jet_pretag_MJDTcut;
  histograms *signal_band_2jet_0tag_MJDTcut;
  histograms *signal_band_2jet_1tag_MJDTcut;
  histograms *signal_band_2jet_2tag_MJDTcut;
  histograms *signal_band_2jet_pretag_MJDTcut_PDTcut;
  histograms *signal_band_2jet_0tag_MJDTcut_PDTcut;
  histograms *signal_band_2jet_1tag_MJDTcut_PDTcut;
  histograms *signal_band_2jet_2tag_MJDTcut_PDTcut;
  histograms *signal_band_2jet_pretag_MJDTcut_invPDTcut;
  histograms *signal_band_2jet_0tag_MJDTcut_invPDTcut;
  histograms *signal_band_2jet_1tag_MJDTcut_invPDTcut;
  histograms *signal_band_2jet_2tag_MJDTcut_invPDTcut;
  histograms *signal_band_2jet_0tag;
  histograms *signal_band_2jet_1tag;
  histograms *signal_band_2jet_2tag;
  histograms *side_band_2jet_pretag;
  histograms *side_band_2jet_pretag_MJDTcut;
  histograms *side_band_2jet_0tag_MJDTcut;
  histograms *side_band_2jet_1tag_MJDTcut;
  histograms *side_band_2jet_2tag_MJDTcut;
  histograms *side_band_2jet_pretag_MJDTcut_PDTcut;
  histograms *side_band_2jet_0tag_MJDTcut_PDTcut;
  histograms *side_band_2jet_1tag_MJDTcut_PDTcut;
  histograms *side_band_2jet_2tag_MJDTcut_PDTcut;
  histograms *side_band_2jet_pretag_MJDTcut_invPDTcut;
  histograms *side_band_2jet_0tag_MJDTcut_invPDTcut;
  histograms *side_band_2jet_1tag_MJDTcut_invPDTcut;
  histograms *side_band_2jet_2tag_MJDTcut_invPDTcut;
  histograms *side_band_2jet_0tag;
  histograms *side_band_2jet_1tag;
  histograms *side_band_2jet_2tag;

  histograms *signal_band_3jet_pretag;
  histograms *signal_band_3jet_pretag_MJDTcut;
  histograms *signal_band_3jet_0tag_MJDTcut;
  histograms *signal_band_3jet_1tag_MJDTcut;
  histograms *signal_band_3jet_2tag_MJDTcut;
  histograms *signal_band_3jet_pretag_MJDTcut_PDTcut;
  histograms *signal_band_3jet_0tag_MJDTcut_PDTcut;
  histograms *signal_band_3jet_1tag_MJDTcut_PDTcut;
  histograms *signal_band_3jet_2tag_MJDTcut_PDTcut;
  histograms *signal_band_3jet_pretag_MJDTcut_invPDTcut;
  histograms *signal_band_3jet_0tag_MJDTcut_invPDTcut;
  histograms *signal_band_3jet_1tag_MJDTcut_invPDTcut;
  histograms *signal_band_3jet_2tag_MJDTcut_invPDTcut;
  histograms *signal_band_3jet_0tag;
  histograms *signal_band_3jet_1tag;
  histograms *signal_band_3jet_2tag;
  histograms *side_band_3jet_pretag;
  histograms *side_band_3jet_pretag_MJDTcut;
  histograms *side_band_3jet_0tag_MJDTcut;
  histograms *side_band_3jet_1tag_MJDTcut;
  histograms *side_band_3jet_2tag_MJDTcut;
  histograms *side_band_3jet_pretag_MJDTcut_PDTcut;
  histograms *side_band_3jet_0tag_MJDTcut_PDTcut;
  histograms *side_band_3jet_1tag_MJDTcut_PDTcut;
  histograms *side_band_3jet_2tag_MJDTcut_PDTcut;
  histograms *side_band_3jet_pretag_MJDTcut_invPDTcut;
  histograms *side_band_3jet_0tag_MJDTcut_invPDTcut;
  histograms *side_band_3jet_1tag_MJDTcut_invPDTcut;
  histograms *side_band_3jet_2tag_MJDTcut_invPDTcut;
  histograms *side_band_3jet_0tag;
  histograms *side_band_3jet_1tag;
  histograms *side_band_3jet_2tag;
  // histograms //
*/
 
  int evttype;
  map <int, TH1F*> cutflow;
  map <int, TH1F*> cutflow_w;
 public:

evtsel(TString name, TFile *file, TFile *outputfile, int isMC);

bool loop();

void finish();

bool passVarCut(Float_t min, Float_t max, Float_t var, TString varname); //passVarCut

bool passRequirement(bool isreq, bool ishas, Float_t weight, TString reqname); //passRequirement

bool passVeto(bool isveto, bool ishas, Float_t weight, Float_t rnd, TString vetoname); //passVeto

void ReadConfigFile(TString name, selvars*& vs);
};
#endif //evtsel_HPP_
