//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Jun 13 10:36:07 2007 by ROOT version 5.13/04c
// from TTree 4242/Events
// found on file: 0601-11144100-nt.root
//////////////////////////////////////////////////////////

#ifndef TrackTypeTool_h
#define TrackTypeTool_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

#define MAXTRACKS	1000
#define MAXMCPARTICLES	10000

class TrackTypeTool {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leave types
   Int_t           nMCParticles;
   Int_t           nSavedMCParticles;
   Int_t           nTracks;
   Int_t           nSavedTracks;
   Int_t           nVeloHits;
   Int_t           nTTHits;
   Int_t           nITHits;
   Int_t           nOTHits;
   Int_t           nMaxSavedTracks;
   Float_t         Tracks_type[MAXTRACKS];   //[nMaxSavedTracks]
   Float_t         Tracks_chi2[MAXTRACKS];   //[nMaxSavedTracks]
   Float_t         Tracks_nDoF[MAXTRACKS];   //[nMaxSavedTracks]
   Float_t         Tracks_Pchi2[MAXTRACKS];   //[nMaxSavedTracks]
   Float_t         Tracks_History[MAXTRACKS];   //[nMaxSavedTracks]
   Float_t         Tracks_assocMCParts[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_assocMCPartWeights[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_firstState[MAXTRACKS][21];   //[nMaxSavedTracks]
   Float_t         Tracks_nTotalHits[MAXTRACKS];   //[nMaxSavedTracks]
   Float_t         Tracks_nMCTotalHits[MAXTRACKS];   //[nMaxSavedTracks]
   Float_t         Tracks_nGoodHits[MAXTRACKS];   //[nMaxSavedTracks]
   Float_t         Tracks_nTotalVelo[MAXTRACKS];   //[nMaxSavedTracks]
   Float_t         Tracks_nMCTotalVelo[MAXTRACKS];   //[nMaxSavedTracks]
   Float_t         Tracks_nGoodVelo[MAXTRACKS];   //[nMaxSavedTracks]
   Float_t         Tracks_nTotalTT[MAXTRACKS];   //[nMaxSavedTracks]
   Float_t         Tracks_nMCTotalTT[MAXTRACKS];   //[nMaxSavedTracks]
   Float_t         Tracks_nGoodTT[MAXTRACKS];   //[nMaxSavedTracks]
   Float_t         Tracks_nTotalIT[MAXTRACKS];   //[nMaxSavedTracks]
   Float_t         Tracks_nMCTotalIT[MAXTRACKS];   //[nMaxSavedTracks]
   Float_t         Tracks_nGoodIT[MAXTRACKS];   //[nMaxSavedTracks]
   Float_t         Tracks_nTotalOT[MAXTRACKS];   //[nMaxSavedTracks]
   Float_t         Tracks_nMCTotalOT[MAXTRACKS];   //[nMaxSavedTracks]
   Float_t         Tracks_nGoodOT[MAXTRACKS];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZz[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZx[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZy[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZtx[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZty[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZqp[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZCxx[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZCxy[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZCxtx[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZCxty[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZCxqp[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZCyy[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZCytx[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZCyty[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZCyqp[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZCtxtx[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZCtxty[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZCtxqp[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZCtyty[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZCtyqp[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZCqpqp[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZTx[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZTy[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZTtx[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZTty[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZTqp[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZTCxx[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZTCxy[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZTCxtx[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZTCxty[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZTCxqp[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZTCyy[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZTCytx[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZTCyty[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZTCyqp[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZTCtxtx[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZTCtxty[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZTCtxqp[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZTCtyty[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZTCtyqp[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZTCqpqp[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZMeasResidual[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZMeasError[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZMeasType[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZLayer[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_FixedZMCParticle[MAXTRACKS][10];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRz[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRx[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRy[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRtx[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRty[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRqp[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRCxx[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRCxy[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRCxtx[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRCxty[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRCxqp[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRCyy[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRCytx[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRCyty[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRCyqp[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRCtxtx[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRCtxty[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRCtxqp[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRCtyty[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRCtyqp[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRCqpqp[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRTx[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRTy[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRTtx[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRTty[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRTqp[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRTCxx[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRTCxy[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRTCxtx[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRTCxty[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRTCxqp[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRTCyy[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRTCytx[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRTCyty[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRTCyqp[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRTCtxtx[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRTCtxty[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRTCtxqp[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRTCtyty[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRTCtyqp[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRTCqpqp[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRMeasResidual[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRMeasError[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRMeasType[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRLayer[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloRMCParticle[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiz[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhix[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiy[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhitx[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhity[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiqp[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiCxx[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiCxy[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiCxtx[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiCxty[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiCxqp[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiCyy[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiCytx[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiCyty[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiCyqp[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiCtxtx[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiCtxty[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiCtxqp[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiCtyty[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiCtyqp[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiCqpqp[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiTx[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiTy[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiTtx[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiTty[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiTqp[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiTCxx[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiTCxy[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiTCxtx[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiTCxty[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiTCxqp[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiTCyy[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiTCytx[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiTCyty[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiTCyqp[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiTCtxtx[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiTCtxty[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiTCtxqp[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiTCtyty[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiTCtyqp[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiTCqpqp[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiMeasResidual[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiMeasError[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiMeasType[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiLayer[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_VeloPhiMCParticle[MAXTRACKS][25];   //[nMaxSavedTracks]
   Float_t         Tracks_TTz[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTx[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTy[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTtx[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTty[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTqp[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTCxx[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTCxy[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTCxtx[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTCxty[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTCxqp[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTCyy[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTCytx[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTCyty[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTCyqp[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTCtxtx[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTCtxty[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTCtxqp[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTCtyty[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTCtyqp[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTCqpqp[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTTx[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTTy[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTTtx[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTTty[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTTqp[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTTCxx[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTTCxy[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTTCxtx[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTTCxty[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTTCxqp[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTTCyy[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTTCytx[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTTCyty[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTTCyqp[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTTCtxtx[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTTCtxty[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTTCtxqp[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTTCtyty[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTTCtyqp[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTTCqpqp[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTMeasResidual[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTMeasError[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTMeasType[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTLayer[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_TTMCParticle[MAXTRACKS][12];   //[nMaxSavedTracks]
   Float_t         Tracks_ITz[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITx[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITy[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITtx[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITty[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITqp[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITCxx[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITCxy[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITCxtx[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITCxty[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITCxqp[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITCyy[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITCytx[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITCyty[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITCyqp[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITCtxtx[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITCtxty[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITCtxqp[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITCtyty[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITCtyqp[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITCqpqp[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITTx[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITTy[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITTtx[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITTty[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITTqp[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITTCxx[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITTCxy[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITTCxtx[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITTCxty[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITTCxqp[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITTCyy[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITTCytx[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITTCyty[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITTCyqp[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITTCtxtx[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITTCtxty[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITTCtxqp[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITTCtyty[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITTCtyqp[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITTCqpqp[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITMeasResidual[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITMeasError[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITMeasType[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITLayer[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_ITMCParticle[MAXTRACKS][20];   //[nMaxSavedTracks]
   Float_t         Tracks_OTz[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTx[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTy[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTtx[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTty[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTqp[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTCxx[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTCxy[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTCxtx[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTCxty[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTCxqp[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTCyy[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTCytx[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTCyty[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTCyqp[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTCtxtx[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTCtxty[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTCtxqp[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTCtyty[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTCtyqp[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTCqpqp[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTTx[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTTy[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTTtx[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTTty[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTTqp[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTTCxx[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTTCxy[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTTCxtx[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTTCxty[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTTCxqp[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTTCyy[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTTCytx[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTTCyty[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTTCyqp[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTTCtxtx[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTTCtxty[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTTCtxqp[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTTCtyty[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTTCtyqp[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTTCqpqp[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTMeasResidual[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTMeasError[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTMeasType[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTLayer[MAXTRACKS][32];   //[nMaxSavedTracks]
   Float_t         Tracks_OTMCParticle[MAXTRACKS][32];   //[nMaxSavedTracks]
   Int_t           nMaxSavedMCParticles;
   Float_t         MCParticles_PID[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_mother[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_momentum[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_assocTracks[MAXMCPARTICLES][10];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_assocTrackWeights[MAXMCPARTICLES][10];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_origVtxPos[MAXMCPARTICLES][3];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_4momentum[MAXMCPARTICLES][4];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nVeloRHits[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nVeloPhiHits[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nVeloHits[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nVeloRStations[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nVeloPhiStations[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nVeloStations[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nTTXHits[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nTTUHits[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nTTVHits[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nTTXStations[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nTTUStations[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nTTVStations[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nTTStations[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nTTStStations[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nTTHits[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nITXHits[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nITUHits[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nITVHits[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nITXStations[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nITUStations[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nITVStations[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nITStations[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nITStStations[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nITHits[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nOTXHits[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nOTUHits[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nOTVHits[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nOTXStations[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nOTUStations[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nOTVStations[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nOTStations[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nOTStStations[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nOTHits[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nTXStations[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nTUStations[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nTVStations[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nTStStations[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_nTStations[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_HitMaskVeloR[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_HitMaskVeloPhi[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_HitMaskTT[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_HitMaskIT[MAXMCPARTICLES];   //[nMaxSavedMCParticles]
   Float_t         MCParticles_HitMaskOT[MAXMCPARTICLES];   //[nMaxSavedMCParticles]

   // List of branches
   TBranch        *b_nMCParticles;   //!
   TBranch        *b_nSavedMCParticles;   //!
   TBranch        *b_nTracks;   //!
   TBranch        *b_nSavedTracks;   //!
   TBranch        *b_nVeloHits;   //!
   TBranch        *b_nTTHits;   //!
   TBranch        *b_nITHits;   //!
   TBranch        *b_nOTHits;   //!
   TBranch        *b_nMaxSavedTracks;   //!
   TBranch        *b_Tracks_type;   //!
   TBranch        *b_Tracks_chi2;   //!
   TBranch        *b_Tracks_nDoF;   //!
   TBranch        *b_Tracks_Pchi2;   //!
   TBranch        *b_Tracks_History;   //!
   TBranch        *b_Tracks_assocMCParts;   //!
   TBranch        *b_Tracks_assocMCPartWeights;   //!
   TBranch        *b_Tracks_firstState;   //!
   TBranch        *b_Tracks_nTotalHits;   //!
   TBranch        *b_Tracks_nMCTotalHits;   //!
   TBranch        *b_Tracks_nGoodHits;   //!
   TBranch        *b_Tracks_nTotalVelo;   //!
   TBranch        *b_Tracks_nMCTotalVelo;   //!
   TBranch        *b_Tracks_nGoodVelo;   //!
   TBranch        *b_Tracks_nTotalTT;   //!
   TBranch        *b_Tracks_nMCTotalTT;   //!
   TBranch        *b_Tracks_nGoodTT;   //!
   TBranch        *b_Tracks_nTotalIT;   //!
   TBranch        *b_Tracks_nMCTotalIT;   //!
   TBranch        *b_Tracks_nGoodIT;   //!
   TBranch        *b_Tracks_nTotalOT;   //!
   TBranch        *b_Tracks_nMCTotalOT;   //!
   TBranch        *b_Tracks_nGoodOT;   //!
   TBranch        *b_Tracks_FixedZz;   //!
   TBranch        *b_Tracks_FixedZx;   //!
   TBranch        *b_Tracks_FixedZy;   //!
   TBranch        *b_Tracks_FixedZtx;   //!
   TBranch        *b_Tracks_FixedZty;   //!
   TBranch        *b_Tracks_FixedZqp;   //!
   TBranch        *b_Tracks_FixedZCxx;   //!
   TBranch        *b_Tracks_FixedZCxy;   //!
   TBranch        *b_Tracks_FixedZCxtx;   //!
   TBranch        *b_Tracks_FixedZCxty;   //!
   TBranch        *b_Tracks_FixedZCxqp;   //!
   TBranch        *b_Tracks_FixedZCyy;   //!
   TBranch        *b_Tracks_FixedZCytx;   //!
   TBranch        *b_Tracks_FixedZCyty;   //!
   TBranch        *b_Tracks_FixedZCyqp;   //!
   TBranch        *b_Tracks_FixedZCtxtx;   //!
   TBranch        *b_Tracks_FixedZCtxty;   //!
   TBranch        *b_Tracks_FixedZCtxqp;   //!
   TBranch        *b_Tracks_FixedZCtyty;   //!
   TBranch        *b_Tracks_FixedZCtyqp;   //!
   TBranch        *b_Tracks_FixedZCqpqp;   //!
   TBranch        *b_Tracks_FixedZTx;   //!
   TBranch        *b_Tracks_FixedZTy;   //!
   TBranch        *b_Tracks_FixedZTtx;   //!
   TBranch        *b_Tracks_FixedZTty;   //!
   TBranch        *b_Tracks_FixedZTqp;   //!
   TBranch        *b_Tracks_FixedZTCxx;   //!
   TBranch        *b_Tracks_FixedZTCxy;   //!
   TBranch        *b_Tracks_FixedZTCxtx;   //!
   TBranch        *b_Tracks_FixedZTCxty;   //!
   TBranch        *b_Tracks_FixedZTCxqp;   //!
   TBranch        *b_Tracks_FixedZTCyy;   //!
   TBranch        *b_Tracks_FixedZTCytx;   //!
   TBranch        *b_Tracks_FixedZTCyty;   //!
   TBranch        *b_Tracks_FixedZTCyqp;   //!
   TBranch        *b_Tracks_FixedZTCtxtx;   //!
   TBranch        *b_Tracks_FixedZTCtxty;   //!
   TBranch        *b_Tracks_FixedZTCtxqp;   //!
   TBranch        *b_Tracks_FixedZTCtyty;   //!
   TBranch        *b_Tracks_FixedZTCtyqp;   //!
   TBranch        *b_Tracks_FixedZTCqpqp;   //!
   TBranch        *b_Tracks_FixedZMeasResidual;   //!
   TBranch        *b_Tracks_FixedZMeasError;   //!
   TBranch        *b_Tracks_FixedZMeasType;   //!
   TBranch        *b_Tracks_FixedZLayer;   //!
   TBranch        *b_Tracks_FixedZMCParticle;   //!
   TBranch        *b_Tracks_VeloRz;   //!
   TBranch        *b_Tracks_VeloRx;   //!
   TBranch        *b_Tracks_VeloRy;   //!
   TBranch        *b_Tracks_VeloRtx;   //!
   TBranch        *b_Tracks_VeloRty;   //!
   TBranch        *b_Tracks_VeloRqp;   //!
   TBranch        *b_Tracks_VeloRCxx;   //!
   TBranch        *b_Tracks_VeloRCxy;   //!
   TBranch        *b_Tracks_VeloRCxtx;   //!
   TBranch        *b_Tracks_VeloRCxty;   //!
   TBranch        *b_Tracks_VeloRCxqp;   //!
   TBranch        *b_Tracks_VeloRCyy;   //!
   TBranch        *b_Tracks_VeloRCytx;   //!
   TBranch        *b_Tracks_VeloRCyty;   //!
   TBranch        *b_Tracks_VeloRCyqp;   //!
   TBranch        *b_Tracks_VeloRCtxtx;   //!
   TBranch        *b_Tracks_VeloRCtxty;   //!
   TBranch        *b_Tracks_VeloRCtxqp;   //!
   TBranch        *b_Tracks_VeloRCtyty;   //!
   TBranch        *b_Tracks_VeloRCtyqp;   //!
   TBranch        *b_Tracks_VeloRCqpqp;   //!
   TBranch        *b_Tracks_VeloRTx;   //!
   TBranch        *b_Tracks_VeloRTy;   //!
   TBranch        *b_Tracks_VeloRTtx;   //!
   TBranch        *b_Tracks_VeloRTty;   //!
   TBranch        *b_Tracks_VeloRTqp;   //!
   TBranch        *b_Tracks_VeloRTCxx;   //!
   TBranch        *b_Tracks_VeloRTCxy;   //!
   TBranch        *b_Tracks_VeloRTCxtx;   //!
   TBranch        *b_Tracks_VeloRTCxty;   //!
   TBranch        *b_Tracks_VeloRTCxqp;   //!
   TBranch        *b_Tracks_VeloRTCyy;   //!
   TBranch        *b_Tracks_VeloRTCytx;   //!
   TBranch        *b_Tracks_VeloRTCyty;   //!
   TBranch        *b_Tracks_VeloRTCyqp;   //!
   TBranch        *b_Tracks_VeloRTCtxtx;   //!
   TBranch        *b_Tracks_VeloRTCtxty;   //!
   TBranch        *b_Tracks_VeloRTCtxqp;   //!
   TBranch        *b_Tracks_VeloRTCtyty;   //!
   TBranch        *b_Tracks_VeloRTCtyqp;   //!
   TBranch        *b_Tracks_VeloRTCqpqp;   //!
   TBranch        *b_Tracks_VeloRMeasResidual;   //!
   TBranch        *b_Tracks_VeloRMeasError;   //!
   TBranch        *b_Tracks_VeloRMeasType;   //!
   TBranch        *b_Tracks_VeloRLayer;   //!
   TBranch        *b_Tracks_VeloRMCParticle;   //!
   TBranch        *b_Tracks_VeloPhiz;   //!
   TBranch        *b_Tracks_VeloPhix;   //!
   TBranch        *b_Tracks_VeloPhiy;   //!
   TBranch        *b_Tracks_VeloPhitx;   //!
   TBranch        *b_Tracks_VeloPhity;   //!
   TBranch        *b_Tracks_VeloPhiqp;   //!
   TBranch        *b_Tracks_VeloPhiCxx;   //!
   TBranch        *b_Tracks_VeloPhiCxy;   //!
   TBranch        *b_Tracks_VeloPhiCxtx;   //!
   TBranch        *b_Tracks_VeloPhiCxty;   //!
   TBranch        *b_Tracks_VeloPhiCxqp;   //!
   TBranch        *b_Tracks_VeloPhiCyy;   //!
   TBranch        *b_Tracks_VeloPhiCytx;   //!
   TBranch        *b_Tracks_VeloPhiCyty;   //!
   TBranch        *b_Tracks_VeloPhiCyqp;   //!
   TBranch        *b_Tracks_VeloPhiCtxtx;   //!
   TBranch        *b_Tracks_VeloPhiCtxty;   //!
   TBranch        *b_Tracks_VeloPhiCtxqp;   //!
   TBranch        *b_Tracks_VeloPhiCtyty;   //!
   TBranch        *b_Tracks_VeloPhiCtyqp;   //!
   TBranch        *b_Tracks_VeloPhiCqpqp;   //!
   TBranch        *b_Tracks_VeloPhiTx;   //!
   TBranch        *b_Tracks_VeloPhiTy;   //!
   TBranch        *b_Tracks_VeloPhiTtx;   //!
   TBranch        *b_Tracks_VeloPhiTty;   //!
   TBranch        *b_Tracks_VeloPhiTqp;   //!
   TBranch        *b_Tracks_VeloPhiTCxx;   //!
   TBranch        *b_Tracks_VeloPhiTCxy;   //!
   TBranch        *b_Tracks_VeloPhiTCxtx;   //!
   TBranch        *b_Tracks_VeloPhiTCxty;   //!
   TBranch        *b_Tracks_VeloPhiTCxqp;   //!
   TBranch        *b_Tracks_VeloPhiTCyy;   //!
   TBranch        *b_Tracks_VeloPhiTCytx;   //!
   TBranch        *b_Tracks_VeloPhiTCyty;   //!
   TBranch        *b_Tracks_VeloPhiTCyqp;   //!
   TBranch        *b_Tracks_VeloPhiTCtxtx;   //!
   TBranch        *b_Tracks_VeloPhiTCtxty;   //!
   TBranch        *b_Tracks_VeloPhiTCtxqp;   //!
   TBranch        *b_Tracks_VeloPhiTCtyty;   //!
   TBranch        *b_Tracks_VeloPhiTCtyqp;   //!
   TBranch        *b_Tracks_VeloPhiTCqpqp;   //!
   TBranch        *b_Tracks_VeloPhiMeasResidual;   //!
   TBranch        *b_Tracks_VeloPhiMeasError;   //!
   TBranch        *b_Tracks_VeloPhiMeasType;   //!
   TBranch        *b_Tracks_VeloPhiLayer;   //!
   TBranch        *b_Tracks_VeloPhiMCParticle;   //!
   TBranch        *b_Tracks_TTz;   //!
   TBranch        *b_Tracks_TTx;   //!
   TBranch        *b_Tracks_TTy;   //!
   TBranch        *b_Tracks_TTtx;   //!
   TBranch        *b_Tracks_TTty;   //!
   TBranch        *b_Tracks_TTqp;   //!
   TBranch        *b_Tracks_TTCxx;   //!
   TBranch        *b_Tracks_TTCxy;   //!
   TBranch        *b_Tracks_TTCxtx;   //!
   TBranch        *b_Tracks_TTCxty;   //!
   TBranch        *b_Tracks_TTCxqp;   //!
   TBranch        *b_Tracks_TTCyy;   //!
   TBranch        *b_Tracks_TTCytx;   //!
   TBranch        *b_Tracks_TTCyty;   //!
   TBranch        *b_Tracks_TTCyqp;   //!
   TBranch        *b_Tracks_TTCtxtx;   //!
   TBranch        *b_Tracks_TTCtxty;   //!
   TBranch        *b_Tracks_TTCtxqp;   //!
   TBranch        *b_Tracks_TTCtyty;   //!
   TBranch        *b_Tracks_TTCtyqp;   //!
   TBranch        *b_Tracks_TTCqpqp;   //!
   TBranch        *b_Tracks_TTTx;   //!
   TBranch        *b_Tracks_TTTy;   //!
   TBranch        *b_Tracks_TTTtx;   //!
   TBranch        *b_Tracks_TTTty;   //!
   TBranch        *b_Tracks_TTTqp;   //!
   TBranch        *b_Tracks_TTTCxx;   //!
   TBranch        *b_Tracks_TTTCxy;   //!
   TBranch        *b_Tracks_TTTCxtx;   //!
   TBranch        *b_Tracks_TTTCxty;   //!
   TBranch        *b_Tracks_TTTCxqp;   //!
   TBranch        *b_Tracks_TTTCyy;   //!
   TBranch        *b_Tracks_TTTCytx;   //!
   TBranch        *b_Tracks_TTTCyty;   //!
   TBranch        *b_Tracks_TTTCyqp;   //!
   TBranch        *b_Tracks_TTTCtxtx;   //!
   TBranch        *b_Tracks_TTTCtxty;   //!
   TBranch        *b_Tracks_TTTCtxqp;   //!
   TBranch        *b_Tracks_TTTCtyty;   //!
   TBranch        *b_Tracks_TTTCtyqp;   //!
   TBranch        *b_Tracks_TTTCqpqp;   //!
   TBranch        *b_Tracks_TTMeasResidual;   //!
   TBranch        *b_Tracks_TTMeasError;   //!
   TBranch        *b_Tracks_TTMeasType;   //!
   TBranch        *b_Tracks_TTLayer;   //!
   TBranch        *b_Tracks_TTMCParticle;   //!
   TBranch        *b_Tracks_ITz;   //!
   TBranch        *b_Tracks_ITx;   //!
   TBranch        *b_Tracks_ITy;   //!
   TBranch        *b_Tracks_ITtx;   //!
   TBranch        *b_Tracks_ITty;   //!
   TBranch        *b_Tracks_ITqp;   //!
   TBranch        *b_Tracks_ITCxx;   //!
   TBranch        *b_Tracks_ITCxy;   //!
   TBranch        *b_Tracks_ITCxtx;   //!
   TBranch        *b_Tracks_ITCxty;   //!
   TBranch        *b_Tracks_ITCxqp;   //!
   TBranch        *b_Tracks_ITCyy;   //!
   TBranch        *b_Tracks_ITCytx;   //!
   TBranch        *b_Tracks_ITCyty;   //!
   TBranch        *b_Tracks_ITCyqp;   //!
   TBranch        *b_Tracks_ITCtxtx;   //!
   TBranch        *b_Tracks_ITCtxty;   //!
   TBranch        *b_Tracks_ITCtxqp;   //!
   TBranch        *b_Tracks_ITCtyty;   //!
   TBranch        *b_Tracks_ITCtyqp;   //!
   TBranch        *b_Tracks_ITCqpqp;   //!
   TBranch        *b_Tracks_ITTx;   //!
   TBranch        *b_Tracks_ITTy;   //!
   TBranch        *b_Tracks_ITTtx;   //!
   TBranch        *b_Tracks_ITTty;   //!
   TBranch        *b_Tracks_ITTqp;   //!
   TBranch        *b_Tracks_ITTCxx;   //!
   TBranch        *b_Tracks_ITTCxy;   //!
   TBranch        *b_Tracks_ITTCxtx;   //!
   TBranch        *b_Tracks_ITTCxty;   //!
   TBranch        *b_Tracks_ITTCxqp;   //!
   TBranch        *b_Tracks_ITTCyy;   //!
   TBranch        *b_Tracks_ITTCytx;   //!
   TBranch        *b_Tracks_ITTCyty;   //!
   TBranch        *b_Tracks_ITTCyqp;   //!
   TBranch        *b_Tracks_ITTCtxtx;   //!
   TBranch        *b_Tracks_ITTCtxty;   //!
   TBranch        *b_Tracks_ITTCtxqp;   //!
   TBranch        *b_Tracks_ITTCtyty;   //!
   TBranch        *b_Tracks_ITTCtyqp;   //!
   TBranch        *b_Tracks_ITTCqpqp;   //!
   TBranch        *b_Tracks_ITMeasResidual;   //!
   TBranch        *b_Tracks_ITMeasError;   //!
   TBranch        *b_Tracks_ITMeasType;   //!
   TBranch        *b_Tracks_ITLayer;   //!
   TBranch        *b_Tracks_ITMCParticle;   //!
   TBranch        *b_Tracks_OTz;   //!
   TBranch        *b_Tracks_OTx;   //!
   TBranch        *b_Tracks_OTy;   //!
   TBranch        *b_Tracks_OTtx;   //!
   TBranch        *b_Tracks_OTty;   //!
   TBranch        *b_Tracks_OTqp;   //!
   TBranch        *b_Tracks_OTCxx;   //!
   TBranch        *b_Tracks_OTCxy;   //!
   TBranch        *b_Tracks_OTCxtx;   //!
   TBranch        *b_Tracks_OTCxty;   //!
   TBranch        *b_Tracks_OTCxqp;   //!
   TBranch        *b_Tracks_OTCyy;   //!
   TBranch        *b_Tracks_OTCytx;   //!
   TBranch        *b_Tracks_OTCyty;   //!
   TBranch        *b_Tracks_OTCyqp;   //!
   TBranch        *b_Tracks_OTCtxtx;   //!
   TBranch        *b_Tracks_OTCtxty;   //!
   TBranch        *b_Tracks_OTCtxqp;   //!
   TBranch        *b_Tracks_OTCtyty;   //!
   TBranch        *b_Tracks_OTCtyqp;   //!
   TBranch        *b_Tracks_OTCqpqp;   //!
   TBranch        *b_Tracks_OTTx;   //!
   TBranch        *b_Tracks_OTTy;   //!
   TBranch        *b_Tracks_OTTtx;   //!
   TBranch        *b_Tracks_OTTty;   //!
   TBranch        *b_Tracks_OTTqp;   //!
   TBranch        *b_Tracks_OTTCxx;   //!
   TBranch        *b_Tracks_OTTCxy;   //!
   TBranch        *b_Tracks_OTTCxtx;   //!
   TBranch        *b_Tracks_OTTCxty;   //!
   TBranch        *b_Tracks_OTTCxqp;   //!
   TBranch        *b_Tracks_OTTCyy;   //!
   TBranch        *b_Tracks_OTTCytx;   //!
   TBranch        *b_Tracks_OTTCyty;   //!
   TBranch        *b_Tracks_OTTCyqp;   //!
   TBranch        *b_Tracks_OTTCtxtx;   //!
   TBranch        *b_Tracks_OTTCtxty;   //!
   TBranch        *b_Tracks_OTTCtxqp;   //!
   TBranch        *b_Tracks_OTTCtyty;   //!
   TBranch        *b_Tracks_OTTCtyqp;   //!
   TBranch        *b_Tracks_OTTCqpqp;   //!
   TBranch        *b_Tracks_OTMeasResidual;   //!
   TBranch        *b_Tracks_OTMeasError;   //!
   TBranch        *b_Tracks_OTMeasType;   //!
   TBranch        *b_Tracks_OTLayer;   //!
   TBranch        *b_Tracks_OTMCParticle;   //!
   TBranch        *b_nMaxSavedMCParticles;   //!
   TBranch        *b_MCParticles_PID;   //!
   TBranch        *b_MCParticles_mother;   //!
   TBranch        *b_MCParticles_momentum;   //!
   TBranch        *b_MCParticles_assocTracks;   //!
   TBranch        *b_MCParticles_assocTrackWeights;   //!
   TBranch        *b_MCParticles_origVtxPos;   //!
   TBranch        *b_MCParticles_4momentum;   //!
   TBranch        *b_MCParticles_nVeloRHits;   //!
   TBranch        *b_MCParticles_nVeloPhiHits;   //!
   TBranch        *b_MCParticles_nVeloHits;   //!
   TBranch        *b_MCParticles_nVeloRStations;   //!
   TBranch        *b_MCParticles_nVeloPhiStations;   //!
   TBranch        *b_MCParticles_nVeloStations;   //!
   TBranch        *b_MCParticles_nTTXHits;   //!
   TBranch        *b_MCParticles_nTTUHits;   //!
   TBranch        *b_MCParticles_nTTVHits;   //!
   TBranch        *b_MCParticles_nTTXStations;   //!
   TBranch        *b_MCParticles_nTTUStations;   //!
   TBranch        *b_MCParticles_nTTVStations;   //!
   TBranch        *b_MCParticles_nTTStations;   //!
   TBranch        *b_MCParticles_nTTStStations;   //!
   TBranch        *b_MCParticles_nTTHits;   //!
   TBranch        *b_MCParticles_nITXHits;   //!
   TBranch        *b_MCParticles_nITUHits;   //!
   TBranch        *b_MCParticles_nITVHits;   //!
   TBranch        *b_MCParticles_nITXStations;   //!
   TBranch        *b_MCParticles_nITUStations;   //!
   TBranch        *b_MCParticles_nITVStations;   //!
   TBranch        *b_MCParticles_nITStations;   //!
   TBranch        *b_MCParticles_nITStStations;   //!
   TBranch        *b_MCParticles_nITHits;   //!
   TBranch        *b_MCParticles_nOTXHits;   //!
   TBranch        *b_MCParticles_nOTUHits;   //!
   TBranch        *b_MCParticles_nOTVHits;   //!
   TBranch        *b_MCParticles_nOTXStations;   //!
   TBranch        *b_MCParticles_nOTUStations;   //!
   TBranch        *b_MCParticles_nOTVStations;   //!
   TBranch        *b_MCParticles_nOTStations;   //!
   TBranch        *b_MCParticles_nOTStStations;   //!
   TBranch        *b_MCParticles_nOTHits;   //!
   TBranch        *b_MCParticles_nTXStations;   //!
   TBranch        *b_MCParticles_nTUStations;   //!
   TBranch        *b_MCParticles_nTVStations;   //!
   TBranch        *b_MCParticles_nTStStations;   //!
   TBranch        *b_MCParticles_nTStations;   //!
   TBranch        *b_MCParticles_HitMaskVeloR;   //!
   TBranch        *b_MCParticles_HitMaskVeloPhi;   //!
   TBranch        *b_MCParticles_HitMaskTT;   //!
   TBranch        *b_MCParticles_HitMaskIT;   //!
   TBranch        *b_MCParticles_HitMaskOT;   //!

   TrackTypeTool(TTree *tree=0);
   virtual ~TrackTypeTool();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef TrackTypeTool_cxx
TrackTypeTool::TrackTypeTool(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("0601-11144100-nt.root");
      if (!f) {
         f = new TFile("0601-11144100-nt.root");
         f->cd("0601-11144100-nt.root:/TrackCheckerNT");
      }
      tree = (TTree*)gDirectory->Get("4242");

   }
   Init(tree);
}

TrackTypeTool::~TrackTypeTool()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t TrackTypeTool::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t TrackTypeTool::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->IsA() != TChain::Class()) return centry;
   TChain *chain = (TChain*)fChain;
   if (chain->GetTreeNumber() != fCurrent) {
      fCurrent = chain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void TrackTypeTool::Init(TTree *tree)
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

   fChain->SetBranchAddress("nMCParticles", &nMCParticles, &b_nMCParticles);
   fChain->SetBranchAddress("nSavedMCParticles", &nSavedMCParticles, &b_nSavedMCParticles);
   fChain->SetBranchAddress("nTracks", &nTracks, &b_nTracks);
   fChain->SetBranchAddress("nSavedTracks", &nSavedTracks, &b_nSavedTracks);
   fChain->SetBranchAddress("nVeloHits", &nVeloHits, &b_nVeloHits);
   fChain->SetBranchAddress("nTTHits", &nTTHits, &b_nTTHits);
   fChain->SetBranchAddress("nITHits", &nITHits, &b_nITHits);
   fChain->SetBranchAddress("nOTHits", &nOTHits, &b_nOTHits);
   fChain->SetBranchAddress("nMaxSavedTracks", &nMaxSavedTracks, &b_nMaxSavedTracks);
   fChain->SetBranchAddress("Tracks_type", Tracks_type, &b_Tracks_type);
   fChain->SetBranchAddress("Tracks_chi2", Tracks_chi2, &b_Tracks_chi2);
   fChain->SetBranchAddress("Tracks_nDoF", Tracks_nDoF, &b_Tracks_nDoF);
   fChain->SetBranchAddress("Tracks_Pchi2", Tracks_Pchi2, &b_Tracks_Pchi2);
   fChain->SetBranchAddress("Tracks_History", Tracks_History, &b_Tracks_History);
   fChain->SetBranchAddress("Tracks_assocMCParts", Tracks_assocMCParts, &b_Tracks_assocMCParts);
   fChain->SetBranchAddress("Tracks_assocMCPartWeights", Tracks_assocMCPartWeights, &b_Tracks_assocMCPartWeights);
   fChain->SetBranchAddress("Tracks_firstState", Tracks_firstState, &b_Tracks_firstState);
   fChain->SetBranchAddress("Tracks_nTotalHits", Tracks_nTotalHits, &b_Tracks_nTotalHits);
   fChain->SetBranchAddress("Tracks_nMCTotalHits", Tracks_nMCTotalHits, &b_Tracks_nMCTotalHits);
   fChain->SetBranchAddress("Tracks_nGoodHits", Tracks_nGoodHits, &b_Tracks_nGoodHits);
   fChain->SetBranchAddress("Tracks_nTotalVelo", Tracks_nTotalVelo, &b_Tracks_nTotalVelo);
   fChain->SetBranchAddress("Tracks_nMCTotalVelo", Tracks_nMCTotalVelo, &b_Tracks_nMCTotalVelo);
   fChain->SetBranchAddress("Tracks_nGoodVelo", Tracks_nGoodVelo, &b_Tracks_nGoodVelo);
   fChain->SetBranchAddress("Tracks_nTotalTT", Tracks_nTotalTT, &b_Tracks_nTotalTT);
   fChain->SetBranchAddress("Tracks_nMCTotalTT", Tracks_nMCTotalTT, &b_Tracks_nMCTotalTT);
   fChain->SetBranchAddress("Tracks_nGoodTT", Tracks_nGoodTT, &b_Tracks_nGoodTT);
   fChain->SetBranchAddress("Tracks_nTotalIT", Tracks_nTotalIT, &b_Tracks_nTotalIT);
   fChain->SetBranchAddress("Tracks_nMCTotalIT", Tracks_nMCTotalIT, &b_Tracks_nMCTotalIT);
   fChain->SetBranchAddress("Tracks_nGoodIT", Tracks_nGoodIT, &b_Tracks_nGoodIT);
   fChain->SetBranchAddress("Tracks_nTotalOT", Tracks_nTotalOT, &b_Tracks_nTotalOT);
   fChain->SetBranchAddress("Tracks_nMCTotalOT", Tracks_nMCTotalOT, &b_Tracks_nMCTotalOT);
   fChain->SetBranchAddress("Tracks_nGoodOT", Tracks_nGoodOT, &b_Tracks_nGoodOT);
   fChain->SetBranchAddress("Tracks_FixedZz", Tracks_FixedZz, &b_Tracks_FixedZz);
   fChain->SetBranchAddress("Tracks_FixedZx", Tracks_FixedZx, &b_Tracks_FixedZx);
   fChain->SetBranchAddress("Tracks_FixedZy", Tracks_FixedZy, &b_Tracks_FixedZy);
   fChain->SetBranchAddress("Tracks_FixedZtx", Tracks_FixedZtx, &b_Tracks_FixedZtx);
   fChain->SetBranchAddress("Tracks_FixedZty", Tracks_FixedZty, &b_Tracks_FixedZty);
   fChain->SetBranchAddress("Tracks_FixedZqp", Tracks_FixedZqp, &b_Tracks_FixedZqp);
   fChain->SetBranchAddress("Tracks_FixedZCxx", Tracks_FixedZCxx, &b_Tracks_FixedZCxx);
   fChain->SetBranchAddress("Tracks_FixedZCxy", Tracks_FixedZCxy, &b_Tracks_FixedZCxy);
   fChain->SetBranchAddress("Tracks_FixedZCxtx", Tracks_FixedZCxtx, &b_Tracks_FixedZCxtx);
   fChain->SetBranchAddress("Tracks_FixedZCxty", Tracks_FixedZCxty, &b_Tracks_FixedZCxty);
   fChain->SetBranchAddress("Tracks_FixedZCxqp", Tracks_FixedZCxqp, &b_Tracks_FixedZCxqp);
   fChain->SetBranchAddress("Tracks_FixedZCyy", Tracks_FixedZCyy, &b_Tracks_FixedZCyy);
   fChain->SetBranchAddress("Tracks_FixedZCytx", Tracks_FixedZCytx, &b_Tracks_FixedZCytx);
   fChain->SetBranchAddress("Tracks_FixedZCyty", Tracks_FixedZCyty, &b_Tracks_FixedZCyty);
   fChain->SetBranchAddress("Tracks_FixedZCyqp", Tracks_FixedZCyqp, &b_Tracks_FixedZCyqp);
   fChain->SetBranchAddress("Tracks_FixedZCtxtx", Tracks_FixedZCtxtx, &b_Tracks_FixedZCtxtx);
   fChain->SetBranchAddress("Tracks_FixedZCtxty", Tracks_FixedZCtxty, &b_Tracks_FixedZCtxty);
   fChain->SetBranchAddress("Tracks_FixedZCtxqp", Tracks_FixedZCtxqp, &b_Tracks_FixedZCtxqp);
   fChain->SetBranchAddress("Tracks_FixedZCtyty", Tracks_FixedZCtyty, &b_Tracks_FixedZCtyty);
   fChain->SetBranchAddress("Tracks_FixedZCtyqp", Tracks_FixedZCtyqp, &b_Tracks_FixedZCtyqp);
   fChain->SetBranchAddress("Tracks_FixedZCqpqp", Tracks_FixedZCqpqp, &b_Tracks_FixedZCqpqp);
   fChain->SetBranchAddress("Tracks_FixedZTx", Tracks_FixedZTx, &b_Tracks_FixedZTx);
   fChain->SetBranchAddress("Tracks_FixedZTy", Tracks_FixedZTy, &b_Tracks_FixedZTy);
   fChain->SetBranchAddress("Tracks_FixedZTtx", Tracks_FixedZTtx, &b_Tracks_FixedZTtx);
   fChain->SetBranchAddress("Tracks_FixedZTty", Tracks_FixedZTty, &b_Tracks_FixedZTty);
   fChain->SetBranchAddress("Tracks_FixedZTqp", Tracks_FixedZTqp, &b_Tracks_FixedZTqp);
   fChain->SetBranchAddress("Tracks_FixedZTCxx", Tracks_FixedZTCxx, &b_Tracks_FixedZTCxx);
   fChain->SetBranchAddress("Tracks_FixedZTCxy", Tracks_FixedZTCxy, &b_Tracks_FixedZTCxy);
   fChain->SetBranchAddress("Tracks_FixedZTCxtx", Tracks_FixedZTCxtx, &b_Tracks_FixedZTCxtx);
   fChain->SetBranchAddress("Tracks_FixedZTCxty", Tracks_FixedZTCxty, &b_Tracks_FixedZTCxty);
   fChain->SetBranchAddress("Tracks_FixedZTCxqp", Tracks_FixedZTCxqp, &b_Tracks_FixedZTCxqp);
   fChain->SetBranchAddress("Tracks_FixedZTCyy", Tracks_FixedZTCyy, &b_Tracks_FixedZTCyy);
   fChain->SetBranchAddress("Tracks_FixedZTCytx", Tracks_FixedZTCytx, &b_Tracks_FixedZTCytx);
   fChain->SetBranchAddress("Tracks_FixedZTCyty", Tracks_FixedZTCyty, &b_Tracks_FixedZTCyty);
   fChain->SetBranchAddress("Tracks_FixedZTCyqp", Tracks_FixedZTCyqp, &b_Tracks_FixedZTCyqp);
   fChain->SetBranchAddress("Tracks_FixedZTCtxtx", Tracks_FixedZTCtxtx, &b_Tracks_FixedZTCtxtx);
   fChain->SetBranchAddress("Tracks_FixedZTCtxty", Tracks_FixedZTCtxty, &b_Tracks_FixedZTCtxty);
   fChain->SetBranchAddress("Tracks_FixedZTCtxqp", Tracks_FixedZTCtxqp, &b_Tracks_FixedZTCtxqp);
   fChain->SetBranchAddress("Tracks_FixedZTCtyty", Tracks_FixedZTCtyty, &b_Tracks_FixedZTCtyty);
   fChain->SetBranchAddress("Tracks_FixedZTCtyqp", Tracks_FixedZTCtyqp, &b_Tracks_FixedZTCtyqp);
   fChain->SetBranchAddress("Tracks_FixedZTCqpqp", Tracks_FixedZTCqpqp, &b_Tracks_FixedZTCqpqp);
   fChain->SetBranchAddress("Tracks_FixedZMeasResidual", Tracks_FixedZMeasResidual, &b_Tracks_FixedZMeasResidual);
   fChain->SetBranchAddress("Tracks_FixedZMeasError", Tracks_FixedZMeasError, &b_Tracks_FixedZMeasError);
   fChain->SetBranchAddress("Tracks_FixedZMeasType", Tracks_FixedZMeasType, &b_Tracks_FixedZMeasType);
   fChain->SetBranchAddress("Tracks_FixedZLayer", Tracks_FixedZLayer, &b_Tracks_FixedZLayer);
   fChain->SetBranchAddress("Tracks_FixedZMCParticle", Tracks_FixedZMCParticle, &b_Tracks_FixedZMCParticle);
   fChain->SetBranchAddress("Tracks_VeloRz", Tracks_VeloRz, &b_Tracks_VeloRz);
   fChain->SetBranchAddress("Tracks_VeloRx", Tracks_VeloRx, &b_Tracks_VeloRx);
   fChain->SetBranchAddress("Tracks_VeloRy", Tracks_VeloRy, &b_Tracks_VeloRy);
   fChain->SetBranchAddress("Tracks_VeloRtx", Tracks_VeloRtx, &b_Tracks_VeloRtx);
   fChain->SetBranchAddress("Tracks_VeloRty", Tracks_VeloRty, &b_Tracks_VeloRty);
   fChain->SetBranchAddress("Tracks_VeloRqp", Tracks_VeloRqp, &b_Tracks_VeloRqp);
   fChain->SetBranchAddress("Tracks_VeloRCxx", Tracks_VeloRCxx, &b_Tracks_VeloRCxx);
   fChain->SetBranchAddress("Tracks_VeloRCxy", Tracks_VeloRCxy, &b_Tracks_VeloRCxy);
   fChain->SetBranchAddress("Tracks_VeloRCxtx", Tracks_VeloRCxtx, &b_Tracks_VeloRCxtx);
   fChain->SetBranchAddress("Tracks_VeloRCxty", Tracks_VeloRCxty, &b_Tracks_VeloRCxty);
   fChain->SetBranchAddress("Tracks_VeloRCxqp", Tracks_VeloRCxqp, &b_Tracks_VeloRCxqp);
   fChain->SetBranchAddress("Tracks_VeloRCyy", Tracks_VeloRCyy, &b_Tracks_VeloRCyy);
   fChain->SetBranchAddress("Tracks_VeloRCytx", Tracks_VeloRCytx, &b_Tracks_VeloRCytx);
   fChain->SetBranchAddress("Tracks_VeloRCyty", Tracks_VeloRCyty, &b_Tracks_VeloRCyty);
   fChain->SetBranchAddress("Tracks_VeloRCyqp", Tracks_VeloRCyqp, &b_Tracks_VeloRCyqp);
   fChain->SetBranchAddress("Tracks_VeloRCtxtx", Tracks_VeloRCtxtx, &b_Tracks_VeloRCtxtx);
   fChain->SetBranchAddress("Tracks_VeloRCtxty", Tracks_VeloRCtxty, &b_Tracks_VeloRCtxty);
   fChain->SetBranchAddress("Tracks_VeloRCtxqp", Tracks_VeloRCtxqp, &b_Tracks_VeloRCtxqp);
   fChain->SetBranchAddress("Tracks_VeloRCtyty", Tracks_VeloRCtyty, &b_Tracks_VeloRCtyty);
   fChain->SetBranchAddress("Tracks_VeloRCtyqp", Tracks_VeloRCtyqp, &b_Tracks_VeloRCtyqp);
   fChain->SetBranchAddress("Tracks_VeloRCqpqp", Tracks_VeloRCqpqp, &b_Tracks_VeloRCqpqp);
   fChain->SetBranchAddress("Tracks_VeloRTx", Tracks_VeloRTx, &b_Tracks_VeloRTx);
   fChain->SetBranchAddress("Tracks_VeloRTy", Tracks_VeloRTy, &b_Tracks_VeloRTy);
   fChain->SetBranchAddress("Tracks_VeloRTtx", Tracks_VeloRTtx, &b_Tracks_VeloRTtx);
   fChain->SetBranchAddress("Tracks_VeloRTty", Tracks_VeloRTty, &b_Tracks_VeloRTty);
   fChain->SetBranchAddress("Tracks_VeloRTqp", Tracks_VeloRTqp, &b_Tracks_VeloRTqp);
   fChain->SetBranchAddress("Tracks_VeloRTCxx", Tracks_VeloRTCxx, &b_Tracks_VeloRTCxx);
   fChain->SetBranchAddress("Tracks_VeloRTCxy", Tracks_VeloRTCxy, &b_Tracks_VeloRTCxy);
   fChain->SetBranchAddress("Tracks_VeloRTCxtx", Tracks_VeloRTCxtx, &b_Tracks_VeloRTCxtx);
   fChain->SetBranchAddress("Tracks_VeloRTCxty", Tracks_VeloRTCxty, &b_Tracks_VeloRTCxty);
   fChain->SetBranchAddress("Tracks_VeloRTCxqp", Tracks_VeloRTCxqp, &b_Tracks_VeloRTCxqp);
   fChain->SetBranchAddress("Tracks_VeloRTCyy", Tracks_VeloRTCyy, &b_Tracks_VeloRTCyy);
   fChain->SetBranchAddress("Tracks_VeloRTCytx", Tracks_VeloRTCytx, &b_Tracks_VeloRTCytx);
   fChain->SetBranchAddress("Tracks_VeloRTCyty", Tracks_VeloRTCyty, &b_Tracks_VeloRTCyty);
   fChain->SetBranchAddress("Tracks_VeloRTCyqp", Tracks_VeloRTCyqp, &b_Tracks_VeloRTCyqp);
   fChain->SetBranchAddress("Tracks_VeloRTCtxtx", Tracks_VeloRTCtxtx, &b_Tracks_VeloRTCtxtx);
   fChain->SetBranchAddress("Tracks_VeloRTCtxty", Tracks_VeloRTCtxty, &b_Tracks_VeloRTCtxty);
   fChain->SetBranchAddress("Tracks_VeloRTCtxqp", Tracks_VeloRTCtxqp, &b_Tracks_VeloRTCtxqp);
   fChain->SetBranchAddress("Tracks_VeloRTCtyty", Tracks_VeloRTCtyty, &b_Tracks_VeloRTCtyty);
   fChain->SetBranchAddress("Tracks_VeloRTCtyqp", Tracks_VeloRTCtyqp, &b_Tracks_VeloRTCtyqp);
   fChain->SetBranchAddress("Tracks_VeloRTCqpqp", Tracks_VeloRTCqpqp, &b_Tracks_VeloRTCqpqp);
   fChain->SetBranchAddress("Tracks_VeloRMeasResidual", Tracks_VeloRMeasResidual, &b_Tracks_VeloRMeasResidual);
   fChain->SetBranchAddress("Tracks_VeloRMeasError", Tracks_VeloRMeasError, &b_Tracks_VeloRMeasError);
   fChain->SetBranchAddress("Tracks_VeloRMeasType", Tracks_VeloRMeasType, &b_Tracks_VeloRMeasType);
   fChain->SetBranchAddress("Tracks_VeloRLayer", Tracks_VeloRLayer, &b_Tracks_VeloRLayer);
   fChain->SetBranchAddress("Tracks_VeloRMCParticle", Tracks_VeloRMCParticle, &b_Tracks_VeloRMCParticle);
   fChain->SetBranchAddress("Tracks_VeloPhiz", Tracks_VeloPhiz, &b_Tracks_VeloPhiz);
   fChain->SetBranchAddress("Tracks_VeloPhix", Tracks_VeloPhix, &b_Tracks_VeloPhix);
   fChain->SetBranchAddress("Tracks_VeloPhiy", Tracks_VeloPhiy, &b_Tracks_VeloPhiy);
   fChain->SetBranchAddress("Tracks_VeloPhitx", Tracks_VeloPhitx, &b_Tracks_VeloPhitx);
   fChain->SetBranchAddress("Tracks_VeloPhity", Tracks_VeloPhity, &b_Tracks_VeloPhity);
   fChain->SetBranchAddress("Tracks_VeloPhiqp", Tracks_VeloPhiqp, &b_Tracks_VeloPhiqp);
   fChain->SetBranchAddress("Tracks_VeloPhiCxx", Tracks_VeloPhiCxx, &b_Tracks_VeloPhiCxx);
   fChain->SetBranchAddress("Tracks_VeloPhiCxy", Tracks_VeloPhiCxy, &b_Tracks_VeloPhiCxy);
   fChain->SetBranchAddress("Tracks_VeloPhiCxtx", Tracks_VeloPhiCxtx, &b_Tracks_VeloPhiCxtx);
   fChain->SetBranchAddress("Tracks_VeloPhiCxty", Tracks_VeloPhiCxty, &b_Tracks_VeloPhiCxty);
   fChain->SetBranchAddress("Tracks_VeloPhiCxqp", Tracks_VeloPhiCxqp, &b_Tracks_VeloPhiCxqp);
   fChain->SetBranchAddress("Tracks_VeloPhiCyy", Tracks_VeloPhiCyy, &b_Tracks_VeloPhiCyy);
   fChain->SetBranchAddress("Tracks_VeloPhiCytx", Tracks_VeloPhiCytx, &b_Tracks_VeloPhiCytx);
   fChain->SetBranchAddress("Tracks_VeloPhiCyty", Tracks_VeloPhiCyty, &b_Tracks_VeloPhiCyty);
   fChain->SetBranchAddress("Tracks_VeloPhiCyqp", Tracks_VeloPhiCyqp, &b_Tracks_VeloPhiCyqp);
   fChain->SetBranchAddress("Tracks_VeloPhiCtxtx", Tracks_VeloPhiCtxtx, &b_Tracks_VeloPhiCtxtx);
   fChain->SetBranchAddress("Tracks_VeloPhiCtxty", Tracks_VeloPhiCtxty, &b_Tracks_VeloPhiCtxty);
   fChain->SetBranchAddress("Tracks_VeloPhiCtxqp", Tracks_VeloPhiCtxqp, &b_Tracks_VeloPhiCtxqp);
   fChain->SetBranchAddress("Tracks_VeloPhiCtyty", Tracks_VeloPhiCtyty, &b_Tracks_VeloPhiCtyty);
   fChain->SetBranchAddress("Tracks_VeloPhiCtyqp", Tracks_VeloPhiCtyqp, &b_Tracks_VeloPhiCtyqp);
   fChain->SetBranchAddress("Tracks_VeloPhiCqpqp", Tracks_VeloPhiCqpqp, &b_Tracks_VeloPhiCqpqp);
   fChain->SetBranchAddress("Tracks_VeloPhiTx", Tracks_VeloPhiTx, &b_Tracks_VeloPhiTx);
   fChain->SetBranchAddress("Tracks_VeloPhiTy", Tracks_VeloPhiTy, &b_Tracks_VeloPhiTy);
   fChain->SetBranchAddress("Tracks_VeloPhiTtx", Tracks_VeloPhiTtx, &b_Tracks_VeloPhiTtx);
   fChain->SetBranchAddress("Tracks_VeloPhiTty", Tracks_VeloPhiTty, &b_Tracks_VeloPhiTty);
   fChain->SetBranchAddress("Tracks_VeloPhiTqp", Tracks_VeloPhiTqp, &b_Tracks_VeloPhiTqp);
   fChain->SetBranchAddress("Tracks_VeloPhiTCxx", Tracks_VeloPhiTCxx, &b_Tracks_VeloPhiTCxx);
   fChain->SetBranchAddress("Tracks_VeloPhiTCxy", Tracks_VeloPhiTCxy, &b_Tracks_VeloPhiTCxy);
   fChain->SetBranchAddress("Tracks_VeloPhiTCxtx", Tracks_VeloPhiTCxtx, &b_Tracks_VeloPhiTCxtx);
   fChain->SetBranchAddress("Tracks_VeloPhiTCxty", Tracks_VeloPhiTCxty, &b_Tracks_VeloPhiTCxty);
   fChain->SetBranchAddress("Tracks_VeloPhiTCxqp", Tracks_VeloPhiTCxqp, &b_Tracks_VeloPhiTCxqp);
   fChain->SetBranchAddress("Tracks_VeloPhiTCyy", Tracks_VeloPhiTCyy, &b_Tracks_VeloPhiTCyy);
   fChain->SetBranchAddress("Tracks_VeloPhiTCytx", Tracks_VeloPhiTCytx, &b_Tracks_VeloPhiTCytx);
   fChain->SetBranchAddress("Tracks_VeloPhiTCyty", Tracks_VeloPhiTCyty, &b_Tracks_VeloPhiTCyty);
   fChain->SetBranchAddress("Tracks_VeloPhiTCyqp", Tracks_VeloPhiTCyqp, &b_Tracks_VeloPhiTCyqp);
   fChain->SetBranchAddress("Tracks_VeloPhiTCtxtx", Tracks_VeloPhiTCtxtx, &b_Tracks_VeloPhiTCtxtx);
   fChain->SetBranchAddress("Tracks_VeloPhiTCtxty", Tracks_VeloPhiTCtxty, &b_Tracks_VeloPhiTCtxty);
   fChain->SetBranchAddress("Tracks_VeloPhiTCtxqp", Tracks_VeloPhiTCtxqp, &b_Tracks_VeloPhiTCtxqp);
   fChain->SetBranchAddress("Tracks_VeloPhiTCtyty", Tracks_VeloPhiTCtyty, &b_Tracks_VeloPhiTCtyty);
   fChain->SetBranchAddress("Tracks_VeloPhiTCtyqp", Tracks_VeloPhiTCtyqp, &b_Tracks_VeloPhiTCtyqp);
   fChain->SetBranchAddress("Tracks_VeloPhiTCqpqp", Tracks_VeloPhiTCqpqp, &b_Tracks_VeloPhiTCqpqp);
   fChain->SetBranchAddress("Tracks_VeloPhiMeasResidual", Tracks_VeloPhiMeasResidual, &b_Tracks_VeloPhiMeasResidual);
   fChain->SetBranchAddress("Tracks_VeloPhiMeasError", Tracks_VeloPhiMeasError, &b_Tracks_VeloPhiMeasError);
   fChain->SetBranchAddress("Tracks_VeloPhiMeasType", Tracks_VeloPhiMeasType, &b_Tracks_VeloPhiMeasType);
   fChain->SetBranchAddress("Tracks_VeloPhiLayer", Tracks_VeloPhiLayer, &b_Tracks_VeloPhiLayer);
   fChain->SetBranchAddress("Tracks_VeloPhiMCParticle", Tracks_VeloPhiMCParticle, &b_Tracks_VeloPhiMCParticle);
   fChain->SetBranchAddress("Tracks_TTz", Tracks_TTz, &b_Tracks_TTz);
   fChain->SetBranchAddress("Tracks_TTx", Tracks_TTx, &b_Tracks_TTx);
   fChain->SetBranchAddress("Tracks_TTy", Tracks_TTy, &b_Tracks_TTy);
   fChain->SetBranchAddress("Tracks_TTtx", Tracks_TTtx, &b_Tracks_TTtx);
   fChain->SetBranchAddress("Tracks_TTty", Tracks_TTty, &b_Tracks_TTty);
   fChain->SetBranchAddress("Tracks_TTqp", Tracks_TTqp, &b_Tracks_TTqp);
   fChain->SetBranchAddress("Tracks_TTCxx", Tracks_TTCxx, &b_Tracks_TTCxx);
   fChain->SetBranchAddress("Tracks_TTCxy", Tracks_TTCxy, &b_Tracks_TTCxy);
   fChain->SetBranchAddress("Tracks_TTCxtx", Tracks_TTCxtx, &b_Tracks_TTCxtx);
   fChain->SetBranchAddress("Tracks_TTCxty", Tracks_TTCxty, &b_Tracks_TTCxty);
   fChain->SetBranchAddress("Tracks_TTCxqp", Tracks_TTCxqp, &b_Tracks_TTCxqp);
   fChain->SetBranchAddress("Tracks_TTCyy", Tracks_TTCyy, &b_Tracks_TTCyy);
   fChain->SetBranchAddress("Tracks_TTCytx", Tracks_TTCytx, &b_Tracks_TTCytx);
   fChain->SetBranchAddress("Tracks_TTCyty", Tracks_TTCyty, &b_Tracks_TTCyty);
   fChain->SetBranchAddress("Tracks_TTCyqp", Tracks_TTCyqp, &b_Tracks_TTCyqp);
   fChain->SetBranchAddress("Tracks_TTCtxtx", Tracks_TTCtxtx, &b_Tracks_TTCtxtx);
   fChain->SetBranchAddress("Tracks_TTCtxty", Tracks_TTCtxty, &b_Tracks_TTCtxty);
   fChain->SetBranchAddress("Tracks_TTCtxqp", Tracks_TTCtxqp, &b_Tracks_TTCtxqp);
   fChain->SetBranchAddress("Tracks_TTCtyty", Tracks_TTCtyty, &b_Tracks_TTCtyty);
   fChain->SetBranchAddress("Tracks_TTCtyqp", Tracks_TTCtyqp, &b_Tracks_TTCtyqp);
   fChain->SetBranchAddress("Tracks_TTCqpqp", Tracks_TTCqpqp, &b_Tracks_TTCqpqp);
   fChain->SetBranchAddress("Tracks_TTTx", Tracks_TTTx, &b_Tracks_TTTx);
   fChain->SetBranchAddress("Tracks_TTTy", Tracks_TTTy, &b_Tracks_TTTy);
   fChain->SetBranchAddress("Tracks_TTTtx", Tracks_TTTtx, &b_Tracks_TTTtx);
   fChain->SetBranchAddress("Tracks_TTTty", Tracks_TTTty, &b_Tracks_TTTty);
   fChain->SetBranchAddress("Tracks_TTTqp", Tracks_TTTqp, &b_Tracks_TTTqp);
   fChain->SetBranchAddress("Tracks_TTTCxx", Tracks_TTTCxx, &b_Tracks_TTTCxx);
   fChain->SetBranchAddress("Tracks_TTTCxy", Tracks_TTTCxy, &b_Tracks_TTTCxy);
   fChain->SetBranchAddress("Tracks_TTTCxtx", Tracks_TTTCxtx, &b_Tracks_TTTCxtx);
   fChain->SetBranchAddress("Tracks_TTTCxty", Tracks_TTTCxty, &b_Tracks_TTTCxty);
   fChain->SetBranchAddress("Tracks_TTTCxqp", Tracks_TTTCxqp, &b_Tracks_TTTCxqp);
   fChain->SetBranchAddress("Tracks_TTTCyy", Tracks_TTTCyy, &b_Tracks_TTTCyy);
   fChain->SetBranchAddress("Tracks_TTTCytx", Tracks_TTTCytx, &b_Tracks_TTTCytx);
   fChain->SetBranchAddress("Tracks_TTTCyty", Tracks_TTTCyty, &b_Tracks_TTTCyty);
   fChain->SetBranchAddress("Tracks_TTTCyqp", Tracks_TTTCyqp, &b_Tracks_TTTCyqp);
   fChain->SetBranchAddress("Tracks_TTTCtxtx", Tracks_TTTCtxtx, &b_Tracks_TTTCtxtx);
   fChain->SetBranchAddress("Tracks_TTTCtxty", Tracks_TTTCtxty, &b_Tracks_TTTCtxty);
   fChain->SetBranchAddress("Tracks_TTTCtxqp", Tracks_TTTCtxqp, &b_Tracks_TTTCtxqp);
   fChain->SetBranchAddress("Tracks_TTTCtyty", Tracks_TTTCtyty, &b_Tracks_TTTCtyty);
   fChain->SetBranchAddress("Tracks_TTTCtyqp", Tracks_TTTCtyqp, &b_Tracks_TTTCtyqp);
   fChain->SetBranchAddress("Tracks_TTTCqpqp", Tracks_TTTCqpqp, &b_Tracks_TTTCqpqp);
   fChain->SetBranchAddress("Tracks_TTMeasResidual", Tracks_TTMeasResidual, &b_Tracks_TTMeasResidual);
   fChain->SetBranchAddress("Tracks_TTMeasError", Tracks_TTMeasError, &b_Tracks_TTMeasError);
   fChain->SetBranchAddress("Tracks_TTMeasType", Tracks_TTMeasType, &b_Tracks_TTMeasType);
   fChain->SetBranchAddress("Tracks_TTLayer", Tracks_TTLayer, &b_Tracks_TTLayer);
   fChain->SetBranchAddress("Tracks_TTMCParticle", Tracks_TTMCParticle, &b_Tracks_TTMCParticle);
   fChain->SetBranchAddress("Tracks_ITz", Tracks_ITz, &b_Tracks_ITz);
   fChain->SetBranchAddress("Tracks_ITx", Tracks_ITx, &b_Tracks_ITx);
   fChain->SetBranchAddress("Tracks_ITy", Tracks_ITy, &b_Tracks_ITy);
   fChain->SetBranchAddress("Tracks_ITtx", Tracks_ITtx, &b_Tracks_ITtx);
   fChain->SetBranchAddress("Tracks_ITty", Tracks_ITty, &b_Tracks_ITty);
   fChain->SetBranchAddress("Tracks_ITqp", Tracks_ITqp, &b_Tracks_ITqp);
   fChain->SetBranchAddress("Tracks_ITCxx", Tracks_ITCxx, &b_Tracks_ITCxx);
   fChain->SetBranchAddress("Tracks_ITCxy", Tracks_ITCxy, &b_Tracks_ITCxy);
   fChain->SetBranchAddress("Tracks_ITCxtx", Tracks_ITCxtx, &b_Tracks_ITCxtx);
   fChain->SetBranchAddress("Tracks_ITCxty", Tracks_ITCxty, &b_Tracks_ITCxty);
   fChain->SetBranchAddress("Tracks_ITCxqp", Tracks_ITCxqp, &b_Tracks_ITCxqp);
   fChain->SetBranchAddress("Tracks_ITCyy", Tracks_ITCyy, &b_Tracks_ITCyy);
   fChain->SetBranchAddress("Tracks_ITCytx", Tracks_ITCytx, &b_Tracks_ITCytx);
   fChain->SetBranchAddress("Tracks_ITCyty", Tracks_ITCyty, &b_Tracks_ITCyty);
   fChain->SetBranchAddress("Tracks_ITCyqp", Tracks_ITCyqp, &b_Tracks_ITCyqp);
   fChain->SetBranchAddress("Tracks_ITCtxtx", Tracks_ITCtxtx, &b_Tracks_ITCtxtx);
   fChain->SetBranchAddress("Tracks_ITCtxty", Tracks_ITCtxty, &b_Tracks_ITCtxty);
   fChain->SetBranchAddress("Tracks_ITCtxqp", Tracks_ITCtxqp, &b_Tracks_ITCtxqp);
   fChain->SetBranchAddress("Tracks_ITCtyty", Tracks_ITCtyty, &b_Tracks_ITCtyty);
   fChain->SetBranchAddress("Tracks_ITCtyqp", Tracks_ITCtyqp, &b_Tracks_ITCtyqp);
   fChain->SetBranchAddress("Tracks_ITCqpqp", Tracks_ITCqpqp, &b_Tracks_ITCqpqp);
   fChain->SetBranchAddress("Tracks_ITTx", Tracks_ITTx, &b_Tracks_ITTx);
   fChain->SetBranchAddress("Tracks_ITTy", Tracks_ITTy, &b_Tracks_ITTy);
   fChain->SetBranchAddress("Tracks_ITTtx", Tracks_ITTtx, &b_Tracks_ITTtx);
   fChain->SetBranchAddress("Tracks_ITTty", Tracks_ITTty, &b_Tracks_ITTty);
   fChain->SetBranchAddress("Tracks_ITTqp", Tracks_ITTqp, &b_Tracks_ITTqp);
   fChain->SetBranchAddress("Tracks_ITTCxx", Tracks_ITTCxx, &b_Tracks_ITTCxx);
   fChain->SetBranchAddress("Tracks_ITTCxy", Tracks_ITTCxy, &b_Tracks_ITTCxy);
   fChain->SetBranchAddress("Tracks_ITTCxtx", Tracks_ITTCxtx, &b_Tracks_ITTCxtx);
   fChain->SetBranchAddress("Tracks_ITTCxty", Tracks_ITTCxty, &b_Tracks_ITTCxty);
   fChain->SetBranchAddress("Tracks_ITTCxqp", Tracks_ITTCxqp, &b_Tracks_ITTCxqp);
   fChain->SetBranchAddress("Tracks_ITTCyy", Tracks_ITTCyy, &b_Tracks_ITTCyy);
   fChain->SetBranchAddress("Tracks_ITTCytx", Tracks_ITTCytx, &b_Tracks_ITTCytx);
   fChain->SetBranchAddress("Tracks_ITTCyty", Tracks_ITTCyty, &b_Tracks_ITTCyty);
   fChain->SetBranchAddress("Tracks_ITTCyqp", Tracks_ITTCyqp, &b_Tracks_ITTCyqp);
   fChain->SetBranchAddress("Tracks_ITTCtxtx", Tracks_ITTCtxtx, &b_Tracks_ITTCtxtx);
   fChain->SetBranchAddress("Tracks_ITTCtxty", Tracks_ITTCtxty, &b_Tracks_ITTCtxty);
   fChain->SetBranchAddress("Tracks_ITTCtxqp", Tracks_ITTCtxqp, &b_Tracks_ITTCtxqp);
   fChain->SetBranchAddress("Tracks_ITTCtyty", Tracks_ITTCtyty, &b_Tracks_ITTCtyty);
   fChain->SetBranchAddress("Tracks_ITTCtyqp", Tracks_ITTCtyqp, &b_Tracks_ITTCtyqp);
   fChain->SetBranchAddress("Tracks_ITTCqpqp", Tracks_ITTCqpqp, &b_Tracks_ITTCqpqp);
   fChain->SetBranchAddress("Tracks_ITMeasResidual", Tracks_ITMeasResidual, &b_Tracks_ITMeasResidual);
   fChain->SetBranchAddress("Tracks_ITMeasError", Tracks_ITMeasError, &b_Tracks_ITMeasError);
   fChain->SetBranchAddress("Tracks_ITMeasType", Tracks_ITMeasType, &b_Tracks_ITMeasType);
   fChain->SetBranchAddress("Tracks_ITLayer", Tracks_ITLayer, &b_Tracks_ITLayer);
   fChain->SetBranchAddress("Tracks_ITMCParticle", Tracks_ITMCParticle, &b_Tracks_ITMCParticle);
   fChain->SetBranchAddress("Tracks_OTz", Tracks_OTz, &b_Tracks_OTz);
   fChain->SetBranchAddress("Tracks_OTx", Tracks_OTx, &b_Tracks_OTx);
   fChain->SetBranchAddress("Tracks_OTy", Tracks_OTy, &b_Tracks_OTy);
   fChain->SetBranchAddress("Tracks_OTtx", Tracks_OTtx, &b_Tracks_OTtx);
   fChain->SetBranchAddress("Tracks_OTty", Tracks_OTty, &b_Tracks_OTty);
   fChain->SetBranchAddress("Tracks_OTqp", Tracks_OTqp, &b_Tracks_OTqp);
   fChain->SetBranchAddress("Tracks_OTCxx", Tracks_OTCxx, &b_Tracks_OTCxx);
   fChain->SetBranchAddress("Tracks_OTCxy", Tracks_OTCxy, &b_Tracks_OTCxy);
   fChain->SetBranchAddress("Tracks_OTCxtx", Tracks_OTCxtx, &b_Tracks_OTCxtx);
   fChain->SetBranchAddress("Tracks_OTCxty", Tracks_OTCxty, &b_Tracks_OTCxty);
   fChain->SetBranchAddress("Tracks_OTCxqp", Tracks_OTCxqp, &b_Tracks_OTCxqp);
   fChain->SetBranchAddress("Tracks_OTCyy", Tracks_OTCyy, &b_Tracks_OTCyy);
   fChain->SetBranchAddress("Tracks_OTCytx", Tracks_OTCytx, &b_Tracks_OTCytx);
   fChain->SetBranchAddress("Tracks_OTCyty", Tracks_OTCyty, &b_Tracks_OTCyty);
   fChain->SetBranchAddress("Tracks_OTCyqp", Tracks_OTCyqp, &b_Tracks_OTCyqp);
   fChain->SetBranchAddress("Tracks_OTCtxtx", Tracks_OTCtxtx, &b_Tracks_OTCtxtx);
   fChain->SetBranchAddress("Tracks_OTCtxty", Tracks_OTCtxty, &b_Tracks_OTCtxty);
   fChain->SetBranchAddress("Tracks_OTCtxqp", Tracks_OTCtxqp, &b_Tracks_OTCtxqp);
   fChain->SetBranchAddress("Tracks_OTCtyty", Tracks_OTCtyty, &b_Tracks_OTCtyty);
   fChain->SetBranchAddress("Tracks_OTCtyqp", Tracks_OTCtyqp, &b_Tracks_OTCtyqp);
   fChain->SetBranchAddress("Tracks_OTCqpqp", Tracks_OTCqpqp, &b_Tracks_OTCqpqp);
   fChain->SetBranchAddress("Tracks_OTTx", Tracks_OTTx, &b_Tracks_OTTx);
   fChain->SetBranchAddress("Tracks_OTTy", Tracks_OTTy, &b_Tracks_OTTy);
   fChain->SetBranchAddress("Tracks_OTTtx", Tracks_OTTtx, &b_Tracks_OTTtx);
   fChain->SetBranchAddress("Tracks_OTTty", Tracks_OTTty, &b_Tracks_OTTty);
   fChain->SetBranchAddress("Tracks_OTTqp", Tracks_OTTqp, &b_Tracks_OTTqp);
   fChain->SetBranchAddress("Tracks_OTTCxx", Tracks_OTTCxx, &b_Tracks_OTTCxx);
   fChain->SetBranchAddress("Tracks_OTTCxy", Tracks_OTTCxy, &b_Tracks_OTTCxy);
   fChain->SetBranchAddress("Tracks_OTTCxtx", Tracks_OTTCxtx, &b_Tracks_OTTCxtx);
   fChain->SetBranchAddress("Tracks_OTTCxty", Tracks_OTTCxty, &b_Tracks_OTTCxty);
   fChain->SetBranchAddress("Tracks_OTTCxqp", Tracks_OTTCxqp, &b_Tracks_OTTCxqp);
   fChain->SetBranchAddress("Tracks_OTTCyy", Tracks_OTTCyy, &b_Tracks_OTTCyy);
   fChain->SetBranchAddress("Tracks_OTTCytx", Tracks_OTTCytx, &b_Tracks_OTTCytx);
   fChain->SetBranchAddress("Tracks_OTTCyty", Tracks_OTTCyty, &b_Tracks_OTTCyty);
   fChain->SetBranchAddress("Tracks_OTTCyqp", Tracks_OTTCyqp, &b_Tracks_OTTCyqp);
   fChain->SetBranchAddress("Tracks_OTTCtxtx", Tracks_OTTCtxtx, &b_Tracks_OTTCtxtx);
   fChain->SetBranchAddress("Tracks_OTTCtxty", Tracks_OTTCtxty, &b_Tracks_OTTCtxty);
   fChain->SetBranchAddress("Tracks_OTTCtxqp", Tracks_OTTCtxqp, &b_Tracks_OTTCtxqp);
   fChain->SetBranchAddress("Tracks_OTTCtyty", Tracks_OTTCtyty, &b_Tracks_OTTCtyty);
   fChain->SetBranchAddress("Tracks_OTTCtyqp", Tracks_OTTCtyqp, &b_Tracks_OTTCtyqp);
   fChain->SetBranchAddress("Tracks_OTTCqpqp", Tracks_OTTCqpqp, &b_Tracks_OTTCqpqp);
   fChain->SetBranchAddress("Tracks_OTMeasResidual", Tracks_OTMeasResidual, &b_Tracks_OTMeasResidual);
   fChain->SetBranchAddress("Tracks_OTMeasError", Tracks_OTMeasError, &b_Tracks_OTMeasError);
   fChain->SetBranchAddress("Tracks_OTMeasType", Tracks_OTMeasType, &b_Tracks_OTMeasType);
   fChain->SetBranchAddress("Tracks_OTLayer", Tracks_OTLayer, &b_Tracks_OTLayer);
   fChain->SetBranchAddress("Tracks_OTMCParticle", Tracks_OTMCParticle, &b_Tracks_OTMCParticle);
   fChain->SetBranchAddress("nMaxSavedMCParticles", &nMaxSavedMCParticles, &b_nMaxSavedMCParticles);
   fChain->SetBranchAddress("MCParticles_PID", MCParticles_PID, &b_MCParticles_PID);
   fChain->SetBranchAddress("MCParticles_mother", MCParticles_mother, &b_MCParticles_mother);
   fChain->SetBranchAddress("MCParticles_momentum", MCParticles_momentum, &b_MCParticles_momentum);
   fChain->SetBranchAddress("MCParticles_assocTracks", MCParticles_assocTracks, &b_MCParticles_assocTracks);
   fChain->SetBranchAddress("MCParticles_assocTrackWeights", MCParticles_assocTrackWeights, &b_MCParticles_assocTrackWeights);
   fChain->SetBranchAddress("MCParticles_origVtxPos", MCParticles_origVtxPos, &b_MCParticles_origVtxPos);
   fChain->SetBranchAddress("MCParticles_4momentum", MCParticles_4momentum, &b_MCParticles_4momentum);
   fChain->SetBranchAddress("MCParticles_nVeloRHits", MCParticles_nVeloRHits, &b_MCParticles_nVeloRHits);
   fChain->SetBranchAddress("MCParticles_nVeloPhiHits", MCParticles_nVeloPhiHits, &b_MCParticles_nVeloPhiHits);
   fChain->SetBranchAddress("MCParticles_nVeloHits", MCParticles_nVeloHits, &b_MCParticles_nVeloHits);
   fChain->SetBranchAddress("MCParticles_nVeloRStations", MCParticles_nVeloRStations, &b_MCParticles_nVeloRStations);
   fChain->SetBranchAddress("MCParticles_nVeloPhiStations", MCParticles_nVeloPhiStations, &b_MCParticles_nVeloPhiStations);
   fChain->SetBranchAddress("MCParticles_nVeloStations", MCParticles_nVeloStations, &b_MCParticles_nVeloStations);
   fChain->SetBranchAddress("MCParticles_nTTXHits", MCParticles_nTTXHits, &b_MCParticles_nTTXHits);
   fChain->SetBranchAddress("MCParticles_nTTUHits", MCParticles_nTTUHits, &b_MCParticles_nTTUHits);
   fChain->SetBranchAddress("MCParticles_nTTVHits", MCParticles_nTTVHits, &b_MCParticles_nTTVHits);
   fChain->SetBranchAddress("MCParticles_nTTXStations", MCParticles_nTTXStations, &b_MCParticles_nTTXStations);
   fChain->SetBranchAddress("MCParticles_nTTUStations", MCParticles_nTTUStations, &b_MCParticles_nTTUStations);
   fChain->SetBranchAddress("MCParticles_nTTVStations", MCParticles_nTTVStations, &b_MCParticles_nTTVStations);
   fChain->SetBranchAddress("MCParticles_nTTStations", MCParticles_nTTStations, &b_MCParticles_nTTStations);
   fChain->SetBranchAddress("MCParticles_nTTStStations", MCParticles_nTTStStations, &b_MCParticles_nTTStStations);
   fChain->SetBranchAddress("MCParticles_nTTHits", MCParticles_nTTHits, &b_MCParticles_nTTHits);
   fChain->SetBranchAddress("MCParticles_nITXHits", MCParticles_nITXHits, &b_MCParticles_nITXHits);
   fChain->SetBranchAddress("MCParticles_nITUHits", MCParticles_nITUHits, &b_MCParticles_nITUHits);
   fChain->SetBranchAddress("MCParticles_nITVHits", MCParticles_nITVHits, &b_MCParticles_nITVHits);
   fChain->SetBranchAddress("MCParticles_nITXStations", MCParticles_nITXStations, &b_MCParticles_nITXStations);
   fChain->SetBranchAddress("MCParticles_nITUStations", MCParticles_nITUStations, &b_MCParticles_nITUStations);
   fChain->SetBranchAddress("MCParticles_nITVStations", MCParticles_nITVStations, &b_MCParticles_nITVStations);
   fChain->SetBranchAddress("MCParticles_nITStations", MCParticles_nITStations, &b_MCParticles_nITStations);
   fChain->SetBranchAddress("MCParticles_nITStStations", MCParticles_nITStStations, &b_MCParticles_nITStStations);
   fChain->SetBranchAddress("MCParticles_nITHits", MCParticles_nITHits, &b_MCParticles_nITHits);
   fChain->SetBranchAddress("MCParticles_nOTXHits", MCParticles_nOTXHits, &b_MCParticles_nOTXHits);
   fChain->SetBranchAddress("MCParticles_nOTUHits", MCParticles_nOTUHits, &b_MCParticles_nOTUHits);
   fChain->SetBranchAddress("MCParticles_nOTVHits", MCParticles_nOTVHits, &b_MCParticles_nOTVHits);
   fChain->SetBranchAddress("MCParticles_nOTXStations", MCParticles_nOTXStations, &b_MCParticles_nOTXStations);
   fChain->SetBranchAddress("MCParticles_nOTUStations", MCParticles_nOTUStations, &b_MCParticles_nOTUStations);
   fChain->SetBranchAddress("MCParticles_nOTVStations", MCParticles_nOTVStations, &b_MCParticles_nOTVStations);
   fChain->SetBranchAddress("MCParticles_nOTStations", MCParticles_nOTStations, &b_MCParticles_nOTStations);
   fChain->SetBranchAddress("MCParticles_nOTStStations", MCParticles_nOTStStations, &b_MCParticles_nOTStStations);
   fChain->SetBranchAddress("MCParticles_nOTHits", MCParticles_nOTHits, &b_MCParticles_nOTHits);
   fChain->SetBranchAddress("MCParticles_nTXStations", MCParticles_nTXStations, &b_MCParticles_nTXStations);
   fChain->SetBranchAddress("MCParticles_nTUStations", MCParticles_nTUStations, &b_MCParticles_nTUStations);
   fChain->SetBranchAddress("MCParticles_nTVStations", MCParticles_nTVStations, &b_MCParticles_nTVStations);
   fChain->SetBranchAddress("MCParticles_nTStStations", MCParticles_nTStStations, &b_MCParticles_nTStStations);
   fChain->SetBranchAddress("MCParticles_nTStations", MCParticles_nTStations, &b_MCParticles_nTStations);
   fChain->SetBranchAddress("MCParticles_HitMaskVeloR", MCParticles_HitMaskVeloR, &b_MCParticles_HitMaskVeloR);
   fChain->SetBranchAddress("MCParticles_HitMaskVeloPhi", MCParticles_HitMaskVeloPhi, &b_MCParticles_HitMaskVeloPhi);
   fChain->SetBranchAddress("MCParticles_HitMaskTT", MCParticles_HitMaskTT, &b_MCParticles_HitMaskTT);
   fChain->SetBranchAddress("MCParticles_HitMaskIT", MCParticles_HitMaskIT, &b_MCParticles_HitMaskIT);
   fChain->SetBranchAddress("MCParticles_HitMaskOT", MCParticles_HitMaskOT, &b_MCParticles_HitMaskOT);
   Notify();
}

Bool_t TrackTypeTool::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normaly not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void TrackTypeTool::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t TrackTypeTool::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef TrackTypeTool_cxx
