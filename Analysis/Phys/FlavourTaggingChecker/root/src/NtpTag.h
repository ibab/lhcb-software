//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Oct 18 20:22:12 2007 by ROOT version 5.14/00h
// from TTree tagging/BTaggingAnalysis
//////////////////////////////////////////////////////////

#ifndef NtpTag_h
#define NtpTag_h

#include <cmath>
#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TLorentzVector.h>
#include <iostream>

#include "Event.h"
#include "Particle.h"
#include "taggingutils.h"

extern bool removeFiles;

/** @class NtpTag
 *
 *  Class generated by root MakeClass() method and slightly modified
 *  to interface with Event class
 *  @author Marco Musy
 *  @date   2010-07-15
 */
class NtpTag {
public :
   TTree          *fChain;   //pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //current Tree number in a TChain

   // Declaration of leaf types
   Int_t           trig;
   Int_t           kType;
   Int_t           Run;
   Int_t           eventNr; 
   Int_t           evType;
   Int_t           L0TisTos;
   Int_t           HltTisTos;
   Float_t         kx;
   Float_t         ky;
   Float_t         kz;
   Int_t           Tag;
   Float_t         Omega;
   Int_t           TagCat;
   Int_t           TagOS;
   Float_t         OmegaOS;
   Int_t           TagCatOS;
   Int_t           TrueTag;
   Int_t           T;
   Float_t         TaggerType[10];   //[T]
   Float_t         TaggerDecision[10];   //[T]
   Float_t         TaggerOmega[10];   //[T]
   Int_t           bkgCat;
   Int_t           BSosc;
   Float_t         BSx;
   Float_t         BSy;
   Float_t         BSz;
   Int_t           BOID;
   Float_t         BOP;
   Float_t         BOthe;
   Float_t         BOphi;
   Float_t         BOx;
   Float_t         BOy;
   Float_t         BOz;
   Int_t           BOosc;
   Int_t           krec;
   Float_t         RVx;
   Float_t         RVy;
   Float_t         RVz;
   Float_t         Bmass;
   Float_t         tau;
   Float_t         tauErr;
   Float_t         ctChi2;
   Int_t           V;
   Float_t         SecVtx_pt1[100];   //[V]
   Float_t         SecVtx_pt2[100];   //[V]
   Float_t         SecVtx_x[100];   //[V]
   Float_t         SecVtx_y[100];   //[V]
   Float_t         SecVtx_z[100];   //[V]
   Float_t         SecVtx_chi2[100];   //[V]
   Int_t           M;
   Float_t         sID[10];   //[M]
   Float_t         sMothID[10];   //[M]
   Float_t         sP[10];   //[M]
   Float_t         sPt[10];   //[M]
   Float_t         sPhi[10];   //[M]
   Float_t         sMass[10];   //[M]
   Float_t         sMCID[10];   //[M]
   Float_t         sMCMothID[10];   //[M]
   Float_t         sMCP[10];   //[M]
   Float_t         sMCPt[10];   //[M]
   Float_t         sMCPhi[10];   //[M]
   Float_t         sVx[10];   //[M]
   Float_t         sVy[10];   //[M]
   Float_t         sVz[10];   //[M]
   Int_t           N;
   Float_t         ID[200];   //[N]
   Float_t         P[200];   //[N]
   Float_t         Pt[200];   //[N]
   Float_t         phi[200];   //[N]
   Float_t         ch[200];   //[N]
   Float_t         ip[200];   //[N]
   Float_t         iperr[200];   //[N]
   Float_t         ipPU[200];   //[N]
  //extrainfo for PV
   Float_t         ipmean[200];   //[N]
   Float_t         nippu[200];   //[N]
   Float_t         nippuerr[200];   //[N]
   Int_t           tracks[200];   //[N]
   Float_t         trackzfirst[200];   //[N]
   Float_t         trackp[200];   //[N]
   Float_t         ippubs[200];   //[N]
   Float_t         ippuchi2bs[200];   //[N]
   Float_t         zpos[200];   //[N]
   Float_t         zerrpos[200];   //[N]
   //
   Float_t         cloneDist[200];   //[N]
   Float_t         trtyp[200];   //[N]
   Float_t         lcs[200];   //[N]
   Float_t         tsal[200];   //[N]
   Float_t         distPhi[200];   //[N]
   Float_t         veloch[200];   //[N]
   Float_t         EOverP[200];   //[N]
   Float_t         PIDe[200];   //[N]
   Float_t         PIDm[200];   //[N]
   Float_t         PIDk[200];   //[N]
   Float_t         PIDp[200];   //[N]
   Float_t         PIDfl[200];   //[N]
   Float_t         MCID[200];   //[N]
   Float_t         MCP[200];   //[N]
   Float_t         MCPt[200];   //[N]
   Float_t         MCphi[200];   //[N]
   Float_t         MCz[200];   //[N]
   Float_t         mothID[200];   //[N]
   Float_t         ancID[200];   //[N]
   Float_t         bFlag[200];   //[N]
   Float_t         xFlag[200];   //[N]
   Float_t         vFlag[200];   //[N]
   Float_t         IPSV[200];   //[N]
   Float_t         IPSVerr[200];   //[N]
   Float_t         DOCA[200];   //[N]
   Float_t         DOCAerr[200];   //[N]

