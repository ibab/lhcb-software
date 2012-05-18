#ifndef histograms_h
#define histograms_h
 
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TList.h"
#include "selvars.h"

using namespace std;

class histograms {

 public:

  ~histograms(){};
  histograms(){};

 private:

  TString _dirname;
  TString _outputname;
  TList *_histolist;

  int _debug;

  TH1F *h1_toptosjets_samePV_lmu  ;
  TH1F *h1_toptosjets_samePV_lmu_pt  ;
  TH1F *h1_toptosjets_samePV_lmu_DPhi_Z0  ;

  TH1F *h1_stdjet_pt;
  TH1F *h1_stdjet_ratio_lmupt_pt;
  TH1F *h1_stdjet_mtf;
  TH1F *h1_stdjet_n90;
  TH1F *h1_stdjet_size;
  TH1F *h1_stdmuon_pt;
  TH1F *h1_lmuon_pt;
  TH1F *h1_ljet_pt;
  TH1F *h1_nljet_pt;
  TH1F *h1_lDijet_Mass;
  TH2F *h2_lDijet_Mass_ljetnljet_ratio;
  TH1F *h1_lDijet_pt;
  TH1F *h1_lDijetMu_Mass;
  TH1F *h1_lDijetMu_pt;
  TH1F *h1_stdjet_nmax;

  TH1F *h1_lDijet_DR ;
  TH1F *h1_lDijet_DPhi;
  TH1F *h1_lDijetMu_DR;
  TH1F *h1_lDijetMu_DPhi;

  TH1F *h1_Dijet_Mass;
  TH1F *h1_Dijet_pt;
  TH1F *h1_DijetMu_Mass;
  TH1F *h1_DijetMu_pt;

  TH1F *h1_lDiMuon_Mass;
  TH1F *h1_lDiMuon_Pt;
  TH1F *h1_lDiMuon_DPhi ;
  TH1F *h1_lDiMuon_DR ;

  TH1F *h1_fatjet_Mass ;
  TH1F *h1_fatjet_Pt ;

 public:

TList* GetHistoList(){return _histolist;};

histograms(TFile *outputfile, TString dirname, int isMC, int debug);  //histograms::histograms
void FillHistos(TES  *_vars);  //fillHistos

TH1F* setupHisto(TString name, int nbins, float xlow, float xhigh) {

   TString hname = name;
   TH1F *h = new TH1F(hname, hname, nbins, xlow, xhigh);
   h->Sumw2();
   _histolist->Add(h);
   return h;
};

TH2F* setupHisto(TString name, int nbinsx, float xlow, float xhigh,
      int nbinsy, float ylow, float yhigh) {

   TString hname = name;
   TH2F *h = new TH2F(hname, hname, nbinsx, xlow, xhigh, nbinsy, ylow, yhigh);
   h->Sumw2();
   _histolist->Add(h);
   return h;
};

TH3F* setupHisto(TString name, int nbinsx, float xlow, float xhigh,
      int nbinsy, float ylow, float yhigh, int nbinsz, float zlow, float zhigh) {

   TString hname = name;
   TH3F *h = new TH3F(hname, hname, nbinsx, xlow, xhigh, nbinsy, ylow, yhigh, nbinsz, zlow, zhigh);
   h->Sumw2();
   _histolist->Add(h);
   return h;
};

void ScaleAll(Float_t scale)
{
   cout << "Scaling all " << _dirname << " histos by " << scale << endl;
   TIter next(_histolist);
   TH1 *iter;
   while( iter==(TH1 *)next()) { iter->Scale(scale); }
}; // ScaleAll


};
#endif //histograms
