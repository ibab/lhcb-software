//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sun Nov  4 10:49:01 2012 by ROOT version 5.34/00
// from TTree D0KKTuple/D0KKTuple
// found on file: castor:///castor/cern.ch/user/g/gersabec/charm/swimming_2011/swimming_production/D2KK_MagUp_swimming_raw_output_v5.root
//////////////////////////////////////////////////////////

#ifndef CorrectVELOSwimInfo_D02PiPi_h
#define CorrectVELOSwimInfo_D02PiPi_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <string>

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.
const Int_t kMaxDstar_OWNPV_COV = 1;
const Int_t kMaxDstar_ENDVERTEX_COV = 1;
const Int_t kMaxslowPi_OWNPV_COV = 1;
const Int_t kMaxslowPi_ORIVX_COV = 1;
const Int_t kMaxD0_OWNPV_COV = 1;
const Int_t kMaxD0_ORIVX_COV = 1;
const Int_t kMaxD0_ENDVERTEX_COV = 1;
const Int_t kMaxdauplus_OWNPV_COV = 1;
const Int_t kMaxdauplus_ORIVX_COV = 1;
const Int_t kMaxdauminus_OWNPV_COV = 1;
const Int_t kMaxdauminus_ORIVX_COV = 1;

class CorrectVELOSwimInfo_D02PiPi {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Float_t         Dstar_MINIP;
   Float_t         Dstar_MINIPCHI2;
   Float_t         Dstar_MINIPNEXTBEST;
   Float_t         Dstar_MINIPCHI2NEXTBEST;
   Float_t         Dstar_OWNPV_X;
   Float_t         Dstar_OWNPV_Y;
   Float_t         Dstar_OWNPV_Z;
   Float_t         Dstar_OWNPV_XERR;
   Float_t         Dstar_OWNPV_YERR;
   Float_t         Dstar_OWNPV_ZERR;
   Float_t         Dstar_OWNPV_CHI2;
   Int_t           Dstar_OWNPV_NDOF;
   Float_t         Dstar_OWNPV_COV_[3][3];
   Float_t         Dstar_IP_OWNPV;
   Float_t         Dstar_IPCHI2_OWNPV;
   Float_t         Dstar_FD_OWNPV;
   Float_t         Dstar_FDCHI2_OWNPV;
   Float_t         Dstar_DIRA_OWNPV;
   Float_t         Dstar_ENDVERTEX_X;
   Float_t         Dstar_ENDVERTEX_Y;
   Float_t         Dstar_ENDVERTEX_Z;
   Float_t         Dstar_ENDVERTEX_XERR;
   Float_t         Dstar_ENDVERTEX_YERR;
   Float_t         Dstar_ENDVERTEX_ZERR;
   Float_t         Dstar_ENDVERTEX_CHI2;
   Int_t           Dstar_ENDVERTEX_NDOF;
   Float_t         Dstar_ENDVERTEX_COV_[3][3];
   Float_t         Dstar_P;
   Float_t         Dstar_PT;
   Float_t         Dstar_PE;
   Float_t         Dstar_PX;
   Float_t         Dstar_PY;
   Float_t         Dstar_PZ;
   Float_t         Dstar_MM;
   Float_t         Dstar_MMERR;
   Float_t         Dstar_M;
   Int_t           Dstar_ID;
   Float_t         Dstar_DTF_CHI2;
   Float_t         Dstar_DTF_D0_BPVIPCHI2;
   Float_t         Dstar_DTF_D0_E;
   Float_t         Dstar_DTF_D0_M;
   Float_t         Dstar_DTF_D0_P;
   Float_t         Dstar_DTF_D0_PT;
   Float_t         Dstar_DTF_D0_PX;
   Float_t         Dstar_DTF_D0_PY;
   Float_t         Dstar_DTF_D0_PZ;
   Float_t         Dstar_DTF_Dstar_E;
   Float_t         Dstar_DTF_Dstar_M;
   Float_t         Dstar_DTF_Dstar_P;
   Float_t         Dstar_DTF_Dstar_PT;
   Float_t         Dstar_DTF_Dstar_PX;
   Float_t         Dstar_DTF_Dstar_PY;
   Float_t         Dstar_DTF_Dstar_PZ;
   Float_t         Dstar_DTF_NDOF;
   Float_t         Dstar_DTF_Pis_BPVIPCHI2;
   Float_t         Dstar_DTF_Pis_E;
   Float_t         Dstar_DTF_Pis_M;
   Float_t         Dstar_DTF_Pis_P;
   Float_t         Dstar_DTF_Pis_PT;
   Float_t         Dstar_DTF_Pis_PX;
   Float_t         Dstar_DTF_Pis_PY;
   Float_t         Dstar_DTF_Pis_PZ;
   Float_t         slowPi_MINIP;
   Float_t         slowPi_MINIPCHI2;
   Float_t         slowPi_MINIPNEXTBEST;
   Float_t         slowPi_MINIPCHI2NEXTBEST;
   Float_t         slowPi_OWNPV_X;
   Float_t         slowPi_OWNPV_Y;
   Float_t         slowPi_OWNPV_Z;
   Float_t         slowPi_OWNPV_XERR;
   Float_t         slowPi_OWNPV_YERR;
   Float_t         slowPi_OWNPV_ZERR;
   Float_t         slowPi_OWNPV_CHI2;
   Int_t           slowPi_OWNPV_NDOF;
   Float_t         slowPi_OWNPV_COV_[3][3];
   Float_t         slowPi_IP_OWNPV;
   Float_t         slowPi_IPCHI2_OWNPV;
   Float_t         slowPi_ORIVX_X;
   Float_t         slowPi_ORIVX_Y;
   Float_t         slowPi_ORIVX_Z;
   Float_t         slowPi_ORIVX_XERR;
   Float_t         slowPi_ORIVX_YERR;
   Float_t         slowPi_ORIVX_ZERR;
   Float_t         slowPi_ORIVX_CHI2;
   Int_t           slowPi_ORIVX_NDOF;
   Float_t         slowPi_ORIVX_COV_[3][3];
   Float_t         slowPi_P;
   Float_t         slowPi_PT;
   Float_t         slowPi_PE;
   Float_t         slowPi_PX;
   Float_t         slowPi_PY;
   Float_t         slowPi_PZ;
   Float_t         slowPi_M;
   Int_t           slowPi_ID;
   Float_t         slowPi_PIDe;
   Float_t         slowPi_PIDmu;
   Float_t         slowPi_PIDK;
   Float_t         slowPi_PIDp;
   Float_t         slowPi_ProbNNe;
   Float_t         slowPi_ProbNNk;
   Float_t         slowPi_ProbNNp;
   Float_t         slowPi_ProbNNpi;
   Float_t         slowPi_ProbNNmu;
   Float_t         slowPi_ProbNNghost;
   Float_t         slowPi_CaloEcalE;
   Float_t         slowPi_CaloHcalE;
   Int_t           slowPi_hasMuon;
   Int_t           slowPi_isMuon;
   Int_t           slowPi_hasRich;
   Int_t           slowPi_hasCalo;
   Int_t           slowPi_TRACK_Type;
   Int_t           slowPi_TRACK_Key;
   Float_t         slowPi_TRACK_CHI2NDOF;
   Float_t         slowPi_TRACK_PCHI2;
   Float_t         slowPi_TRACK_GhostProb;
   Float_t         slowPi_TRACK_CloneDist;
   Float_t         D0_MINIP;
   Float_t         D0_MINIPCHI2;
   Float_t         D0_MINIPNEXTBEST;
   Float_t         D0_MINIPCHI2NEXTBEST;
   Float_t         D0_OWNPV_X;
   Float_t         D0_OWNPV_Y;
   Float_t         D0_OWNPV_Z;
   Float_t         D0_OWNPV_XERR;
   Float_t         D0_OWNPV_YERR;
   Float_t         D0_OWNPV_ZERR;
   Float_t         D0_OWNPV_CHI2;
   Int_t           D0_OWNPV_NDOF;
   Float_t         D0_OWNPV_COV_[3][3];
   Float_t         D0_IP_OWNPV;
   Float_t         D0_IPCHI2_OWNPV;
   Float_t         D0_FD_OWNPV;
   Float_t         D0_FDCHI2_OWNPV;
   Float_t         D0_DIRA_OWNPV;
   Float_t         D0_ORIVX_X;
   Float_t         D0_ORIVX_Y;
   Float_t         D0_ORIVX_Z;
   Float_t         D0_ORIVX_XERR;
   Float_t         D0_ORIVX_YERR;
   Float_t         D0_ORIVX_ZERR;
   Float_t         D0_ORIVX_CHI2;
   Int_t           D0_ORIVX_NDOF;
   Float_t         D0_ORIVX_COV_[3][3];
   Float_t         D0_FD_ORIVX;
   Float_t         D0_FDCHI2_ORIVX;
   Float_t         D0_DIRA_ORIVX;
   Float_t         D0_ENDVERTEX_X;
   Float_t         D0_ENDVERTEX_Y;
   Float_t         D0_ENDVERTEX_Z;
   Float_t         D0_ENDVERTEX_XERR;
   Float_t         D0_ENDVERTEX_YERR;
   Float_t         D0_ENDVERTEX_ZERR;
   Float_t         D0_ENDVERTEX_CHI2;
   Int_t           D0_ENDVERTEX_NDOF;
   Float_t         D0_ENDVERTEX_COV_[3][3];
   Float_t         D0_P;
   Float_t         D0_PT;
   Float_t         D0_PE;
   Float_t         D0_PX;
   Float_t         D0_PY;
   Float_t         D0_PZ;
   Float_t         D0_MM;
   Float_t         D0_MMERR;
   Float_t         D0_M;
   Int_t           D0_ID;
   Float_t         D0_TAU;
   Float_t         D0_TAUERR;
   Float_t         D0_TAUCHI2;
   Int_t           D0_Stripping_nTP;
   Float_t         D0_Stripping_TP_RAW[200];   //[D0_Stripping_nTP]
   Float_t         D0_Stripping_TP_DEC[200];   //[D0_Stripping_nTP]
   Float_t         D0_Stripping_TP_TAU[200];   //[D0_Stripping_nTP]
   Float_t         D0_Stripping_TP_IP[200];   //[D0_Stripping_nTP]
   Int_t           D0_nTP;
   Float_t         D0_TP_DEC__Event_Phys_D2hhPromptD2PiPiLine[200];   //[D0_nTP]
   Int_t           D0_Trigger_nTP;
   Float_t         D0_Trigger_TP_RAW[200];   //[D0_Trigger_nTP]
   Float_t         D0_Trigger_TP_DEC[200];   //[D0_Trigger_nTP]
   Float_t         D0_Trigger_TP_TAU[200];   //[D0_Trigger_nTP]
   Float_t         D0_Trigger_TP_IP[200];   //[D0_Trigger_nTP]
   Float_t         D0_TP_DEC_Hlt1TrackAllL0Decision[200];   //[D0_nTP]
   Float_t         D0_TP_DEC_Hlt2CharmHadD02HH_D02PiPiDecision[200];   //[D0_nTP]
   Float_t         D0_TP_DEC_Hlt2CharmHadD02HH_D02PiPiWideMassDecision[200];   //[D0_nTP]
   Int_t           D0L0Global_Dec;
   Int_t           D0L0Global_TIS;
   Int_t           D0L0Global_TOS;
   Int_t           D0Hlt1Global_Dec;
   Int_t           D0Hlt1Global_TIS;
   Int_t           D0Hlt1Global_TOS;
   Int_t           D0Hlt2Global_Dec;
   Int_t           D0Hlt2Global_TIS;
   Int_t           D0Hlt2Global_TOS;
   Int_t           D0StrippingGlobal_Dec;
   Int_t           D0StrippingGlobal_TIS;
   Int_t           D0StrippingGlobal_TOS;
   Int_t           D0L0MuonDecision_Dec;
   Int_t           D0L0MuonDecision_TIS;
   Int_t           D0L0MuonDecision_TOS;
   Int_t           D0L0HadronDecision_Dec;
   Int_t           D0L0HadronDecision_TIS;
   Int_t           D0L0HadronDecision_TOS;
   Int_t           D0L0ElectronDecision_Dec;
   Int_t           D0L0ElectronDecision_TIS;
   Int_t           D0L0ElectronDecision_TOS;
   Int_t           D0L0PhotonDecision_Dec;
   Int_t           D0L0PhotonDecision_TIS;
   Int_t           D0L0PhotonDecision_TOS;
   Int_t           D0Hlt1TrackAllL0Decision_Dec;
   Int_t           D0Hlt1TrackAllL0Decision_TIS;
   Int_t           D0Hlt1TrackAllL0Decision_TOS;
   Int_t           D0Hlt2CharmHadD02HH_D02KPiDecision_Dec;
   Int_t           D0Hlt2CharmHadD02HH_D02KPiDecision_TIS;
   Int_t           D0Hlt2CharmHadD02HH_D02KPiDecision_TOS;
   Int_t           D0Hlt2CharmHadD02HH_D02KPiWideMassDecision_Dec;
   Int_t           D0Hlt2CharmHadD02HH_D02KPiWideMassDecision_TIS;
   Int_t           D0Hlt2CharmHadD02HH_D02KPiWideMassDecision_TOS;
   Int_t           D0Hlt2CharmHadD02HH_D02KKDecision_Dec;
   Int_t           D0Hlt2CharmHadD02HH_D02KKDecision_TIS;
   Int_t           D0Hlt2CharmHadD02HH_D02KKDecision_TOS;
   Int_t           D0Hlt2CharmHadD02HH_D02KKWideMassDecision_Dec;
   Int_t           D0Hlt2CharmHadD02HH_D02KKWideMassDecision_TIS;
   Int_t           D0Hlt2CharmHadD02HH_D02KKWideMassDecision_TOS;
   Int_t           D0Hlt2CharmHadD02HH_D02PiPiDecision_Dec;
   Int_t           D0Hlt2CharmHadD02HH_D02PiPiDecision_TIS;
   Int_t           D0Hlt2CharmHadD02HH_D02PiPiDecision_TOS;
   Int_t           D0Hlt2CharmHadD02HH_D02PiPiWideMassDecision_Dec;
   Int_t           D0Hlt2CharmHadD02HH_D02PiPiWideMassDecision_TIS;
   Int_t           D0Hlt2CharmHadD02HH_D02PiPiWideMassDecision_TOS;
   Float_t         dauplus_MINIP;
   Float_t         dauplus_MINIPCHI2;
   Float_t         dauplus_MINIPNEXTBEST;
   Float_t         dauplus_MINIPCHI2NEXTBEST;
   Float_t         dauplus_OWNPV_X;
   Float_t         dauplus_OWNPV_Y;
   Float_t         dauplus_OWNPV_Z;
   Float_t         dauplus_OWNPV_XERR;
   Float_t         dauplus_OWNPV_YERR;
   Float_t         dauplus_OWNPV_ZERR;
   Float_t         dauplus_OWNPV_CHI2;
   Int_t           dauplus_OWNPV_NDOF;
   Float_t         dauplus_OWNPV_COV_[3][3];
   Float_t         dauplus_IP_OWNPV;
   Float_t         dauplus_IPCHI2_OWNPV;
   Float_t         dauplus_ORIVX_X;
   Float_t         dauplus_ORIVX_Y;
   Float_t         dauplus_ORIVX_Z;
   Float_t         dauplus_ORIVX_XERR;
   Float_t         dauplus_ORIVX_YERR;
   Float_t         dauplus_ORIVX_ZERR;
   Float_t         dauplus_ORIVX_CHI2;
   Int_t           dauplus_ORIVX_NDOF;
   Float_t         dauplus_ORIVX_COV_[3][3];
   Float_t         dauplus_P;
   Float_t         dauplus_PT;
   Float_t         dauplus_PE;
   Float_t         dauplus_PX;
   Float_t         dauplus_PY;
   Float_t         dauplus_PZ;
   Float_t         dauplus_M;
   Int_t           dauplus_ID;
   Float_t         dauplus_PIDe;
   Float_t         dauplus_PIDmu;
   Float_t         dauplus_PIDK;
   Float_t         dauplus_PIDp;
   Float_t         dauplus_ProbNNe;
   Float_t         dauplus_ProbNNk;
   Float_t         dauplus_ProbNNp;
   Float_t         dauplus_ProbNNpi;
   Float_t         dauplus_ProbNNmu;
   Float_t         dauplus_ProbNNghost;
   Float_t         dauplus_CaloEcalE;
   Float_t         dauplus_CaloHcalE;
   Int_t           dauplus_hasMuon;
   Int_t           dauplus_isMuon;
   Int_t           dauplus_hasRich;
   Int_t           dauplus_hasCalo;
   Int_t           dauplus_TRACK_Type;
   Int_t           dauplus_TRACK_Key;
   Float_t         dauplus_TRACK_CHI2NDOF;
   Float_t         dauplus_TRACK_PCHI2;
   Float_t         dauplus_TRACK_GhostProb;
   Float_t         dauplus_TRACK_CloneDist;
   Int_t           dauplus_Trigger_nTP;
   Float_t         dauplus_Trigger_TP_DEC_trRec_HLT1[200];   //[dauplus_Trigger_nTP]
   Float_t         dauplus_Trigger_TP_DEC_trRec_HLT2[200];   //[dauplus_Trigger_nTP]
   Float_t         dauplus_Trigger_TP_DEC_trRec_VELO_HLT1[200];   //[dauplus_Trigger_nTP]
   Float_t         dauplus_Trigger_TP_DEC_trRec_VELO_OFF[200];   //[dauplus_Trigger_nTP]
   Float_t         dauplus_Trigger_TP_DEC_trTOS_Hlt1TrackAllL0Decision[200];   //[dauplus_Trigger_nTP]
   Float_t         dauplus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiDecision[200];   //[dauplus_Trigger_nTP]
   Float_t         dauplus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiWideMassDecision[200];   //[dauplus_Trigger_nTP]
   Float_t         dauminus_MINIP;
   Float_t         dauminus_MINIPCHI2;
   Float_t         dauminus_MINIPNEXTBEST;
   Float_t         dauminus_MINIPCHI2NEXTBEST;
   Float_t         dauminus_OWNPV_X;
   Float_t         dauminus_OWNPV_Y;
   Float_t         dauminus_OWNPV_Z;
   Float_t         dauminus_OWNPV_XERR;
   Float_t         dauminus_OWNPV_YERR;
   Float_t         dauminus_OWNPV_ZERR;
   Float_t         dauminus_OWNPV_CHI2;
   Int_t           dauminus_OWNPV_NDOF;
   Float_t         dauminus_OWNPV_COV_[3][3];
   Float_t         dauminus_IP_OWNPV;
   Float_t         dauminus_IPCHI2_OWNPV;
   Float_t         dauminus_ORIVX_X;
   Float_t         dauminus_ORIVX_Y;
   Float_t         dauminus_ORIVX_Z;
   Float_t         dauminus_ORIVX_XERR;
   Float_t         dauminus_ORIVX_YERR;
   Float_t         dauminus_ORIVX_ZERR;
   Float_t         dauminus_ORIVX_CHI2;
   Int_t           dauminus_ORIVX_NDOF;
   Float_t         dauminus_ORIVX_COV_[3][3];
   Float_t         dauminus_P;
   Float_t         dauminus_PT;
   Float_t         dauminus_PE;
   Float_t         dauminus_PX;
   Float_t         dauminus_PY;
   Float_t         dauminus_PZ;
   Float_t         dauminus_M;
   Int_t           dauminus_ID;
   Float_t         dauminus_PIDe;
   Float_t         dauminus_PIDmu;
   Float_t         dauminus_PIDK;
   Float_t         dauminus_PIDp;
   Float_t         dauminus_ProbNNe;
   Float_t         dauminus_ProbNNk;
   Float_t         dauminus_ProbNNp;
   Float_t         dauminus_ProbNNpi;
   Float_t         dauminus_ProbNNmu;
   Float_t         dauminus_ProbNNghost;
   Float_t         dauminus_CaloEcalE;
   Float_t         dauminus_CaloHcalE;
   Int_t           dauminus_hasMuon;
   Int_t           dauminus_isMuon;
   Int_t           dauminus_hasRich;
   Int_t           dauminus_hasCalo;
   Int_t           dauminus_TRACK_Type;
   Int_t           dauminus_TRACK_Key;
   Float_t         dauminus_TRACK_CHI2NDOF;
   Float_t         dauminus_TRACK_PCHI2;
   Float_t         dauminus_TRACK_GhostProb;
   Float_t         dauminus_TRACK_CloneDist;
   Int_t           dauminus_Trigger_nTP;
   Float_t         dauminus_Trigger_TP_DEC_trRec_HLT1[200];   //[dauminus_Trigger_nTP]
   Float_t         dauminus_Trigger_TP_DEC_trRec_HLT2[200];   //[dauminus_Trigger_nTP]
   Float_t         dauminus_Trigger_TP_DEC_trRec_VELO_HLT1[200];   //[dauminus_Trigger_nTP]
   Float_t         dauminus_Trigger_TP_DEC_trRec_VELO_OFF[200];   //[dauminus_Trigger_nTP]
   Float_t         dauminus_Trigger_TP_DEC_trTOS_Hlt1TrackAllL0Decision[200];   //[dauminus_Trigger_nTP]
   Float_t         dauminus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiDecision[200];   //[dauminus_Trigger_nTP]
   Float_t         dauminus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiWideMassDecision[200];   //[dauminus_Trigger_nTP]
   Int_t           nCandidate;
   Int_t           totCandidates;
   Int_t           EventInSequence;
   Int_t           runNumber;
   Float_t         eventNumber;
   Int_t           BCID;
   Int_t           BCType;
   Int_t           OdinTCK;
   Int_t           L0DUTCK;
   Int_t           HLTTCK;
   Float_t         GpsTime;
   Int_t           Primaries;
   Int_t           Polarity;
   Int_t           nPV;
   Float_t         PVX[200];   //[nPV]
   Float_t         PVY[200];   //[nPV]
   Float_t         PVZ[200];   //[nPV]
   Float_t         PVXERR[200];   //[nPV]
   Float_t         PVYERR[200];   //[nPV]
   Float_t         PVZERR[200];   //[nPV]
   Float_t         PVCHI2[200];   //[nPV]
   Float_t         PVNDOF[200];   //[nPV]
   Float_t         PVNTRACKS[200];   //[nPV]