   // List of branches
   TBranch        *b_trig;   
   TBranch        *b_kType;   
   TBranch        *b_Run;   
   TBranch        *b_eventNr;    
   TBranch        *b_evType;   
   TBranch        *b_L0TisTos;   
   TBranch        *b_HltTisTos;   
   TBranch        *b_kx;   
   TBranch        *b_ky;   
   TBranch        *b_kz;   
   TBranch        *b_Tag;   
   TBranch        *b_Omega;   
   TBranch        *b_TagCat;   
   TBranch        *b_TagOS;   
   TBranch        *b_OmegaOS;   
   TBranch        *b_TagCatOS;   
   TBranch        *b_TrueTag;   
   TBranch        *b_T;   
   TBranch        *b_TaggerType;   
   TBranch        *b_TaggerDecision;   
   TBranch        *b_TaggerOmega;   
   TBranch        *b_bkgCat;   
   TBranch        *b_BSosc;   
   TBranch        *b_BSx;   
   TBranch        *b_BSy;   
   TBranch        *b_BSz;   
   TBranch        *b_BOID;   
   TBranch        *b_BOP;   
   TBranch        *b_BOthe;   
   TBranch        *b_BOphi;   
   TBranch        *b_BOx;   
   TBranch        *b_BOy;   
   TBranch        *b_BOz;   
   TBranch        *b_BOosc;   
   TBranch        *b_krec;   
   TBranch        *b_RVx;   
   TBranch        *b_RVy;   
   TBranch        *b_RVz;   
   TBranch        *b_tau;   
   TBranch        *b_tauErr;   
   TBranch        *b_ctChi2;   
   TBranch        *b_V;   
   TBranch        *b_SecVtx_pt1;   
   TBranch        *b_SecVtx_pt2;   
   TBranch        *b_SecVtx_x;   
   TBranch        *b_SecVtx_y;   
   TBranch        *b_SecVtx_z;   
   TBranch        *b_SecVtx_chi2;   
   TBranch        *b_M;   
   TBranch        *b_sID;   
   TBranch        *b_sMothID;   
   TBranch        *b_sP;   
   TBranch        *b_sPt;   
   TBranch        *b_sPhi;   
   TBranch        *b_sMass;   
   TBranch        *b_sMCID;   
   TBranch        *b_sMCMothID;   
   TBranch        *b_sMCP;   
   TBranch        *b_sMCPt;   
   TBranch        *b_sMCPhi;   
   TBranch        *b_sVx;   
   TBranch        *b_sVy;   
   TBranch        *b_sVz;   
   TBranch        *b_N;   
   TBranch        *b_ID;   
   TBranch        *b_P;   
   TBranch        *b_Pt;   
   TBranch        *b_phi;   
   TBranch        *b_ch;   
   TBranch        *b_ip;   
   TBranch        *b_iperr;   
   TBranch        *b_ipPU;   
  //extrainfo for PV
   TBranch        *b_ipmean;   
   TBranch        *b_nippu;   
   TBranch        *b_nippuerr;
   TBranch        *b_tracks;      
   TBranch        *b_trackzfirst;      
   TBranch        *b_trackp;      
   TBranch        *b_ippubs;   
   TBranch        *b_ippuchi2bs;   
   TBranch        *b_zpos;   
   TBranch        *b_zerrpos;   
   //
   TBranch        *b_cloneDist;   
   TBranch        *b_trtyp;   
   TBranch        *b_lcs;   
   TBranch        *b_tsal;   
   TBranch        *b_distPhi;   
   TBranch        *b_veloch;   
   TBranch        *b_EOverP;   
   TBranch        *b_PIDe;   
   TBranch        *b_PIDm;   
   TBranch        *b_PIDk;   
   TBranch        *b_PIDp;   
   TBranch        *b_PIDfl;   
   TBranch        *b_MCID;   
   TBranch        *b_MCP;   
   TBranch        *b_MCPt;   
   TBranch        *b_MCphi;   
   TBranch        *b_MCz;   
   TBranch        *b_mothID;   
   TBranch        *b_ancID;   
   TBranch        *b_bFlag;   
   TBranch        *b_xFlag;   
   TBranch        *b_vFlag;   
   TBranch        *b_IPSV;   
   TBranch        *b_IPSVerr;   
   TBranch        *b_DOCA;   
   TBranch        *b_DOCAerr;   
 
