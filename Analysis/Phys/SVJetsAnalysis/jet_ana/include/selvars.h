#ifndef selvars_h
#define selvars_h

#include "ROOTOscar.h"
#include "TES.h"

#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector> 
#include <map>
#include <string>
#include <assert.h>
#include "TString.h"
#include "TObject.h"
#include "TObjString.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TMath.h"
#include "Rtypes.h"
#include "TRegexp.h"
#include "TList.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TRandom3.h"

#include <time.h>

#include "JetIDCut.h"

using namespace std;
using namespace TMath;



class selvars {

 public:

  selvars(){

  //lmuon=0; ljet=0; nljet=0;
  //lDiJet=0; lDiJetMu=0;

  SingleIsoMuon15 = new TES();
  JetID = new TES();
  noselection = new TES();
  DiMuonHighPt = new TES();
  StripWmuHighPt_JetID = new TES();
  NeutMuqq = new TES();

  };

  ~selvars(){
   SingleIsoMuon15->ClearVariables();
   JetID->ClearVariables();
   noselection->ClearVariables();
   DiMuonHighPt->ClearVariables();
   StripWmuHighPt_JetID->ClearVariables();
	  //stdjets.erase(stdjets.begin(),stdjets.end());
	  //stdmuons.erase(stdmuons.begin(),stdmuons.end());
	  /*for(ParticlesVector::iterator ip = stdjets.begin() ; ip !=stdjets.end() ; ip++){
		Particle * p = *ip;
		  delete p;
	  }
	  for(ParticlesVector::iterator ip = stdmuons.begin() ; ip !=stdmuons.end() ; ip++){
		Particle * p = *ip;
		  delete p;
	  }*/

  }

  friend class histograms;
  friend class evtsel;
  void Clear();
  bool FillVars_JetID();
  bool FillVars_StripWmuHighPt_JetID();
  bool FillVars_DiMuonHighPt();
  bool FillVars_SingleIsoMuon15();
  bool FillVars_NeutMuqq();

 private:

  TTree 	*_tree;
  TString	_type;
  Int_t		_debug;
  Int_t		_events;
  Bool_t	_isMC;
  Long64_t     _entries;

   Int_t           NRecPVs;
   Int_t           NMaxjets;
   Int_t           NMaxMuons;

   Float_t         stdjet_px[10000];   //[NMaxjets]
   Float_t         stdjet_py[10000];   //[NMaxjets]
   Float_t         stdjet_pz[10000];   //[NMaxjets]
   Float_t         stdjet_e[10000];   //[NMaxjets]
   Float_t         stdjet_pdgid[10000];   //[NMaxjets]
   Float_t         stdjet_bestpvz[10000];   //[NMaxjets]
   Float_t         stdjet_n90[10000];   //[NMaxjets]
   Float_t         stdjet_mtf[10000];   //[NMaxjets]
   Float_t	   stdjet_Ntopotriggers[10000]; //[NMaxjets]
   Float_t	   stdjet_N_HasPVInfo[10000]; //[NMaxjets]

   int NMaxFatjets;
   Float_t fatjet_px[10000];//[NMaxFatjets]
   Float_t fatjet_py[10000];//[NMaxFatjets]
   Float_t fatjet_pz[10000];//[NMaxFatjets]
   Float_t fatjet_e[10000];//[NMaxFatjets]
   Float_t fatjet_bestpvz[10000];//[NMaxFatjets]

   Float_t         stdloosemuon_px[10000];   //[NMaxloosemuons]
   Float_t         stdloosemuon_py[10000];   //[NMaxloosemuons]
   Float_t         stdloosemuon_pz[10000];   //[NMaxloosemuons]
   Float_t         stdloosemuon_e[10000];   //[NMaxloosemuons]
   Float_t         stdloosemuon_pdgid[10000];   //[NMaxloosemuons]
   Float_t         stdloosemuon_bestpvz[10000];   //[NMaxloosemuons]


  public:

  TES *SingleIsoMuon15, *JetID,*StripWmuHighPt_JetID, *noselection, *DiMuonHighPt, *NeutMuqq;

  selvars(TTree *tree, int isMC);
  void GetEntry(int i);
  void FillVars_noselection();
  //void SetVariables();
  //bool SetVariables(bool (*)(Particle*), int i=0);
  //void ClearVariables();
  void SetTree();

void setbranch(const char *name,int &var)
{
  if(_debug>3) cout << "setting branch "  << name << endl;
  _tree->SetBranchStatus(name,1);
  _tree->SetBranchAddress(name,&var);
};

void setbranch(const char *name,bool &var)
{
  if(_debug>3) cout << "setting branch "  << name << endl;
  _tree->SetBranchStatus(name,1);
  _tree->SetBranchAddress(name,&var);
};

void setbranch(const char *name,float &var)
{
  if(_debug>3) cout << "setting branch "  << name << endl;
  _tree->SetBranchStatus(name,1);
  _tree->SetBranchAddress(name,&var);
};

void setbranch(const char *name,float *var)
{
  if(_debug>3) cout << "setting branch "  << name << endl;
  _tree->SetBranchStatus(name,1);
  _tree->SetBranchAddress(name,var);
};

void setbranch(const char *bname, const char *name,float &var)
{
  if(_debug>3) cout << "setting branch "  << name << endl;
  _tree->SetBranchStatus(bname,1);
  TLeaf *l = _tree->GetLeaf(name);
  l->SetAddress(&var);
};

void setbranch(const char *bname, vector<float> *var)
{
  if(_debug>3) cout << "setting branch "  << bname << endl;
  _tree->SetBranchStatus(bname,1);
  _tree->SetBranchAddress(bname,&var);
};

};

#endif //selvars_HPP_