   // List of branches
   TBranch        *b_Dstar_MINIP;   //!
   TBranch        *b_Dstar_MINIPCHI2;   //!
   TBranch        *b_Dstar_MINIPNEXTBEST;   //!
   TBranch        *b_Dstar_MINIPCHI2NEXTBEST;   //!
   TBranch        *b_Dstar_OWNPV_X;   //!
   TBranch        *b_Dstar_OWNPV_Y;   //!
   TBranch        *b_Dstar_OWNPV_Z;   //!
   TBranch        *b_Dstar_OWNPV_XERR;   //!
   TBranch        *b_Dstar_OWNPV_YERR;   //!
   TBranch        *b_Dstar_OWNPV_ZERR;   //!
   TBranch        *b_Dstar_OWNPV_CHI2;   //!
   TBranch        *b_Dstar_OWNPV_NDOF;   //!
   TBranch        *b_Dstar_OWNPV_COV_;   //!
   TBranch        *b_Dstar_IP_OWNPV;   //!
   TBranch        *b_Dstar_IPCHI2_OWNPV;   //!
   TBranch        *b_Dstar_FD_OWNPV;   //!
   TBranch        *b_Dstar_FDCHI2_OWNPV;   //!
   TBranch        *b_Dstar_DIRA_OWNPV;   //!
   TBranch        *b_Dstar_ENDVERTEX_X;   //!
   TBranch        *b_Dstar_ENDVERTEX_Y;   //!
   TBranch        *b_Dstar_ENDVERTEX_Z;   //!
   TBranch        *b_Dstar_ENDVERTEX_XERR;   //!
   TBranch        *b_Dstar_ENDVERTEX_YERR;   //!
   TBranch        *b_Dstar_ENDVERTEX_ZERR;   //!
   TBranch        *b_Dstar_ENDVERTEX_CHI2;   //!
   TBranch        *b_Dstar_ENDVERTEX_NDOF;   //!
   TBranch        *b_Dstar_ENDVERTEX_COV_;   //!
   TBranch        *b_Dstar_P;   //!
   TBranch        *b_Dstar_PT;   //!
   TBranch        *b_Dstar_PE;   //!
   TBranch        *b_Dstar_PX;   //!
   TBranch        *b_Dstar_PY;   //!
   TBranch        *b_Dstar_PZ;   //!
   TBranch        *b_Dstar_MM;   //!
   TBranch        *b_Dstar_MMERR;   //!
   TBranch        *b_Dstar_M;   //!
   TBranch        *b_Dstar_ID;   //!
   TBranch        *b_Dstar_DTF_CHI2;   //!
   TBranch        *b_Dstar_DTF_D0_BPVIPCHI2;   //!
   TBranch        *b_Dstar_DTF_D0_E;   //!
   TBranch        *b_Dstar_DTF_D0_M;   //!
   TBranch        *b_Dstar_DTF_D0_P;   //!
   TBranch        *b_Dstar_DTF_D0_PT;   //!
   TBranch        *b_Dstar_DTF_D0_PX;   //!
   TBranch        *b_Dstar_DTF_D0_PY;   //!
   TBranch        *b_Dstar_DTF_D0_PZ;   //!
   TBranch        *b_Dstar_DTF_Dstar_E;   //!
   TBranch        *b_Dstar_DTF_Dstar_M;   //!
   TBranch        *b_Dstar_DTF_Dstar_P;   //!
   TBranch        *b_Dstar_DTF_Dstar_PT;   //!
   TBranch        *b_Dstar_DTF_Dstar_PX;   //!
   TBranch        *b_Dstar_DTF_Dstar_PY;   //!
   TBranch        *b_Dstar_DTF_Dstar_PZ;   //!
   TBranch        *b_Dstar_DTF_NDOF;   //!
   TBranch        *b_Dstar_DTF_Pis_BPVIPCHI2;   //!
   TBranch        *b_Dstar_DTF_Pis_E;   //!
   TBranch        *b_Dstar_DTF_Pis_M;   //!
   TBranch        *b_Dstar_DTF_Pis_P;   //!
   TBranch        *b_Dstar_DTF_Pis_PT;   //!
   TBranch        *b_Dstar_DTF_Pis_PX;   //!
   TBranch        *b_Dstar_DTF_Pis_PY;   //!
   TBranch        *b_Dstar_DTF_Pis_PZ;   //!
   TBranch        *b_slowPi_MINIP;   //!
   TBranch        *b_slowPi_MINIPCHI2;   //!
   TBranch        *b_slowPi_MINIPNEXTBEST;   //!
   TBranch        *b_slowPi_MINIPCHI2NEXTBEST;   //!
   TBranch        *b_slowPi_OWNPV_X;   //!
   TBranch        *b_slowPi_OWNPV_Y;   //!
   TBranch        *b_slowPi_OWNPV_Z;   //!
   TBranch        *b_slowPi_OWNPV_XERR;   //!
   TBranch        *b_slowPi_OWNPV_YERR;   //!
   TBranch        *b_slowPi_OWNPV_ZERR;   //!
   TBranch        *b_slowPi_OWNPV_CHI2;   //!
   TBranch        *b_slowPi_OWNPV_NDOF;   //!
   TBranch        *b_slowPi_OWNPV_COV_;   //!
   TBranch        *b_slowPi_IP_OWNPV;   //!
   TBranch        *b_slowPi_IPCHI2_OWNPV;   //!
   TBranch        *b_slowPi_ORIVX_X;   //!
   TBranch        *b_slowPi_ORIVX_Y;   //!
   TBranch        *b_slowPi_ORIVX_Z;   //!
   TBranch        *b_slowPi_ORIVX_XERR;   //!
   TBranch        *b_slowPi_ORIVX_YERR;   //!
   TBranch        *b_slowPi_ORIVX_ZERR;   //!
   TBranch        *b_slowPi_ORIVX_CHI2;   //!
   TBranch        *b_slowPi_ORIVX_NDOF;   //!
   TBranch        *b_slowPi_ORIVX_COV_;   //!
   TBranch        *b_slowPi_P;   //!
   TBranch        *b_slowPi_PT;   //!
   TBranch        *b_slowPi_PE;   //!
   TBranch        *b_slowPi_PX;   //!
   TBranch        *b_slowPi_PY;   //!
   TBranch        *b_slowPi_PZ;   //!
   TBranch        *b_slowPi_M;   //!
   TBranch        *b_slowPi_ID;   //!
   TBranch        *b_slowPi_PIDe;   //!
   TBranch        *b_slowPi_PIDmu;   //!
   TBranch        *b_slowPi_PIDK;   //!
   TBranch        *b_slowPi_PIDp;   //!
   TBranch        *b_slowPi_ProbNNe;   //!
   TBranch        *b_slowPi_ProbNNk;   //!
   TBranch        *b_slowPi_ProbNNp;   //!
   TBranch        *b_slowPi_ProbNNpi;   //!
   TBranch        *b_slowPi_ProbNNmu;   //!
   TBranch        *b_slowPi_ProbNNghost;   //!
   TBranch        *b_slowPi_CaloEcalE;   //!
   TBranch        *b_slowPi_CaloHcalE;   //!
   TBranch        *b_slowPi_hasMuon;   //!
   TBranch        *b_slowPi_isMuon;   //!
   TBranch        *b_slowPi_hasRich;   //!
   TBranch        *b_slowPi_hasCalo;   //!
   TBranch        *b_slowPi_TRACK_Type;   //!
   TBranch        *b_slowPi_TRACK_Key;   //!
   TBranch        *b_slowPi_TRACK_CHI2NDOF;   //!
   TBranch        *b_slowPi_TRACK_PCHI2;   //!
   TBranch        *b_slowPi_TRACK_GhostProb;   //!
   TBranch        *b_slowPi_TRACK_CloneDist;   //!
   TBranch        *b_D0_MINIP;   //!
   TBranch        *b_D0_MINIPCHI2;   //!
   TBranch        *b_D0_MINIPNEXTBEST;   //!
   TBranch        *b_D0_MINIPCHI2NEXTBEST;   //!
   TBranch        *b_D0_OWNPV_X;   //!
   TBranch        *b_D0_OWNPV_Y;   //!
   TBranch        *b_D0_OWNPV_Z;   //!
   TBranch        *b_D0_OWNPV_XERR;   //!
   TBranch        *b_D0_OWNPV_YERR;   //!
   TBranch        *b_D0_OWNPV_ZERR;   //!
   TBranch        *b_D0_OWNPV_CHI2;   //!
   TBranch        *b_D0_OWNPV_NDOF;   //!
   TBranch        *b_D0_OWNPV_COV_;   //!
   TBranch        *b_D0_IP_OWNPV;   //!
   TBranch        *b_D0_IPCHI2_OWNPV;   //!
   TBranch        *b_D0_FD_OWNPV;   //!
   TBranch        *b_D0_FDCHI2_OWNPV;   //!
   TBranch        *b_D0_DIRA_OWNPV;   //!
   TBranch        *b_D0_ORIVX_X;   //!
   TBranch        *b_D0_ORIVX_Y;   //!
   TBranch        *b_D0_ORIVX_Z;   //!
   TBranch        *b_D0_ORIVX_XERR;   //!
   TBranch        *b_D0_ORIVX_YERR;   //!
   TBranch        *b_D0_ORIVX_ZERR;   //!
   TBranch        *b_D0_ORIVX_CHI2;   //!
   TBranch        *b_D0_ORIVX_NDOF;   //!
   TBranch        *b_D0_ORIVX_COV_;   //!
   TBranch        *b_D0_FD_ORIVX;   //!
   TBranch        *b_D0_FDCHI2_ORIVX;   //!
   TBranch        *b_D0_DIRA_ORIVX;   //!
   TBranch        *b_D0_ENDVERTEX_X;   //!
   TBranch        *b_D0_ENDVERTEX_Y;   //!
   TBranch        *b_D0_ENDVERTEX_Z;   //!
   TBranch        *b_D0_ENDVERTEX_XERR;   //!
   TBranch        *b_D0_ENDVERTEX_YERR;   //!
   TBranch        *b_D0_ENDVERTEX_ZERR;   //!
   TBranch        *b_D0_ENDVERTEX_CHI2;   //!
   TBranch        *b_D0_ENDVERTEX_NDOF;   //!
   TBranch        *b_D0_ENDVERTEX_COV_;   //!
   TBranch        *b_D0_P;   //!
   TBranch        *b_D0_PT;   //!
   TBranch        *b_D0_PE;   //!
   TBranch        *b_D0_PX;   //!
   TBranch        *b_D0_PY;   //!
   TBranch        *b_D0_PZ;   //!
   TBranch        *b_D0_MM;   //!
   TBranch        *b_D0_MMERR;   //!
   TBranch        *b_D0_M;   //!
   TBranch        *b_D0_ID;   //!
   TBranch        *b_D0_TAU;   //!
   TBranch        *b_D0_TAUERR;   //!
   TBranch        *b_D0_TAUCHI2;   //!
   TBranch        *b_D0_Stripping_nTP;   //!
   TBranch        *b_D0_Stripping_TP_RAW;   //!
   TBranch        *b_D0_Stripping_TP_DEC;   //!
   TBranch        *b_D0_Stripping_TP_TAU;   //!
   TBranch        *b_D0_Stripping_TP_IP;   //!
   TBranch        *b_D0_nTP;   //!
   TBranch        *b_D0_TP_DEC__Event_Phys_D2hhPromptD2PiPiLine;   //!
   TBranch        *b_D0_Trigger_nTP;   //!
   TBranch        *b_D0_Trigger_TP_RAW;   //!
   TBranch        *b_D0_Trigger_TP_DEC;   //!
   TBranch        *b_D0_Trigger_TP_TAU;   //!
   TBranch        *b_D0_Trigger_TP_IP;   //!
   TBranch        *b_D0_TP_DEC_Hlt1TrackAllL0Decision;   //!
   TBranch        *b_D0_TP_DEC_Hlt2CharmHadD02HH_D02PiPiDecision;   //!
   TBranch        *b_D0_TP_DEC_Hlt2CharmHadD02HH_D02PiPiWideMassDecision;   //!
   TBranch        *b_D0L0Global_Dec;   //!
   TBranch        *b_D0L0Global_TIS;   //!
   TBranch        *b_D0L0Global_TOS;   //!
   TBranch        *b_D0Hlt1Global_Dec;   //!
   TBranch        *b_D0Hlt1Global_TIS;   //!
   TBranch        *b_D0Hlt1Global_TOS;   //!
   TBranch        *b_D0Hlt2Global_Dec;   //!
   TBranch        *b_D0Hlt2Global_TIS;   //!
   TBranch        *b_D0Hlt2Global_TOS;   //!
   TBranch        *b_D0StrippingGlobal_Dec;   //!
   TBranch        *b_D0StrippingGlobal_TIS;   //!
   TBranch        *b_D0StrippingGlobal_TOS;   //!
   TBranch        *b_D0L0MuonDecision_Dec;   //!
   TBranch        *b_D0L0MuonDecision_TIS;   //!
   TBranch        *b_D0L0MuonDecision_TOS;   //!
   TBranch        *b_D0L0HadronDecision_Dec;   //!
   TBranch        *b_D0L0HadronDecision_TIS;   //!
   TBranch        *b_D0L0HadronDecision_TOS;   //!
   TBranch        *b_D0L0ElectronDecision_Dec;   //!
   TBranch        *b_D0L0ElectronDecision_TIS;   //!
   TBranch        *b_D0L0ElectronDecision_TOS;   //!
   TBranch        *b_D0L0PhotonDecision_Dec;   //!
   TBranch        *b_D0L0PhotonDecision_TIS;   //!
   TBranch        *b_D0L0PhotonDecision_TOS;   //!
   TBranch        *b_D0Hlt1TrackAllL0Decision_Dec;   //!
   TBranch        *b_D0Hlt1TrackAllL0Decision_TIS;   //!
   TBranch        *b_D0Hlt1TrackAllL0Decision_TOS;   //!
   TBranch        *b_D0Hlt2CharmHadD02HH_D02KPiDecision_Dec;   //!
   TBranch        *b_D0Hlt2CharmHadD02HH_D02KPiDecision_TIS;   //!
   TBranch        *b_D0Hlt2CharmHadD02HH_D02KPiDecision_TOS;   //!
   TBranch        *b_D0Hlt2CharmHadD02HH_D02KPiWideMassDecision_Dec;   //!
   TBranch        *b_D0Hlt2CharmHadD02HH_D02KPiWideMassDecision_TIS;   //!
   TBranch        *b_D0Hlt2CharmHadD02HH_D02KPiWideMassDecision_TOS;   //!
   TBranch        *b_D0Hlt2CharmHadD02HH_D02KKDecision_Dec;   //!
   TBranch        *b_D0Hlt2CharmHadD02HH_D02KKDecision_TIS;   //!
   TBranch        *b_D0Hlt2CharmHadD02HH_D02KKDecision_TOS;   //!
   TBranch        *b_D0Hlt2CharmHadD02HH_D02KKWideMassDecision_Dec;   //!
   TBranch        *b_D0Hlt2CharmHadD02HH_D02KKWideMassDecision_TIS;   //!
   TBranch        *b_D0Hlt2CharmHadD02HH_D02KKWideMassDecision_TOS;   //!
   TBranch        *b_D0Hlt2CharmHadD02HH_D02PiPiDecision_Dec;   //!
   TBranch        *b_D0Hlt2CharmHadD02HH_D02PiPiDecision_TIS;   //!
   TBranch        *b_D0Hlt2CharmHadD02HH_D02PiPiDecision_TOS;   //!
   TBranch        *b_D0Hlt2CharmHadD02HH_D02PiPiWideMassDecision_Dec;   //!
   TBranch        *b_D0Hlt2CharmHadD02HH_D02PiPiWideMassDecision_TIS;   //!
   TBranch        *b_D0Hlt2CharmHadD02HH_D02PiPiWideMassDecision_TOS;   //!
   TBranch        *b_dauplus_MINIP;   //!
   TBranch        *b_dauplus_MINIPCHI2;   //!
   TBranch        *b_dauplus_MINIPNEXTBEST;   //!
   TBranch        *b_dauplus_MINIPCHI2NEXTBEST;   //!
   TBranch        *b_dauplus_OWNPV_X;   //!
   TBranch        *b_dauplus_OWNPV_Y;   //!
   TBranch        *b_dauplus_OWNPV_Z;   //!
   TBranch        *b_dauplus_OWNPV_XERR;   //!
   TBranch        *b_dauplus_OWNPV_YERR;   //!
   TBranch        *b_dauplus_OWNPV_ZERR;   //!
   TBranch        *b_dauplus_OWNPV_CHI2;   //!
   TBranch        *b_dauplus_OWNPV_NDOF;   //!
   TBranch        *b_dauplus_OWNPV_COV_;   //!
   TBranch        *b_dauplus_IP_OWNPV;   //!
   TBranch        *b_dauplus_IPCHI2_OWNPV;   //!
   TBranch        *b_dauplus_ORIVX_X;   //!
   TBranch        *b_dauplus_ORIVX_Y;   //!
   TBranch        *b_dauplus_ORIVX_Z;   //!
   TBranch        *b_dauplus_ORIVX_XERR;   //!
   TBranch        *b_dauplus_ORIVX_YERR;   //!
   TBranch        *b_dauplus_ORIVX_ZERR;   //!
   TBranch        *b_dauplus_ORIVX_CHI2;   //!
   TBranch        *b_dauplus_ORIVX_NDOF;   //!
   TBranch        *b_dauplus_ORIVX_COV_;   //!
   TBranch        *b_dauplus_P;   //!
   TBranch        *b_dauplus_PT;   //!
   TBranch        *b_dauplus_PE;   //!
   TBranch        *b_dauplus_PX;   //!
   TBranch        *b_dauplus_PY;   //!
   TBranch        *b_dauplus_PZ;   //!
   TBranch        *b_dauplus_M;   //!
   TBranch        *b_dauplus_ID;   //!
   TBranch        *b_dauplus_PIDe;   //!
   TBranch        *b_dauplus_PIDmu;   //!
   TBranch        *b_dauplus_PIDK;   //!
   TBranch        *b_dauplus_PIDp;   //!
   TBranch        *b_dauplus_ProbNNe;   //!
   TBranch        *b_dauplus_ProbNNk;   //!
   TBranch        *b_dauplus_ProbNNp;   //!
   TBranch        *b_dauplus_ProbNNpi;   //!
   TBranch        *b_dauplus_ProbNNmu;   //!
   TBranch        *b_dauplus_ProbNNghost;   //!
   TBranch        *b_dauplus_CaloEcalE;   //!
   TBranch        *b_dauplus_CaloHcalE;   //!
   TBranch        *b_dauplus_hasMuon;   //!
   TBranch        *b_dauplus_isMuon;   //!
   TBranch        *b_dauplus_hasRich;   //!
   TBranch        *b_dauplus_hasCalo;   //!
   TBranch        *b_dauplus_TRACK_Type;   //!
   TBranch        *b_dauplus_TRACK_Key;   //!
   TBranch        *b_dauplus_TRACK_CHI2NDOF;   //!
   TBranch        *b_dauplus_TRACK_PCHI2;   //!
   TBranch        *b_dauplus_TRACK_GhostProb;   //!
   TBranch        *b_dauplus_TRACK_CloneDist;   //!
   TBranch        *b_dauplus_Trigger_nTP;   //!
   TBranch        *b_dauplus_Trigger_TP_DEC_trRec_HLT1;   //!
   TBranch        *b_dauplus_Trigger_TP_DEC_trRec_HLT2;   //!
   TBranch        *b_dauplus_Trigger_TP_DEC_trRec_VELO_HLT1;   //!
   TBranch        *b_dauplus_Trigger_TP_DEC_trRec_VELO_OFF;   //!
   TBranch        *b_dauplus_Trigger_TP_DEC_trTOS_Hlt1TrackAllL0Decision;   //!
   TBranch        *b_dauplus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiDecision;   //!
   TBranch        *b_dauplus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiWideMassDecision;   //!
   TBranch        *b_dauminus_MINIP;   //!
   TBranch        *b_dauminus_MINIPCHI2;   //!
   TBranch        *b_dauminus_MINIPNEXTBEST;   //!
   TBranch        *b_dauminus_MINIPCHI2NEXTBEST;   //!
   TBranch        *b_dauminus_OWNPV_X;   //!
   TBranch        *b_dauminus_OWNPV_Y;   //!
   TBranch        *b_dauminus_OWNPV_Z;   //!
   TBranch        *b_dauminus_OWNPV_XERR;   //!
   TBranch        *b_dauminus_OWNPV_YERR;   //!
   TBranch        *b_dauminus_OWNPV_ZERR;   //!
   TBranch        *b_dauminus_OWNPV_CHI2;   //!
   TBranch        *b_dauminus_OWNPV_NDOF;   //!
   TBranch        *b_dauminus_OWNPV_COV_;   //!
   TBranch        *b_dauminus_IP_OWNPV;   //!
   TBranch        *b_dauminus_IPCHI2_OWNPV;   //!
   TBranch        *b_dauminus_ORIVX_X;   //!
   TBranch        *b_dauminus_ORIVX_Y;   //!
   TBranch        *b_dauminus_ORIVX_Z;   //!
   TBranch        *b_dauminus_ORIVX_XERR;   //!
   TBranch        *b_dauminus_ORIVX_YERR;   //!
   TBranch        *b_dauminus_ORIVX_ZERR;   //!
   TBranch        *b_dauminus_ORIVX_CHI2;   //!
   TBranch        *b_dauminus_ORIVX_NDOF;   //!
   TBranch        *b_dauminus_ORIVX_COV_;   //!
   TBranch        *b_dauminus_P;   //!
   TBranch        *b_dauminus_PT;   //!
   TBranch        *b_dauminus_PE;   //!
   TBranch        *b_dauminus_PX;   //!
   TBranch        *b_dauminus_PY;   //!
   TBranch        *b_dauminus_PZ;   //!
   TBranch        *b_dauminus_M;   //!
   TBranch        *b_dauminus_ID;   //!
   TBranch        *b_dauminus_PIDe;   //!
   TBranch        *b_dauminus_PIDmu;   //!
   TBranch        *b_dauminus_PIDK;   //!
   TBranch        *b_dauminus_PIDp;   //!
   TBranch        *b_dauminus_ProbNNe;   //!
   TBranch        *b_dauminus_ProbNNk;   //!
   TBranch        *b_dauminus_ProbNNp;   //!
   TBranch        *b_dauminus_ProbNNpi;   //!
   TBranch        *b_dauminus_ProbNNmu;   //!
   TBranch        *b_dauminus_ProbNNghost;   //!
   TBranch        *b_dauminus_CaloEcalE;   //!
   TBranch        *b_dauminus_CaloHcalE;   //!
   TBranch        *b_dauminus_hasMuon;   //!
   TBranch        *b_dauminus_isMuon;   //!
   TBranch        *b_dauminus_hasRich;   //!
   TBranch        *b_dauminus_hasCalo;   //!
   TBranch        *b_dauminus_TRACK_Type;   //!
   TBranch        *b_dauminus_TRACK_Key;   //!
   TBranch        *b_dauminus_TRACK_CHI2NDOF;   //!
   TBranch        *b_dauminus_TRACK_PCHI2;   //!
   TBranch        *b_dauminus_TRACK_GhostProb;   //!
   TBranch        *b_dauminus_TRACK_CloneDist;   //!
   TBranch        *b_dauminus_Trigger_nTP;   //!
   TBranch        *b_dauminus_Trigger_TP_DEC_trRec_HLT1;   //!
   TBranch        *b_dauminus_Trigger_TP_DEC_trRec_HLT2;   //!
   TBranch        *b_dauminus_Trigger_TP_DEC_trRec_VELO_HLT1;   //!
   TBranch        *b_dauminus_Trigger_TP_DEC_trRec_VELO_OFF;   //!
   TBranch        *b_dauminus_Trigger_TP_DEC_trTOS_Hlt1TrackAllL0Decision;   //!
   TBranch        *b_dauminus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiDecision;   //!
   TBranch        *b_dauminus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiWideMassDecision;   //!
   TBranch        *b_nCandidate;   //!
   TBranch        *b_totCandidates;   //!
   TBranch        *b_EventInSequence;   //!
   TBranch        *b_runNumber;   //!
   TBranch        *b_eventNumber;   //!
   TBranch        *b_BCID;   //!
   TBranch        *b_BCType;   //!
   TBranch        *b_OdinTCK;   //!
   TBranch        *b_L0DUTCK;   //!
   TBranch        *b_HLTTCK;   //!
   TBranch        *b_GpsTime;   //!
   TBranch        *b_Primaries;   //!
   TBranch        *b_Polarity;   //!
   TBranch        *b_nPV;   //!
   TBranch        *b_PVX;   //!
   TBranch        *b_PVY;   //!
   TBranch        *b_PVZ;   //!
   TBranch        *b_PVXERR;   //!
   TBranch        *b_PVYERR;   //!
   TBranch        *b_PVZERR;   //!
   TBranch        *b_PVCHI2;   //!
   TBranch        *b_PVNDOF;   //!
   TBranch        *b_PVNTRACKS;   //!