   NtpTag(TString& name);
   NtpTag(TTree *tree=0);
   virtual ~NtpTag();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Long64_t size();
   virtual bool     execute(Long64_t ientry, Event& event);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
   virtual void decode(const int flags, int& a, int& b, int& c) ;
   virtual void decode(const int flags, int& a, int& b, int& c, int& d, int& e, int& f);

   bool enableMC;

};

#endif

#ifdef NtpTag_cxx


NtpTag::NtpTag(TString& filename)
{
  TTree *tree = new TTree;

  TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(filename);
  if (!f) {
    //f = new TFile(filename);
    f = TFile::Open(filename); 
  }
  tree = (TTree*)gDirectory->Get("BTaggingAnalysis/mytagging");
  //tree = (TTree*)gDirectory->Get("TagAnaBu/mytagging");
   
  if(!tree) {
    if(!f) return;
    cout<<"\nNo tree found for "<<filename<<" skip!\n"<<endl;
    int i = 0;
    if(removeFiles) i= system("rm -v "+filename);//REMOVE physical file!
  }

  Init(tree);

}

void NtpTag::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normaly not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   ///////////////////////////////////////////////
   fChain->SetBranchAddress("trig", &trig, &b_trig);

   enableMC = fChain->GetBranchStatus("kType");
   if(!enableMC) info()<<"...assume reading DATA file!"<<endmsg;
   else fChain->SetBranchAddress("kType", &kType, &b_kType);

   if(fChain->GetBranchStatus("run"))   fChain->SetBranchAddress("run", &Run, &b_Run);
   if(fChain->GetBranchStatus("event")) fChain->SetBranchAddress("event", &eventNr, &b_eventNr);
   if(fChain->GetBranchStatus("Run"))   fChain->SetBranchAddress("Run", &Run, &b_Run);
   if(fChain->GetBranchStatus("Event")) fChain->SetBranchAddress("Event", &eventNr, &b_eventNr);

   fChain->SetBranchAddress("evType", &evType, &b_evType);
   fChain->SetBranchAddress("L0TisTos", &L0TisTos, &b_L0TisTos);
   fChain->SetBranchAddress("HltTisTos", &HltTisTos, &b_HltTisTos);
   if(enableMC) fChain->SetBranchAddress("kx", &kx, &b_kx);
   if(enableMC) fChain->SetBranchAddress("ky", &ky, &b_ky);
   if(enableMC) fChain->SetBranchAddress("kz", &kz, &b_kz);
   fChain->SetBranchAddress("Tag", &Tag, &b_Tag);
   fChain->SetBranchAddress("Omega", &Omega, &b_Omega);
   fChain->SetBranchAddress("TagCat", &TagCat, &b_TagCat);
   fChain->SetBranchAddress("TagOS", &TagOS, &b_TagOS);
   fChain->SetBranchAddress("OmegaOS", &OmegaOS, &b_OmegaOS);
   fChain->SetBranchAddress("TagCatOS", &TagCatOS, &b_TagCatOS);
   if(enableMC) fChain->SetBranchAddress("TrueTag", &TrueTag, &b_TrueTag);
   fChain->SetBranchAddress("T", &T, &b_T);
   fChain->SetBranchAddress("TaggerType", TaggerType, &b_TaggerType);
   fChain->SetBranchAddress("TaggerDecision", TaggerDecision, &b_TaggerDecision);
   fChain->SetBranchAddress("TaggerOmega", TaggerOmega, &b_TaggerOmega);
   if(enableMC) fChain->SetBranchAddress("bkgCat", &bkgCat, &b_bkgCat);
   if(enableMC) fChain->SetBranchAddress("BSosc", &BSosc, &b_BSosc);
   if(enableMC) fChain->SetBranchAddress("BSx", &BSx, &b_BSx);
   if(enableMC) fChain->SetBranchAddress("BSy", &BSy, &b_BSy);
   if(enableMC) fChain->SetBranchAddress("BSz", &BSz, &b_BSz);
   if(enableMC) fChain->SetBranchAddress("BOID", &BOID, &b_BOID);
   if(enableMC) fChain->SetBranchAddress("BOP", &BOP, &b_BOP);
   if(enableMC) fChain->SetBranchAddress("BOthe", &BOthe, &b_BOthe);
   if(enableMC) fChain->SetBranchAddress("BOphi", &BOphi, &b_BOphi);
   if(enableMC) fChain->SetBranchAddress("BOx", &BOx, &b_BOx);
   if(enableMC) fChain->SetBranchAddress("BOy", &BOy, &b_BOy);
   if(enableMC) fChain->SetBranchAddress("BOz", &BOz, &b_BOz);
   if(enableMC) fChain->SetBranchAddress("BOosc", &BOosc, &b_BOosc);
   fChain->SetBranchAddress("krec", &krec, &b_krec);
   fChain->SetBranchAddress("RVx", &RVx, &b_RVx);
   fChain->SetBranchAddress("RVy", &RVy, &b_RVy);
   fChain->SetBranchAddress("RVz", &RVz, &b_RVz);
   fChain->SetBranchAddress("tau", &tau, &b_tau);
   fChain->SetBranchAddress("tauErr", &tauErr, &b_tauErr);
   fChain->SetBranchAddress("ctChi2", &ctChi2, &b_ctChi2);
   fChain->SetBranchAddress("V", &V, &b_V);
   fChain->SetBranchAddress("SecVtx_pt1", SecVtx_pt1, &b_SecVtx_pt1);
   fChain->SetBranchAddress("SecVtx_pt2", SecVtx_pt2, &b_SecVtx_pt2);
   fChain->SetBranchAddress("SecVtx_x", SecVtx_x, &b_SecVtx_x);
   fChain->SetBranchAddress("SecVtx_y", SecVtx_y, &b_SecVtx_y);
   fChain->SetBranchAddress("SecVtx_z", SecVtx_z, &b_SecVtx_z);
   fChain->SetBranchAddress("SecVtx_chi2", SecVtx_chi2, &b_SecVtx_chi2);
   fChain->SetBranchAddress("M", &M, &b_M);
   fChain->SetBranchAddress("sID", sID, &b_sID);
   fChain->SetBranchAddress("sMothID", sMothID, &b_sMothID);
   fChain->SetBranchAddress("sP", sP, &b_sP);
   fChain->SetBranchAddress("sPt", sPt, &b_sPt);
   fChain->SetBranchAddress("sPhi", sPhi, &b_sPhi);
   fChain->SetBranchAddress("sMass", sMass, &b_sMass);
//    if(enableMC) fChain->SetBranchAddress("sMCID", sMCID, &b_sMCID);
//    if(enableMC) fChain->SetBranchAddress("sMCMothID", sMCMothID, &b_sMCMothID);
//    if(enableMC) fChain->SetBranchAddress("sMCP", sMCP, &b_sMCP);
//    if(enableMC) fChain->SetBranchAddress("sMCPt", sMCPt, &b_sMCPt);
//    if(enableMC) fChain->SetBranchAddress("sMCPhi", sMCPhi, &b_sMCPhi);
   fChain->SetBranchAddress("sVx", sVx, &b_sVx);
   fChain->SetBranchAddress("sVy", sVy, &b_sVy);
   fChain->SetBranchAddress("sVz", sVz, &b_sVz);
   fChain->SetBranchAddress("N", &N, &b_N);
   fChain->SetBranchAddress("ID", ID, &b_ID);
   fChain->SetBranchAddress("P", P, &b_P);
   fChain->SetBranchAddress("Pt", Pt, &b_Pt);
   fChain->SetBranchAddress("phi", phi, &b_phi);
   fChain->SetBranchAddress("ch", ch, &b_ch);
   fChain->SetBranchAddress("ip", ip, &b_ip);
   fChain->SetBranchAddress("iperr", iperr, &b_iperr);
   fChain->SetBranchAddress("ipPU", ipPU, &b_ipPU);
   //extrainfo for PV
   fChain->SetBranchAddress("ipmean", ipmean, &b_ipmean);
   fChain->SetBranchAddress("nippu", nippu, &b_nippu);
   fChain->SetBranchAddress("nippuerr", nippuerr, &b_nippuerr);
   fChain->SetBranchAddress("tracks", tracks, &b_tracks);
   fChain->SetBranchAddress("trackzfirst", trackzfirst, &b_trackzfirst);
   fChain->SetBranchAddress("trackp", trackp, &b_trackp);
   fChain->SetBranchAddress("ippubs", ippubs, &b_ippubs);
   fChain->SetBranchAddress("ippuchi2bs", ippuchi2bs, &b_ippuchi2bs);
   fChain->SetBranchAddress("zpos", zpos, &b_zpos);
   fChain->SetBranchAddress("zerrpos", zerrpos, &b_zerrpos);
   //
   fChain->SetBranchAddress("cloneDist", cloneDist, &b_cloneDist);
   fChain->SetBranchAddress("trtyp", trtyp, &b_trtyp);
   fChain->SetBranchAddress("lcs", lcs, &b_lcs);
   fChain->SetBranchAddress("tsal", tsal, &b_tsal);
   fChain->SetBranchAddress("distPhi", distPhi, &b_distPhi);
   fChain->SetBranchAddress("veloch", veloch, &b_veloch);
   fChain->SetBranchAddress("EOverP", EOverP, &b_EOverP);
   fChain->SetBranchAddress("PIDe", PIDe, &b_PIDe);
   fChain->SetBranchAddress("PIDm", PIDm, &b_PIDm);
   fChain->SetBranchAddress("PIDk", PIDk, &b_PIDk);
   fChain->SetBranchAddress("PIDp", PIDp, &b_PIDp);
   fChain->SetBranchAddress("PIDfl", PIDfl, &b_PIDfl);
   if(enableMC) fChain->SetBranchAddress("MCID", MCID, &b_MCID);
   if(enableMC) fChain->SetBranchAddress("MCP", MCP, &b_MCP);
   if(enableMC) fChain->SetBranchAddress("MCPt", MCPt, &b_MCPt);
   if(enableMC) fChain->SetBranchAddress("MCphi", MCphi, &b_MCphi);
   if(enableMC) fChain->SetBranchAddress("MCz", MCz, &b_MCz);
   if(enableMC) fChain->SetBranchAddress("mothID", mothID, &b_mothID);
   if(enableMC) fChain->SetBranchAddress("ancID", ancID, &b_ancID);
   if(enableMC) fChain->SetBranchAddress("bFlag", bFlag, &b_bFlag);
   if(enableMC) fChain->SetBranchAddress("xFlag", xFlag, &b_xFlag);
   fChain->SetBranchAddress("vFlag", vFlag, &b_vFlag);
   fChain->SetBranchAddress("IPSV", IPSV, &b_IPSV);
   fChain->SetBranchAddress("IPSVerr", IPSVerr, &b_IPSVerr);
   fChain->SetBranchAddress("DOCA", DOCA, &b_DOCA);
   fChain->SetBranchAddress("DOCAerr", DOCAerr, &b_DOCAerr);
   
   Notify();
}