   CorrectVELOSwimInfo_D02PiPi(std::string inputfile, std::string tupledir, std::string tuplename, TTree *tree=0);
   virtual ~CorrectVELOSwimInfo_D02PiPi();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop(std::string outputfile);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
   std::string m_tuplename;
   std::string m_tupledir; 
   std::string m_inputfile;  
};

#endif

#ifdef CorrectVELOSwimInfo_D02PiPi_cxx
CorrectVELOSwimInfo_D02PiPi::CorrectVELOSwimInfo_D02PiPi(std::string inputfile, std::string tupledir, std::string tuplename, TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   m_inputfile = inputfile;
   m_tupledir = tupledir;
   m_tuplename = tuplename; 
   if (tree == 0) {
      TFile *f = TFile::Open(m_inputfile.c_str());
      tree = (TTree*) f->Get(m_tupledir.c_str());
   }
   Init(tree);
}

CorrectVELOSwimInfo_D02PiPi::~CorrectVELOSwimInfo_D02PiPi()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t CorrectVELOSwimInfo_D02PiPi::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t CorrectVELOSwimInfo_D02PiPi::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void CorrectVELOSwimInfo_D02PiPi::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("Dstar_MINIP", &Dstar_MINIP, &b_Dstar_MINIP);
   fChain->SetBranchAddress("Dstar_MINIPCHI2", &Dstar_MINIPCHI2, &b_Dstar_MINIPCHI2);
   fChain->SetBranchAddress("Dstar_MINIPNEXTBEST", &Dstar_MINIPNEXTBEST, &b_Dstar_MINIPNEXTBEST);
   fChain->SetBranchAddress("Dstar_MINIPCHI2NEXTBEST", &Dstar_MINIPCHI2NEXTBEST, &b_Dstar_MINIPCHI2NEXTBEST);
   fChain->SetBranchAddress("Dstar_OWNPV_X", &Dstar_OWNPV_X, &b_Dstar_OWNPV_X);
   fChain->SetBranchAddress("Dstar_OWNPV_Y", &Dstar_OWNPV_Y, &b_Dstar_OWNPV_Y);
   fChain->SetBranchAddress("Dstar_OWNPV_Z", &Dstar_OWNPV_Z, &b_Dstar_OWNPV_Z);
   fChain->SetBranchAddress("Dstar_OWNPV_XERR", &Dstar_OWNPV_XERR, &b_Dstar_OWNPV_XERR);
   fChain->SetBranchAddress("Dstar_OWNPV_YERR", &Dstar_OWNPV_YERR, &b_Dstar_OWNPV_YERR);
   fChain->SetBranchAddress("Dstar_OWNPV_ZERR", &Dstar_OWNPV_ZERR, &b_Dstar_OWNPV_ZERR);
   fChain->SetBranchAddress("Dstar_OWNPV_CHI2", &Dstar_OWNPV_CHI2, &b_Dstar_OWNPV_CHI2);
   fChain->SetBranchAddress("Dstar_OWNPV_NDOF", &Dstar_OWNPV_NDOF, &b_Dstar_OWNPV_NDOF);
   fChain->SetBranchAddress("Dstar_OWNPV_COV_", Dstar_OWNPV_COV_, &b_Dstar_OWNPV_COV_);
   fChain->SetBranchAddress("Dstar_IP_OWNPV", &Dstar_IP_OWNPV, &b_Dstar_IP_OWNPV);
   fChain->SetBranchAddress("Dstar_IPCHI2_OWNPV", &Dstar_IPCHI2_OWNPV, &b_Dstar_IPCHI2_OWNPV);
   fChain->SetBranchAddress("Dstar_FD_OWNPV", &Dstar_FD_OWNPV, &b_Dstar_FD_OWNPV);
   fChain->SetBranchAddress("Dstar_FDCHI2_OWNPV", &Dstar_FDCHI2_OWNPV, &b_Dstar_FDCHI2_OWNPV);
   fChain->SetBranchAddress("Dstar_DIRA_OWNPV", &Dstar_DIRA_OWNPV, &b_Dstar_DIRA_OWNPV);
   fChain->SetBranchAddress("Dstar_ENDVERTEX_X", &Dstar_ENDVERTEX_X, &b_Dstar_ENDVERTEX_X);
   fChain->SetBranchAddress("Dstar_ENDVERTEX_Y", &Dstar_ENDVERTEX_Y, &b_Dstar_ENDVERTEX_Y);
   fChain->SetBranchAddress("Dstar_ENDVERTEX_Z", &Dstar_ENDVERTEX_Z, &b_Dstar_ENDVERTEX_Z);
   fChain->SetBranchAddress("Dstar_ENDVERTEX_XERR", &Dstar_ENDVERTEX_XERR, &b_Dstar_ENDVERTEX_XERR);
   fChain->SetBranchAddress("Dstar_ENDVERTEX_YERR", &Dstar_ENDVERTEX_YERR, &b_Dstar_ENDVERTEX_YERR);
   fChain->SetBranchAddress("Dstar_ENDVERTEX_ZERR", &Dstar_ENDVERTEX_ZERR, &b_Dstar_ENDVERTEX_ZERR);
   fChain->SetBranchAddress("Dstar_ENDVERTEX_CHI2", &Dstar_ENDVERTEX_CHI2, &b_Dstar_ENDVERTEX_CHI2);
   fChain->SetBranchAddress("Dstar_ENDVERTEX_NDOF", &Dstar_ENDVERTEX_NDOF, &b_Dstar_ENDVERTEX_NDOF);
   fChain->SetBranchAddress("Dstar_ENDVERTEX_COV_", Dstar_ENDVERTEX_COV_, &b_Dstar_ENDVERTEX_COV_);
   fChain->SetBranchAddress("Dstar_P", &Dstar_P, &b_Dstar_P);
   fChain->SetBranchAddress("Dstar_PT", &Dstar_PT, &b_Dstar_PT);
   fChain->SetBranchAddress("Dstar_PE", &Dstar_PE, &b_Dstar_PE);
   fChain->SetBranchAddress("Dstar_PX", &Dstar_PX, &b_Dstar_PX);
   fChain->SetBranchAddress("Dstar_PY", &Dstar_PY, &b_Dstar_PY);
   fChain->SetBranchAddress("Dstar_PZ", &Dstar_PZ, &b_Dstar_PZ);
   fChain->SetBranchAddress("Dstar_MM", &Dstar_MM, &b_Dstar_MM);
   fChain->SetBranchAddress("Dstar_MMERR", &Dstar_MMERR, &b_Dstar_MMERR);
   fChain->SetBranchAddress("Dstar_M", &Dstar_M, &b_Dstar_M);
   fChain->SetBranchAddress("Dstar_ID", &Dstar_ID, &b_Dstar_ID);
   fChain->SetBranchAddress("Dstar_DTF_CHI2", &Dstar_DTF_CHI2, &b_Dstar_DTF_CHI2);
   fChain->SetBranchAddress("Dstar_DTF_D0_BPVIPCHI2", &Dstar_DTF_D0_BPVIPCHI2, &b_Dstar_DTF_D0_BPVIPCHI2);
   fChain->SetBranchAddress("Dstar_DTF_D0_E", &Dstar_DTF_D0_E, &b_Dstar_DTF_D0_E);
   fChain->SetBranchAddress("Dstar_DTF_D0_M", &Dstar_DTF_D0_M, &b_Dstar_DTF_D0_M);
   fChain->SetBranchAddress("Dstar_DTF_D0_P", &Dstar_DTF_D0_P, &b_Dstar_DTF_D0_P);
   fChain->SetBranchAddress("Dstar_DTF_D0_PT", &Dstar_DTF_D0_PT, &b_Dstar_DTF_D0_PT);
   fChain->SetBranchAddress("Dstar_DTF_D0_PX", &Dstar_DTF_D0_PX, &b_Dstar_DTF_D0_PX);
   fChain->SetBranchAddress("Dstar_DTF_D0_PY", &Dstar_DTF_D0_PY, &b_Dstar_DTF_D0_PY);
   fChain->SetBranchAddress("Dstar_DTF_D0_PZ", &Dstar_DTF_D0_PZ, &b_Dstar_DTF_D0_PZ);
   fChain->SetBranchAddress("Dstar_DTF_Dstar_E", &Dstar_DTF_Dstar_E, &b_Dstar_DTF_Dstar_E);
   fChain->SetBranchAddress("Dstar_DTF_Dstar_M", &Dstar_DTF_Dstar_M, &b_Dstar_DTF_Dstar_M);
   fChain->SetBranchAddress("Dstar_DTF_Dstar_P", &Dstar_DTF_Dstar_P, &b_Dstar_DTF_Dstar_P);
   fChain->SetBranchAddress("Dstar_DTF_Dstar_PT", &Dstar_DTF_Dstar_PT, &b_Dstar_DTF_Dstar_PT);
   fChain->SetBranchAddress("Dstar_DTF_Dstar_PX", &Dstar_DTF_Dstar_PX, &b_Dstar_DTF_Dstar_PX);
   fChain->SetBranchAddress("Dstar_DTF_Dstar_PY", &Dstar_DTF_Dstar_PY, &b_Dstar_DTF_Dstar_PY);
   fChain->SetBranchAddress("Dstar_DTF_Dstar_PZ", &Dstar_DTF_Dstar_PZ, &b_Dstar_DTF_Dstar_PZ);
   fChain->SetBranchAddress("Dstar_DTF_NDOF", &Dstar_DTF_NDOF, &b_Dstar_DTF_NDOF);
   fChain->SetBranchAddress("Dstar_DTF_Pis_BPVIPCHI2", &Dstar_DTF_Pis_BPVIPCHI2, &b_Dstar_DTF_Pis_BPVIPCHI2);
   fChain->SetBranchAddress("Dstar_DTF_Pis_E", &Dstar_DTF_Pis_E, &b_Dstar_DTF_Pis_E);
   fChain->SetBranchAddress("Dstar_DTF_Pis_M", &Dstar_DTF_Pis_M, &b_Dstar_DTF_Pis_M);
   fChain->SetBranchAddress("Dstar_DTF_Pis_P", &Dstar_DTF_Pis_P, &b_Dstar_DTF_Pis_P);
   fChain->SetBranchAddress("Dstar_DTF_Pis_PT", &Dstar_DTF_Pis_PT, &b_Dstar_DTF_Pis_PT);
   fChain->SetBranchAddress("Dstar_DTF_Pis_PX", &Dstar_DTF_Pis_PX, &b_Dstar_DTF_Pis_PX);
   fChain->SetBranchAddress("Dstar_DTF_Pis_PY", &Dstar_DTF_Pis_PY, &b_Dstar_DTF_Pis_PY);
   fChain->SetBranchAddress("Dstar_DTF_Pis_PZ", &Dstar_DTF_Pis_PZ, &b_Dstar_DTF_Pis_PZ);
   fChain->SetBranchAddress("slowPi_MINIP", &slowPi_MINIP, &b_slowPi_MINIP);
   fChain->SetBranchAddress("slowPi_MINIPCHI2", &slowPi_MINIPCHI2, &b_slowPi_MINIPCHI2);
   fChain->SetBranchAddress("slowPi_MINIPNEXTBEST", &slowPi_MINIPNEXTBEST, &b_slowPi_MINIPNEXTBEST);
   fChain->SetBranchAddress("slowPi_MINIPCHI2NEXTBEST", &slowPi_MINIPCHI2NEXTBEST, &b_slowPi_MINIPCHI2NEXTBEST);
   fChain->SetBranchAddress("slowPi_OWNPV_X", &slowPi_OWNPV_X, &b_slowPi_OWNPV_X);
   fChain->SetBranchAddress("slowPi_OWNPV_Y", &slowPi_OWNPV_Y, &b_slowPi_OWNPV_Y);
   fChain->SetBranchAddress("slowPi_OWNPV_Z", &slowPi_OWNPV_Z, &b_slowPi_OWNPV_Z);
   fChain->SetBranchAddress("slowPi_OWNPV_XERR", &slowPi_OWNPV_XERR, &b_slowPi_OWNPV_XERR);
   fChain->SetBranchAddress("slowPi_OWNPV_YERR", &slowPi_OWNPV_YERR, &b_slowPi_OWNPV_YERR);
   fChain->SetBranchAddress("slowPi_OWNPV_ZERR", &slowPi_OWNPV_ZERR, &b_slowPi_OWNPV_ZERR);
   fChain->SetBranchAddress("slowPi_OWNPV_CHI2", &slowPi_OWNPV_CHI2, &b_slowPi_OWNPV_CHI2);
   fChain->SetBranchAddress("slowPi_OWNPV_NDOF", &slowPi_OWNPV_NDOF, &b_slowPi_OWNPV_NDOF);
   fChain->SetBranchAddress("slowPi_OWNPV_COV_", slowPi_OWNPV_COV_, &b_slowPi_OWNPV_COV_);
   fChain->SetBranchAddress("slowPi_IP_OWNPV", &slowPi_IP_OWNPV, &b_slowPi_IP_OWNPV);
   fChain->SetBranchAddress("slowPi_IPCHI2_OWNPV", &slowPi_IPCHI2_OWNPV, &b_slowPi_IPCHI2_OWNPV);
   fChain->SetBranchAddress("slowPi_ORIVX_X", &slowPi_ORIVX_X, &b_slowPi_ORIVX_X);
   fChain->SetBranchAddress("slowPi_ORIVX_Y", &slowPi_ORIVX_Y, &b_slowPi_ORIVX_Y);
   fChain->SetBranchAddress("slowPi_ORIVX_Z", &slowPi_ORIVX_Z, &b_slowPi_ORIVX_Z);
   fChain->SetBranchAddress("slowPi_ORIVX_XERR", &slowPi_ORIVX_XERR, &b_slowPi_ORIVX_XERR);
   fChain->SetBranchAddress("slowPi_ORIVX_YERR", &slowPi_ORIVX_YERR, &b_slowPi_ORIVX_YERR);
   fChain->SetBranchAddress("slowPi_ORIVX_ZERR", &slowPi_ORIVX_ZERR, &b_slowPi_ORIVX_ZERR);
   fChain->SetBranchAddress("slowPi_ORIVX_CHI2", &slowPi_ORIVX_CHI2, &b_slowPi_ORIVX_CHI2);
   fChain->SetBranchAddress("slowPi_ORIVX_NDOF", &slowPi_ORIVX_NDOF, &b_slowPi_ORIVX_NDOF);
   fChain->SetBranchAddress("slowPi_ORIVX_COV_", slowPi_ORIVX_COV_, &b_slowPi_ORIVX_COV_);
   fChain->SetBranchAddress("slowPi_P", &slowPi_P, &b_slowPi_P);
   fChain->SetBranchAddress("slowPi_PT", &slowPi_PT, &b_slowPi_PT);
   fChain->SetBranchAddress("slowPi_PE", &slowPi_PE, &b_slowPi_PE);
   fChain->SetBranchAddress("slowPi_PX", &slowPi_PX, &b_slowPi_PX);
   fChain->SetBranchAddress("slowPi_PY", &slowPi_PY, &b_slowPi_PY);
   fChain->SetBranchAddress("slowPi_PZ", &slowPi_PZ, &b_slowPi_PZ);
   fChain->SetBranchAddress("slowPi_M", &slowPi_M, &b_slowPi_M);
   fChain->SetBranchAddress("slowPi_ID", &slowPi_ID, &b_slowPi_ID);
   fChain->SetBranchAddress("slowPi_PIDe", &slowPi_PIDe, &b_slowPi_PIDe);
   fChain->SetBranchAddress("slowPi_PIDmu", &slowPi_PIDmu, &b_slowPi_PIDmu);
   fChain->SetBranchAddress("slowPi_PIDK", &slowPi_PIDK, &b_slowPi_PIDK);
   fChain->SetBranchAddress("slowPi_PIDp", &slowPi_PIDp, &b_slowPi_PIDp);
   fChain->SetBranchAddress("slowPi_ProbNNe", &slowPi_ProbNNe, &b_slowPi_ProbNNe);
   fChain->SetBranchAddress("slowPi_ProbNNk", &slowPi_ProbNNk, &b_slowPi_ProbNNk);
   fChain->SetBranchAddress("slowPi_ProbNNp", &slowPi_ProbNNp, &b_slowPi_ProbNNp);
   fChain->SetBranchAddress("slowPi_ProbNNpi", &slowPi_ProbNNpi, &b_slowPi_ProbNNpi);
   fChain->SetBranchAddress("slowPi_ProbNNmu", &slowPi_ProbNNmu, &b_slowPi_ProbNNmu);
   fChain->SetBranchAddress("slowPi_ProbNNghost", &slowPi_ProbNNghost, &b_slowPi_ProbNNghost);
   fChain->SetBranchAddress("slowPi_CaloEcalE", &slowPi_CaloEcalE, &b_slowPi_CaloEcalE);
   fChain->SetBranchAddress("slowPi_CaloHcalE", &slowPi_CaloHcalE, &b_slowPi_CaloHcalE);
   fChain->SetBranchAddress("slowPi_hasMuon", &slowPi_hasMuon, &b_slowPi_hasMuon);
   fChain->SetBranchAddress("slowPi_isMuon", &slowPi_isMuon, &b_slowPi_isMuon);
   fChain->SetBranchAddress("slowPi_hasRich", &slowPi_hasRich, &b_slowPi_hasRich);
   fChain->SetBranchAddress("slowPi_hasCalo", &slowPi_hasCalo, &b_slowPi_hasCalo);
   fChain->SetBranchAddress("slowPi_TRACK_Type", &slowPi_TRACK_Type, &b_slowPi_TRACK_Type);
   fChain->SetBranchAddress("slowPi_TRACK_Key", &slowPi_TRACK_Key, &b_slowPi_TRACK_Key);
   fChain->SetBranchAddress("slowPi_TRACK_CHI2NDOF", &slowPi_TRACK_CHI2NDOF, &b_slowPi_TRACK_CHI2NDOF);
   fChain->SetBranchAddress("slowPi_TRACK_PCHI2", &slowPi_TRACK_PCHI2, &b_slowPi_TRACK_PCHI2);
   fChain->SetBranchAddress("slowPi_TRACK_GhostProb", &slowPi_TRACK_GhostProb, &b_slowPi_TRACK_GhostProb);
   fChain->SetBranchAddress("slowPi_TRACK_CloneDist", &slowPi_TRACK_CloneDist, &b_slowPi_TRACK_CloneDist);
   fChain->SetBranchAddress("D0_MINIP", &D0_MINIP, &b_D0_MINIP);
   fChain->SetBranchAddress("D0_MINIPCHI2", &D0_MINIPCHI2, &b_D0_MINIPCHI2);
   fChain->SetBranchAddress("D0_MINIPNEXTBEST", &D0_MINIPNEXTBEST, &b_D0_MINIPNEXTBEST);
   fChain->SetBranchAddress("D0_MINIPCHI2NEXTBEST", &D0_MINIPCHI2NEXTBEST, &b_D0_MINIPCHI2NEXTBEST);
   fChain->SetBranchAddress("D0_OWNPV_X", &D0_OWNPV_X, &b_D0_OWNPV_X);
   fChain->SetBranchAddress("D0_OWNPV_Y", &D0_OWNPV_Y, &b_D0_OWNPV_Y);
   fChain->SetBranchAddress("D0_OWNPV_Z", &D0_OWNPV_Z, &b_D0_OWNPV_Z);
   fChain->SetBranchAddress("D0_OWNPV_XERR", &D0_OWNPV_XERR, &b_D0_OWNPV_XERR);
   fChain->SetBranchAddress("D0_OWNPV_YERR", &D0_OWNPV_YERR, &b_D0_OWNPV_YERR);
   fChain->SetBranchAddress("D0_OWNPV_ZERR", &D0_OWNPV_ZERR, &b_D0_OWNPV_ZERR);
   fChain->SetBranchAddress("D0_OWNPV_CHI2", &D0_OWNPV_CHI2, &b_D0_OWNPV_CHI2);
   fChain->SetBranchAddress("D0_OWNPV_NDOF", &D0_OWNPV_NDOF, &b_D0_OWNPV_NDOF);
   fChain->SetBranchAddress("D0_OWNPV_COV_", D0_OWNPV_COV_, &b_D0_OWNPV_COV_);
   fChain->SetBranchAddress("D0_IP_OWNPV", &D0_IP_OWNPV, &b_D0_IP_OWNPV);
   fChain->SetBranchAddress("D0_IPCHI2_OWNPV", &D0_IPCHI2_OWNPV, &b_D0_IPCHI2_OWNPV);
   fChain->SetBranchAddress("D0_FD_OWNPV", &D0_FD_OWNPV, &b_D0_FD_OWNPV);
   fChain->SetBranchAddress("D0_FDCHI2_OWNPV", &D0_FDCHI2_OWNPV, &b_D0_FDCHI2_OWNPV);
   fChain->SetBranchAddress("D0_DIRA_OWNPV", &D0_DIRA_OWNPV, &b_D0_DIRA_OWNPV);
   fChain->SetBranchAddress("D0_ORIVX_X", &D0_ORIVX_X, &b_D0_ORIVX_X);
   fChain->SetBranchAddress("D0_ORIVX_Y", &D0_ORIVX_Y, &b_D0_ORIVX_Y);
   fChain->SetBranchAddress("D0_ORIVX_Z", &D0_ORIVX_Z, &b_D0_ORIVX_Z);
   fChain->SetBranchAddress("D0_ORIVX_XERR", &D0_ORIVX_XERR, &b_D0_ORIVX_XERR);
   fChain->SetBranchAddress("D0_ORIVX_YERR", &D0_ORIVX_YERR, &b_D0_ORIVX_YERR);
   fChain->SetBranchAddress("D0_ORIVX_ZERR", &D0_ORIVX_ZERR, &b_D0_ORIVX_ZERR);
   fChain->SetBranchAddress("D0_ORIVX_CHI2", &D0_ORIVX_CHI2, &b_D0_ORIVX_CHI2);
   fChain->SetBranchAddress("D0_ORIVX_NDOF", &D0_ORIVX_NDOF, &b_D0_ORIVX_NDOF);
   fChain->SetBranchAddress("D0_ORIVX_COV_", D0_ORIVX_COV_, &b_D0_ORIVX_COV_);
   fChain->SetBranchAddress("D0_FD_ORIVX", &D0_FD_ORIVX, &b_D0_FD_ORIVX);
   fChain->SetBranchAddress("D0_FDCHI2_ORIVX", &D0_FDCHI2_ORIVX, &b_D0_FDCHI2_ORIVX);
   fChain->SetBranchAddress("D0_DIRA_ORIVX", &D0_DIRA_ORIVX, &b_D0_DIRA_ORIVX);
   fChain->SetBranchAddress("D0_ENDVERTEX_X", &D0_ENDVERTEX_X, &b_D0_ENDVERTEX_X);
   fChain->SetBranchAddress("D0_ENDVERTEX_Y", &D0_ENDVERTEX_Y, &b_D0_ENDVERTEX_Y);
   fChain->SetBranchAddress("D0_ENDVERTEX_Z", &D0_ENDVERTEX_Z, &b_D0_ENDVERTEX_Z);
   fChain->SetBranchAddress("D0_ENDVERTEX_XERR", &D0_ENDVERTEX_XERR, &b_D0_ENDVERTEX_XERR);
   fChain->SetBranchAddress("D0_ENDVERTEX_YERR", &D0_ENDVERTEX_YERR, &b_D0_ENDVERTEX_YERR);
   fChain->SetBranchAddress("D0_ENDVERTEX_ZERR", &D0_ENDVERTEX_ZERR, &b_D0_ENDVERTEX_ZERR);
   fChain->SetBranchAddress("D0_ENDVERTEX_CHI2", &D0_ENDVERTEX_CHI2, &b_D0_ENDVERTEX_CHI2);
   fChain->SetBranchAddress("D0_ENDVERTEX_NDOF", &D0_ENDVERTEX_NDOF, &b_D0_ENDVERTEX_NDOF);
   fChain->SetBranchAddress("D0_ENDVERTEX_COV_", D0_ENDVERTEX_COV_, &b_D0_ENDVERTEX_COV_);
   fChain->SetBranchAddress("D0_P", &D0_P, &b_D0_P);
   fChain->SetBranchAddress("D0_PT", &D0_PT, &b_D0_PT);
   fChain->SetBranchAddress("D0_PE", &D0_PE, &b_D0_PE);
   fChain->SetBranchAddress("D0_PX", &D0_PX, &b_D0_PX);
   fChain->SetBranchAddress("D0_PY", &D0_PY, &b_D0_PY);
   fChain->SetBranchAddress("D0_PZ", &D0_PZ, &b_D0_PZ);
   fChain->SetBranchAddress("D0_MM", &D0_MM, &b_D0_MM);
   fChain->SetBranchAddress("D0_MMERR", &D0_MMERR, &b_D0_MMERR);
   fChain->SetBranchAddress("D0_M", &D0_M, &b_D0_M);
   fChain->SetBranchAddress("D0_ID", &D0_ID, &b_D0_ID);
   fChain->SetBranchAddress("D0_TAU", &D0_TAU, &b_D0_TAU);
   fChain->SetBranchAddress("D0_TAUERR", &D0_TAUERR, &b_D0_TAUERR);
   fChain->SetBranchAddress("D0_TAUCHI2", &D0_TAUCHI2, &b_D0_TAUCHI2);
   fChain->SetBranchAddress("D0_Stripping_nTP", &D0_Stripping_nTP, &b_D0_Stripping_nTP);
   fChain->SetBranchAddress("D0_Stripping_TP_RAW", D0_Stripping_TP_RAW, &b_D0_Stripping_TP_RAW);
   fChain->SetBranchAddress("D0_Stripping_TP_DEC", D0_Stripping_TP_DEC, &b_D0_Stripping_TP_DEC);
   fChain->SetBranchAddress("D0_Stripping_TP_TAU", D0_Stripping_TP_TAU, &b_D0_Stripping_TP_TAU);
   fChain->SetBranchAddress("D0_Stripping_TP_IP", D0_Stripping_TP_IP, &b_D0_Stripping_TP_IP);
   fChain->SetBranchAddress("D0_nTP", &D0_nTP, &b_D0_nTP);
   fChain->SetBranchAddress("D0_TP_DEC__Event_Phys_D2hhPromptD2PiPiLine", D0_TP_DEC__Event_Phys_D2hhPromptD2PiPiLine, &b_D0_TP_DEC__Event_Phys_D2hhPromptD2PiPiLine);
   fChain->SetBranchAddress("D0_Trigger_nTP", &D0_Trigger_nTP, &b_D0_Trigger_nTP);
   fChain->SetBranchAddress("D0_Trigger_TP_RAW", D0_Trigger_TP_RAW, &b_D0_Trigger_TP_RAW);
   fChain->SetBranchAddress("D0_Trigger_TP_DEC", D0_Trigger_TP_DEC, &b_D0_Trigger_TP_DEC);
   fChain->SetBranchAddress("D0_Trigger_TP_TAU", D0_Trigger_TP_TAU, &b_D0_Trigger_TP_TAU);
   fChain->SetBranchAddress("D0_Trigger_TP_IP", D0_Trigger_TP_IP, &b_D0_Trigger_TP_IP);
   fChain->SetBranchAddress("D0_TP_DEC_Hlt1TrackAllL0Decision", D0_TP_DEC_Hlt1TrackAllL0Decision, &b_D0_TP_DEC_Hlt1TrackAllL0Decision);
   fChain->SetBranchAddress("D0_TP_DEC_Hlt2CharmHadD02HH_D02PiPiDecision", D0_TP_DEC_Hlt2CharmHadD02HH_D02PiPiDecision, &b_D0_TP_DEC_Hlt2CharmHadD02HH_D02PiPiDecision);
   fChain->SetBranchAddress("D0_TP_DEC_Hlt2CharmHadD02HH_D02PiPiWideMassDecision", D0_TP_DEC_Hlt2CharmHadD02HH_D02PiPiWideMassDecision, &b_D0_TP_DEC_Hlt2CharmHadD02HH_D02PiPiWideMassDecision);
   fChain->SetBranchAddress("D0L0Global_Dec", &D0L0Global_Dec, &b_D0L0Global_Dec);
   fChain->SetBranchAddress("D0L0Global_TIS", &D0L0Global_TIS, &b_D0L0Global_TIS);
   fChain->SetBranchAddress("D0L0Global_TOS", &D0L0Global_TOS, &b_D0L0Global_TOS);
   fChain->SetBranchAddress("D0Hlt1Global_Dec", &D0Hlt1Global_Dec, &b_D0Hlt1Global_Dec);
   fChain->SetBranchAddress("D0Hlt1Global_TIS", &D0Hlt1Global_TIS, &b_D0Hlt1Global_TIS);
   fChain->SetBranchAddress("D0Hlt1Global_TOS", &D0Hlt1Global_TOS, &b_D0Hlt1Global_TOS);
   fChain->SetBranchAddress("D0Hlt2Global_Dec", &D0Hlt2Global_Dec, &b_D0Hlt2Global_Dec);
   fChain->SetBranchAddress("D0Hlt2Global_TIS", &D0Hlt2Global_TIS, &b_D0Hlt2Global_TIS);
   fChain->SetBranchAddress("D0Hlt2Global_TOS", &D0Hlt2Global_TOS, &b_D0Hlt2Global_TOS);
   fChain->SetBranchAddress("D0StrippingGlobal_Dec", &D0StrippingGlobal_Dec, &b_D0StrippingGlobal_Dec);
   fChain->SetBranchAddress("D0StrippingGlobal_TIS", &D0StrippingGlobal_TIS, &b_D0StrippingGlobal_TIS);
   fChain->SetBranchAddress("D0StrippingGlobal_TOS", &D0StrippingGlobal_TOS, &b_D0StrippingGlobal_TOS);
   fChain->SetBranchAddress("D0L0MuonDecision_Dec", &D0L0MuonDecision_Dec, &b_D0L0MuonDecision_Dec);
   fChain->SetBranchAddress("D0L0MuonDecision_TIS", &D0L0MuonDecision_TIS, &b_D0L0MuonDecision_TIS);
   fChain->SetBranchAddress("D0L0MuonDecision_TOS", &D0L0MuonDecision_TOS, &b_D0L0MuonDecision_TOS);
   fChain->SetBranchAddress("D0L0HadronDecision_Dec", &D0L0HadronDecision_Dec, &b_D0L0HadronDecision_Dec);
   fChain->SetBranchAddress("D0L0HadronDecision_TIS", &D0L0HadronDecision_TIS, &b_D0L0HadronDecision_TIS);
   fChain->SetBranchAddress("D0L0HadronDecision_TOS", &D0L0HadronDecision_TOS, &b_D0L0HadronDecision_TOS);
   fChain->SetBranchAddress("D0L0ElectronDecision_Dec", &D0L0ElectronDecision_Dec, &b_D0L0ElectronDecision_Dec);
   fChain->SetBranchAddress("D0L0ElectronDecision_TIS", &D0L0ElectronDecision_TIS, &b_D0L0ElectronDecision_TIS);
   fChain->SetBranchAddress("D0L0ElectronDecision_TOS", &D0L0ElectronDecision_TOS, &b_D0L0ElectronDecision_TOS);
   fChain->SetBranchAddress("D0L0PhotonDecision_Dec", &D0L0PhotonDecision_Dec, &b_D0L0PhotonDecision_Dec);
   fChain->SetBranchAddress("D0L0PhotonDecision_TIS", &D0L0PhotonDecision_TIS, &b_D0L0PhotonDecision_TIS);
   fChain->SetBranchAddress("D0L0PhotonDecision_TOS", &D0L0PhotonDecision_TOS, &b_D0L0PhotonDecision_TOS);
   fChain->SetBranchAddress("D0Hlt1TrackAllL0Decision_Dec", &D0Hlt1TrackAllL0Decision_Dec, &b_D0Hlt1TrackAllL0Decision_Dec);
   fChain->SetBranchAddress("D0Hlt1TrackAllL0Decision_TIS", &D0Hlt1TrackAllL0Decision_TIS, &b_D0Hlt1TrackAllL0Decision_TIS);
   fChain->SetBranchAddress("D0Hlt1TrackAllL0Decision_TOS", &D0Hlt1TrackAllL0Decision_TOS, &b_D0Hlt1TrackAllL0Decision_TOS);
   fChain->SetBranchAddress("D0Hlt2CharmHadD02HH_D02KPiDecision_Dec", &D0Hlt2CharmHadD02HH_D02KPiDecision_Dec, &b_D0Hlt2CharmHadD02HH_D02KPiDecision_Dec);
   fChain->SetBranchAddress("D0Hlt2CharmHadD02HH_D02KPiDecision_TIS", &D0Hlt2CharmHadD02HH_D02KPiDecision_TIS, &b_D0Hlt2CharmHadD02HH_D02KPiDecision_TIS);
   fChain->SetBranchAddress("D0Hlt2CharmHadD02HH_D02KPiDecision_TOS", &D0Hlt2CharmHadD02HH_D02KPiDecision_TOS, &b_D0Hlt2CharmHadD02HH_D02KPiDecision_TOS);
   fChain->SetBranchAddress("D0Hlt2CharmHadD02HH_D02KPiWideMassDecision_Dec", &D0Hlt2CharmHadD02HH_D02KPiWideMassDecision_Dec, &b_D0Hlt2CharmHadD02HH_D02KPiWideMassDecision_Dec);
   fChain->SetBranchAddress("D0Hlt2CharmHadD02HH_D02KPiWideMassDecision_TIS", &D0Hlt2CharmHadD02HH_D02KPiWideMassDecision_TIS, &b_D0Hlt2CharmHadD02HH_D02KPiWideMassDecision_TIS);
   fChain->SetBranchAddress("D0Hlt2CharmHadD02HH_D02KPiWideMassDecision_TOS", &D0Hlt2CharmHadD02HH_D02KPiWideMassDecision_TOS, &b_D0Hlt2CharmHadD02HH_D02KPiWideMassDecision_TOS);
   fChain->SetBranchAddress("D0Hlt2CharmHadD02HH_D02KKDecision_Dec", &D0Hlt2CharmHadD02HH_D02KKDecision_Dec, &b_D0Hlt2CharmHadD02HH_D02KKDecision_Dec);
   fChain->SetBranchAddress("D0Hlt2CharmHadD02HH_D02KKDecision_TIS", &D0Hlt2CharmHadD02HH_D02KKDecision_TIS, &b_D0Hlt2CharmHadD02HH_D02KKDecision_TIS);
   fChain->SetBranchAddress("D0Hlt2CharmHadD02HH_D02KKDecision_TOS", &D0Hlt2CharmHadD02HH_D02KKDecision_TOS, &b_D0Hlt2CharmHadD02HH_D02KKDecision_TOS);
   fChain->SetBranchAddress("D0Hlt2CharmHadD02HH_D02KKWideMassDecision_Dec", &D0Hlt2CharmHadD02HH_D02KKWideMassDecision_Dec, &b_D0Hlt2CharmHadD02HH_D02KKWideMassDecision_Dec);
   fChain->SetBranchAddress("D0Hlt2CharmHadD02HH_D02KKWideMassDecision_TIS", &D0Hlt2CharmHadD02HH_D02KKWideMassDecision_TIS, &b_D0Hlt2CharmHadD02HH_D02KKWideMassDecision_TIS);
   fChain->SetBranchAddress("D0Hlt2CharmHadD02HH_D02KKWideMassDecision_TOS", &D0Hlt2CharmHadD02HH_D02KKWideMassDecision_TOS, &b_D0Hlt2CharmHadD02HH_D02KKWideMassDecision_TOS);
   fChain->SetBranchAddress("D0Hlt2CharmHadD02HH_D02PiPiDecision_Dec", &D0Hlt2CharmHadD02HH_D02PiPiDecision_Dec, &b_D0Hlt2CharmHadD02HH_D02PiPiDecision_Dec);
   fChain->SetBranchAddress("D0Hlt2CharmHadD02HH_D02PiPiDecision_TIS", &D0Hlt2CharmHadD02HH_D02PiPiDecision_TIS, &b_D0Hlt2CharmHadD02HH_D02PiPiDecision_TIS);
   fChain->SetBranchAddress("D0Hlt2CharmHadD02HH_D02PiPiDecision_TOS", &D0Hlt2CharmHadD02HH_D02PiPiDecision_TOS, &b_D0Hlt2CharmHadD02HH_D02PiPiDecision_TOS);
   fChain->SetBranchAddress("D0Hlt2CharmHadD02HH_D02PiPiWideMassDecision_Dec", &D0Hlt2CharmHadD02HH_D02PiPiWideMassDecision_Dec, &b_D0Hlt2CharmHadD02HH_D02PiPiWideMassDecision_Dec);
   fChain->SetBranchAddress("D0Hlt2CharmHadD02HH_D02PiPiWideMassDecision_TIS", &D0Hlt2CharmHadD02HH_D02PiPiWideMassDecision_TIS, &b_D0Hlt2CharmHadD02HH_D02PiPiWideMassDecision_TIS);
   fChain->SetBranchAddress("D0Hlt2CharmHadD02HH_D02PiPiWideMassDecision_TOS", &D0Hlt2CharmHadD02HH_D02PiPiWideMassDecision_TOS, &b_D0Hlt2CharmHadD02HH_D02PiPiWideMassDecision_TOS);
   fChain->SetBranchAddress("dauplus_MINIP", &dauplus_MINIP, &b_dauplus_MINIP);
   fChain->SetBranchAddress("dauplus_MINIPCHI2", &dauplus_MINIPCHI2, &b_dauplus_MINIPCHI2);
   fChain->SetBranchAddress("dauplus_MINIPNEXTBEST", &dauplus_MINIPNEXTBEST, &b_dauplus_MINIPNEXTBEST);
   fChain->SetBranchAddress("dauplus_MINIPCHI2NEXTBEST", &dauplus_MINIPCHI2NEXTBEST, &b_dauplus_MINIPCHI2NEXTBEST);
   fChain->SetBranchAddress("dauplus_OWNPV_X", &dauplus_OWNPV_X, &b_dauplus_OWNPV_X);
   fChain->SetBranchAddress("dauplus_OWNPV_Y", &dauplus_OWNPV_Y, &b_dauplus_OWNPV_Y);
   fChain->SetBranchAddress("dauplus_OWNPV_Z", &dauplus_OWNPV_Z, &b_dauplus_OWNPV_Z);
   fChain->SetBranchAddress("dauplus_OWNPV_XERR", &dauplus_OWNPV_XERR, &b_dauplus_OWNPV_XERR);
   fChain->SetBranchAddress("dauplus_OWNPV_YERR", &dauplus_OWNPV_YERR, &b_dauplus_OWNPV_YERR);
   fChain->SetBranchAddress("dauplus_OWNPV_ZERR", &dauplus_OWNPV_ZERR, &b_dauplus_OWNPV_ZERR);
   fChain->SetBranchAddress("dauplus_OWNPV_CHI2", &dauplus_OWNPV_CHI2, &b_dauplus_OWNPV_CHI2);
   fChain->SetBranchAddress("dauplus_OWNPV_NDOF", &dauplus_OWNPV_NDOF, &b_dauplus_OWNPV_NDOF);
   fChain->SetBranchAddress("dauplus_OWNPV_COV_", dauplus_OWNPV_COV_, &b_dauplus_OWNPV_COV_);
   fChain->SetBranchAddress("dauplus_IP_OWNPV", &dauplus_IP_OWNPV, &b_dauplus_IP_OWNPV);
   fChain->SetBranchAddress("dauplus_IPCHI2_OWNPV", &dauplus_IPCHI2_OWNPV, &b_dauplus_IPCHI2_OWNPV);
   fChain->SetBranchAddress("dauplus_ORIVX_X", &dauplus_ORIVX_X, &b_dauplus_ORIVX_X);
   fChain->SetBranchAddress("dauplus_ORIVX_Y", &dauplus_ORIVX_Y, &b_dauplus_ORIVX_Y);
   fChain->SetBranchAddress("dauplus_ORIVX_Z", &dauplus_ORIVX_Z, &b_dauplus_ORIVX_Z);
   fChain->SetBranchAddress("dauplus_ORIVX_XERR", &dauplus_ORIVX_XERR, &b_dauplus_ORIVX_XERR);
   fChain->SetBranchAddress("dauplus_ORIVX_YERR", &dauplus_ORIVX_YERR, &b_dauplus_ORIVX_YERR);
   fChain->SetBranchAddress("dauplus_ORIVX_ZERR", &dauplus_ORIVX_ZERR, &b_dauplus_ORIVX_ZERR);
   fChain->SetBranchAddress("dauplus_ORIVX_CHI2", &dauplus_ORIVX_CHI2, &b_dauplus_ORIVX_CHI2);
   fChain->SetBranchAddress("dauplus_ORIVX_NDOF", &dauplus_ORIVX_NDOF, &b_dauplus_ORIVX_NDOF);
   fChain->SetBranchAddress("dauplus_ORIVX_COV_", dauplus_ORIVX_COV_, &b_dauplus_ORIVX_COV_);
   fChain->SetBranchAddress("dauplus_P", &dauplus_P, &b_dauplus_P);
   fChain->SetBranchAddress("dauplus_PT", &dauplus_PT, &b_dauplus_PT);
   fChain->SetBranchAddress("dauplus_PE", &dauplus_PE, &b_dauplus_PE);
   fChain->SetBranchAddress("dauplus_PX", &dauplus_PX, &b_dauplus_PX);
   fChain->SetBranchAddress("dauplus_PY", &dauplus_PY, &b_dauplus_PY);
   fChain->SetBranchAddress("dauplus_PZ", &dauplus_PZ, &b_dauplus_PZ);
   fChain->SetBranchAddress("dauplus_M", &dauplus_M, &b_dauplus_M);
   fChain->SetBranchAddress("dauplus_ID", &dauplus_ID, &b_dauplus_ID);
   fChain->SetBranchAddress("dauplus_PIDe", &dauplus_PIDe, &b_dauplus_PIDe);
   fChain->SetBranchAddress("dauplus_PIDmu", &dauplus_PIDmu, &b_dauplus_PIDmu);
   fChain->SetBranchAddress("dauplus_PIDK", &dauplus_PIDK, &b_dauplus_PIDK);
   fChain->SetBranchAddress("dauplus_PIDp", &dauplus_PIDp, &b_dauplus_PIDp);
   fChain->SetBranchAddress("dauplus_ProbNNe", &dauplus_ProbNNe, &b_dauplus_ProbNNe);
   fChain->SetBranchAddress("dauplus_ProbNNk", &dauplus_ProbNNk, &b_dauplus_ProbNNk);
   fChain->SetBranchAddress("dauplus_ProbNNp", &dauplus_ProbNNp, &b_dauplus_ProbNNp);
   fChain->SetBranchAddress("dauplus_ProbNNpi", &dauplus_ProbNNpi, &b_dauplus_ProbNNpi);
   fChain->SetBranchAddress("dauplus_ProbNNmu", &dauplus_ProbNNmu, &b_dauplus_ProbNNmu);
   fChain->SetBranchAddress("dauplus_ProbNNghost", &dauplus_ProbNNghost, &b_dauplus_ProbNNghost);
   fChain->SetBranchAddress("dauplus_CaloEcalE", &dauplus_CaloEcalE, &b_dauplus_CaloEcalE);
   fChain->SetBranchAddress("dauplus_CaloHcalE", &dauplus_CaloHcalE, &b_dauplus_CaloHcalE);
   fChain->SetBranchAddress("dauplus_hasMuon", &dauplus_hasMuon, &b_dauplus_hasMuon);
   fChain->SetBranchAddress("dauplus_isMuon", &dauplus_isMuon, &b_dauplus_isMuon);
   fChain->SetBranchAddress("dauplus_hasRich", &dauplus_hasRich, &b_dauplus_hasRich);
   fChain->SetBranchAddress("dauplus_hasCalo", &dauplus_hasCalo, &b_dauplus_hasCalo);
   fChain->SetBranchAddress("dauplus_TRACK_Type", &dauplus_TRACK_Type, &b_dauplus_TRACK_Type);
   fChain->SetBranchAddress("dauplus_TRACK_Key", &dauplus_TRACK_Key, &b_dauplus_TRACK_Key);
   fChain->SetBranchAddress("dauplus_TRACK_CHI2NDOF", &dauplus_TRACK_CHI2NDOF, &b_dauplus_TRACK_CHI2NDOF);
   fChain->SetBranchAddress("dauplus_TRACK_PCHI2", &dauplus_TRACK_PCHI2, &b_dauplus_TRACK_PCHI2);
   fChain->SetBranchAddress("dauplus_TRACK_GhostProb", &dauplus_TRACK_GhostProb, &b_dauplus_TRACK_GhostProb);
   fChain->SetBranchAddress("dauplus_TRACK_CloneDist", &dauplus_TRACK_CloneDist, &b_dauplus_TRACK_CloneDist);
   fChain->SetBranchAddress("dauplus_Trigger_nTP", &dauplus_Trigger_nTP, &b_dauplus_Trigger_nTP);
   fChain->SetBranchAddress("dauplus_Trigger_TP_DEC_trRec_HLT1", dauplus_Trigger_TP_DEC_trRec_HLT1, &b_dauplus_Trigger_TP_DEC_trRec_HLT1);
   fChain->SetBranchAddress("dauplus_Trigger_TP_DEC_trRec_HLT2", dauplus_Trigger_TP_DEC_trRec_HLT2, &b_dauplus_Trigger_TP_DEC_trRec_HLT2);
   fChain->SetBranchAddress("dauplus_Trigger_TP_DEC_trRec_VELO_HLT1", dauplus_Trigger_TP_DEC_trRec_VELO_HLT1, &b_dauplus_Trigger_TP_DEC_trRec_VELO_HLT1);
   fChain->SetBranchAddress("dauplus_Trigger_TP_DEC_trRec_VELO_OFF", dauplus_Trigger_TP_DEC_trRec_VELO_OFF, &b_dauplus_Trigger_TP_DEC_trRec_VELO_OFF);
   fChain->SetBranchAddress("dauplus_Trigger_TP_DEC_trTOS_Hlt1TrackAllL0Decision", dauplus_Trigger_TP_DEC_trTOS_Hlt1TrackAllL0Decision, &b_dauplus_Trigger_TP_DEC_trTOS_Hlt1TrackAllL0Decision);
   fChain->SetBranchAddress("dauplus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiDecision", dauplus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiDecision, &b_dauplus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiDecision);
   fChain->SetBranchAddress("dauplus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiWideMassDecision", dauplus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiWideMassDecision, &b_dauplus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiWideMassDecision);
   fChain->SetBranchAddress("dauminus_MINIP", &dauminus_MINIP, &b_dauminus_MINIP);
   fChain->SetBranchAddress("dauminus_MINIPCHI2", &dauminus_MINIPCHI2, &b_dauminus_MINIPCHI2);
   fChain->SetBranchAddress("dauminus_MINIPNEXTBEST", &dauminus_MINIPNEXTBEST, &b_dauminus_MINIPNEXTBEST);
   fChain->SetBranchAddress("dauminus_MINIPCHI2NEXTBEST", &dauminus_MINIPCHI2NEXTBEST, &b_dauminus_MINIPCHI2NEXTBEST);
   fChain->SetBranchAddress("dauminus_OWNPV_X", &dauminus_OWNPV_X, &b_dauminus_OWNPV_X);
   fChain->SetBranchAddress("dauminus_OWNPV_Y", &dauminus_OWNPV_Y, &b_dauminus_OWNPV_Y);
   fChain->SetBranchAddress("dauminus_OWNPV_Z", &dauminus_OWNPV_Z, &b_dauminus_OWNPV_Z);
   fChain->SetBranchAddress("dauminus_OWNPV_XERR", &dauminus_OWNPV_XERR, &b_dauminus_OWNPV_XERR);
   fChain->SetBranchAddress("dauminus_OWNPV_YERR", &dauminus_OWNPV_YERR, &b_dauminus_OWNPV_YERR);
   fChain->SetBranchAddress("dauminus_OWNPV_ZERR", &dauminus_OWNPV_ZERR, &b_dauminus_OWNPV_ZERR);
   fChain->SetBranchAddress("dauminus_OWNPV_CHI2", &dauminus_OWNPV_CHI2, &b_dauminus_OWNPV_CHI2);
   fChain->SetBranchAddress("dauminus_OWNPV_NDOF", &dauminus_OWNPV_NDOF, &b_dauminus_OWNPV_NDOF);
   fChain->SetBranchAddress("dauminus_OWNPV_COV_", dauminus_OWNPV_COV_, &b_dauminus_OWNPV_COV_);
   fChain->SetBranchAddress("dauminus_IP_OWNPV", &dauminus_IP_OWNPV, &b_dauminus_IP_OWNPV);
   fChain->SetBranchAddress("dauminus_IPCHI2_OWNPV", &dauminus_IPCHI2_OWNPV, &b_dauminus_IPCHI2_OWNPV);
   fChain->SetBranchAddress("dauminus_ORIVX_X", &dauminus_ORIVX_X, &b_dauminus_ORIVX_X);
   fChain->SetBranchAddress("dauminus_ORIVX_Y", &dauminus_ORIVX_Y, &b_dauminus_ORIVX_Y);
   fChain->SetBranchAddress("dauminus_ORIVX_Z", &dauminus_ORIVX_Z, &b_dauminus_ORIVX_Z);
   fChain->SetBranchAddress("dauminus_ORIVX_XERR", &dauminus_ORIVX_XERR, &b_dauminus_ORIVX_XERR);
   fChain->SetBranchAddress("dauminus_ORIVX_YERR", &dauminus_ORIVX_YERR, &b_dauminus_ORIVX_YERR);
   fChain->SetBranchAddress("dauminus_ORIVX_ZERR", &dauminus_ORIVX_ZERR, &b_dauminus_ORIVX_ZERR);
   fChain->SetBranchAddress("dauminus_ORIVX_CHI2", &dauminus_ORIVX_CHI2, &b_dauminus_ORIVX_CHI2);
   fChain->SetBranchAddress("dauminus_ORIVX_NDOF", &dauminus_ORIVX_NDOF, &b_dauminus_ORIVX_NDOF);
   fChain->SetBranchAddress("dauminus_ORIVX_COV_", dauminus_ORIVX_COV_, &b_dauminus_ORIVX_COV_);
   fChain->SetBranchAddress("dauminus_P", &dauminus_P, &b_dauminus_P);
   fChain->SetBranchAddress("dauminus_PT", &dauminus_PT, &b_dauminus_PT);
   fChain->SetBranchAddress("dauminus_PE", &dauminus_PE, &b_dauminus_PE);
   fChain->SetBranchAddress("dauminus_PX", &dauminus_PX, &b_dauminus_PX);
   fChain->SetBranchAddress("dauminus_PY", &dauminus_PY, &b_dauminus_PY);
   fChain->SetBranchAddress("dauminus_PZ", &dauminus_PZ, &b_dauminus_PZ);
   fChain->SetBranchAddress("dauminus_M", &dauminus_M, &b_dauminus_M);
   fChain->SetBranchAddress("dauminus_ID", &dauminus_ID, &b_dauminus_ID);
   fChain->SetBranchAddress("dauminus_PIDe", &dauminus_PIDe, &b_dauminus_PIDe);
   fChain->SetBranchAddress("dauminus_PIDmu", &dauminus_PIDmu, &b_dauminus_PIDmu);
   fChain->SetBranchAddress("dauminus_PIDK", &dauminus_PIDK, &b_dauminus_PIDK);
   fChain->SetBranchAddress("dauminus_PIDp", &dauminus_PIDp, &b_dauminus_PIDp);
   fChain->SetBranchAddress("dauminus_ProbNNe", &dauminus_ProbNNe, &b_dauminus_ProbNNe);
   fChain->SetBranchAddress("dauminus_ProbNNk", &dauminus_ProbNNk, &b_dauminus_ProbNNk);
   fChain->SetBranchAddress("dauminus_ProbNNp", &dauminus_ProbNNp, &b_dauminus_ProbNNp);
   fChain->SetBranchAddress("dauminus_ProbNNpi", &dauminus_ProbNNpi, &b_dauminus_ProbNNpi);
   fChain->SetBranchAddress("dauminus_ProbNNmu", &dauminus_ProbNNmu, &b_dauminus_ProbNNmu);
   fChain->SetBranchAddress("dauminus_ProbNNghost", &dauminus_ProbNNghost, &b_dauminus_ProbNNghost);
   fChain->SetBranchAddress("dauminus_CaloEcalE", &dauminus_CaloEcalE, &b_dauminus_CaloEcalE);
   fChain->SetBranchAddress("dauminus_CaloHcalE", &dauminus_CaloHcalE, &b_dauminus_CaloHcalE);
   fChain->SetBranchAddress("dauminus_hasMuon", &dauminus_hasMuon, &b_dauminus_hasMuon);
   fChain->SetBranchAddress("dauminus_isMuon", &dauminus_isMuon, &b_dauminus_isMuon);
   fChain->SetBranchAddress("dauminus_hasRich", &dauminus_hasRich, &b_dauminus_hasRich);
   fChain->SetBranchAddress("dauminus_hasCalo", &dauminus_hasCalo, &b_dauminus_hasCalo);
   fChain->SetBranchAddress("dauminus_TRACK_Type", &dauminus_TRACK_Type, &b_dauminus_TRACK_Type);
   fChain->SetBranchAddress("dauminus_TRACK_Key", &dauminus_TRACK_Key, &b_dauminus_TRACK_Key);
   fChain->SetBranchAddress("dauminus_TRACK_CHI2NDOF", &dauminus_TRACK_CHI2NDOF, &b_dauminus_TRACK_CHI2NDOF);
   fChain->SetBranchAddress("dauminus_TRACK_PCHI2", &dauminus_TRACK_PCHI2, &b_dauminus_TRACK_PCHI2);
   fChain->SetBranchAddress("dauminus_TRACK_GhostProb", &dauminus_TRACK_GhostProb, &b_dauminus_TRACK_GhostProb);
   fChain->SetBranchAddress("dauminus_TRACK_CloneDist", &dauminus_TRACK_CloneDist, &b_dauminus_TRACK_CloneDist);
   fChain->SetBranchAddress("dauminus_Trigger_nTP", &dauminus_Trigger_nTP, &b_dauminus_Trigger_nTP);
   fChain->SetBranchAddress("dauminus_Trigger_TP_DEC_trRec_HLT1", dauminus_Trigger_TP_DEC_trRec_HLT1, &b_dauminus_Trigger_TP_DEC_trRec_HLT1);
   fChain->SetBranchAddress("dauminus_Trigger_TP_DEC_trRec_HLT2", dauminus_Trigger_TP_DEC_trRec_HLT2, &b_dauminus_Trigger_TP_DEC_trRec_HLT2);
   fChain->SetBranchAddress("dauminus_Trigger_TP_DEC_trRec_VELO_HLT1", dauminus_Trigger_TP_DEC_trRec_VELO_HLT1, &b_dauminus_Trigger_TP_DEC_trRec_VELO_HLT1);
   fChain->SetBranchAddress("dauminus_Trigger_TP_DEC_trRec_VELO_OFF", dauminus_Trigger_TP_DEC_trRec_VELO_OFF, &b_dauminus_Trigger_TP_DEC_trRec_VELO_OFF);
   fChain->SetBranchAddress("dauminus_Trigger_TP_DEC_trTOS_Hlt1TrackAllL0Decision", dauminus_Trigger_TP_DEC_trTOS_Hlt1TrackAllL0Decision, &b_dauminus_Trigger_TP_DEC_trTOS_Hlt1TrackAllL0Decision);
   fChain->SetBranchAddress("dauminus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiDecision", dauminus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiDecision, &b_dauminus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiDecision);
   fChain->SetBranchAddress("dauminus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiWideMassDecision", dauminus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiWideMassDecision, &b_dauminus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiWideMassDecision);
   fChain->SetBranchAddress("nCandidate", &nCandidate, &b_nCandidate);
   fChain->SetBranchAddress("totCandidates", &totCandidates, &b_totCandidates);
   fChain->SetBranchAddress("EventInSequence", &EventInSequence, &b_EventInSequence);
   fChain->SetBranchAddress("runNumber", &runNumber, &b_runNumber);
   fChain->SetBranchAddress("eventNumber", &eventNumber, &b_eventNumber);
   fChain->SetBranchAddress("BCID", &BCID, &b_BCID);
   fChain->SetBranchAddress("BCType", &BCType, &b_BCType);
   fChain->SetBranchAddress("OdinTCK", &OdinTCK, &b_OdinTCK);
   fChain->SetBranchAddress("L0DUTCK", &L0DUTCK, &b_L0DUTCK);
   fChain->SetBranchAddress("HLTTCK", &HLTTCK, &b_HLTTCK);
   fChain->SetBranchAddress("GpsTime", &GpsTime, &b_GpsTime);
   fChain->SetBranchAddress("Primaries", &Primaries, &b_Primaries);
   fChain->SetBranchAddress("Polarity", &Polarity, &b_Polarity);
   fChain->SetBranchAddress("nPV", &nPV, &b_nPV);
   fChain->SetBranchAddress("PVX", PVX, &b_PVX);
   fChain->SetBranchAddress("PVY", PVY, &b_PVY);
   fChain->SetBranchAddress("PVZ", PVZ, &b_PVZ);
   fChain->SetBranchAddress("PVXERR", PVXERR, &b_PVXERR);
   fChain->SetBranchAddress("PVYERR", PVYERR, &b_PVYERR);
   fChain->SetBranchAddress("PVZERR", PVZERR, &b_PVZERR);
   fChain->SetBranchAddress("PVCHI2", PVCHI2, &b_PVCHI2);
   fChain->SetBranchAddress("PVNDOF", PVNDOF, &b_PVNDOF);
   fChain->SetBranchAddress("PVNTRACKS", PVNTRACKS, &b_PVNTRACKS);
   Notify();
}

Bool_t CorrectVELOSwimInfo_D02PiPi::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void CorrectVELOSwimInfo_D02PiPi::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t CorrectVELOSwimInfo_D02PiPi::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   if (entry > -1) return 1;
   else return 0; 
}
#endif // #ifdef CorrectVELOSwimInfo_D02PiPi_cxx