Bool_t NtpTag::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normaly not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void NtpTag::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t NtpTag::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}

Long64_t NtpTag::size() {
   if (fChain == 0) return 0;
   return fChain->GetEntriesFast();
}

NtpTag::NtpTag(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("data.root");
      if (!f) {
         f = new TFile("data.root");
      }
      tree = (TTree*)gDirectory->Get("tagging");

   }
   Init(tree);
}

NtpTag::~NtpTag()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t NtpTag::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t NtpTag::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (!fChain->InheritsFrom(TChain::Class()))  return centry;
   TChain *chain = (TChain*)fChain;
   if (chain->GetTreeNumber() != fCurrent) {
      fCurrent = chain->GetTreeNumber();
      Notify();
   }
   return centry;
}
/////////////////////////////////////////////////////////////////
void NtpTag::Loop() {

  if (fChain == 0) return;
  Long64_t nentries = fChain->GetEntriesFast();

  Long64_t nbytes = 0, nb = 0;
  for (Long64_t ievt=0; ievt<nentries; ievt++) {
    Long64_t ientry = LoadTree(ievt);
    if (ientry < 0) break;
    nb = fChain->GetEntry(ievt); nbytes += nb;

  }
}

////////////////////////////////////////////////////////////////////////
void NtpTag::decode(const int flags, int& a, int& b, int& c) {
  a = int(float(flags)/100);
  b = int(float(flags-100*a)/10);
  c = int(float(flags-100*a-10*b)/1);
}
void NtpTag::decode(const int flags, int& a, int& b, int& c, int& d, int& e, int& f) {
  a = int(float(flags) /100000);
  b = int(float(flags-100000*a)/10000);
  c = int(float(flags-100000*a-10000*b)/1000);
  d = int(float(flags-100000*a-10000*b-1000*c)/100);
  e = int(float(flags-100000*a-10000*b-1000*c-100*d)/10);
  f = int(float(flags-100000*a-10000*b-1000*c-100*d-10*e)/1);
}



//======================================================================
#endif // #ifdef NtpTag_cxx
