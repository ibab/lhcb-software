#define CorrectVELOSwimInfo_D02PiPi_cxx
#include "CorrectVELOSwimInfo_D02PiPi.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <SwimVELOAcceptance.h>
#include <iostream>
#include <string>

void CorrectVELOSwimInfo_D02PiPi::Loop(std::string outputfile)
{
//   In a ROOT session, you can do:
//      Root > .L CorrectVELOSwimInfo_D02PiPi.C
//      Root > CorrectVELOSwimInfo_D02PiPi t
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
    
    TFile *outFile =TFile::Open(outputfile.c_str(), "recreate");
    TTree *outTree = new TTree(m_tuplename.c_str(),m_tuplename.c_str());
    outTree = fChain->CloneTree(0);

    Long64_t nentries = fChain->GetEntriesFast();

    cout << "Processing chain with " << nentries << " entries" << endl;

    Long64_t nbytes = 0, nb = 0;
    for (Long64_t jentry=0; jentry<nentries;jentry++) {
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        nb = fChain->GetEntry(jentry);   nbytes += nb;
        // if (Cut(ientry) < 0) continue;
        SwimVELOAcceptance* swimd0dauplus = new SwimVELOAcceptance( 1.,10.,3,-600.,600.,
                                                                    D0_ENDVERTEX_X,D0_ENDVERTEX_Y,D0_ENDVERTEX_Z,
                                                                    D0_PX/D0_P,D0_PY/D0_P,D0_PZ/D0_P,
                                                                    dauplus_PX/dauplus_PZ, dauplus_PY/dauplus_PZ);
        std::map< std::string, std::vector< std::pair<double,bool> > > thesetps_dauplus = swimd0dauplus->getTurningPoints();

        SwimVELOAcceptance* swimd0dauminus = new SwimVELOAcceptance( 1.,10.,3,-600.,600.,
                                                                    D0_ENDVERTEX_X,D0_ENDVERTEX_Y,D0_ENDVERTEX_Z,
                                                                    D0_PX/D0_P,D0_PY/D0_P,D0_PZ/D0_P,
                                                                    dauminus_PX/dauminus_PZ, dauminus_PY/dauminus_PZ);
        std::map< std::string, std::vector< std::pair<double,bool> > > thesetps_dauminus = swimd0dauminus->getTurningPoints();

        std::vector< std::pair<double,bool> >::iterator thesetps_dauplus_hlt1iter  = thesetps_dauplus["Hlt1"].begin();
        std::vector< std::pair<double,bool> >::iterator thesetps_dauplus_offliter  = thesetps_dauplus["Offline"].begin();
        std::vector< std::pair<double,bool> >::iterator thesetps_dauminus_hlt1iter = thesetps_dauminus["Hlt1"].begin();
        std::vector< std::pair<double,bool> >::iterator thesetps_dauminus_offliter = thesetps_dauminus["Offline"].begin();

        // The temporary arrays
        Int_t           tmp_D0_Trigger_nTP = 0;
        Int_t           tmp_dauplus_Trigger_nTP = 0;
        Int_t           tmp_dauminus_Trigger_nTP = 0; 
        Float_t         tmp_D0_Trigger_TP_RAW[200];   //[D0_Trigger_nTP]
        Float_t         tmp_D0_Trigger_TP_DEC[200];   //[D0_Trigger_nTP]
        Float_t         tmp_D0_Trigger_TP_TAU[200];   //[D0_Trigger_nTP]
        Float_t         tmp_D0_Trigger_TP_IP[200];   //[D0_Trigger_nTP]
        Float_t         tmp_D0_TP_DEC_Hlt1TrackAllL0Decision[200];   //[D0_nTP]
        Float_t         tmp_D0_TP_DEC_Hlt2CharmHadD02HH_D02PiPiDecision[200];   //[D0_nTP]
        Float_t         tmp_D0_TP_DEC_Hlt2CharmHadD02HH_D02PiPiWideMassDecision[200];   //[D0_nTP]
        Float_t         tmp_dauplus_Trigger_TP_DEC_trRec_HLT1[200];   //[dauplus_Trigger_nTP]
        Float_t         tmp_dauplus_Trigger_TP_DEC_trRec_HLT2[200];   //[dauplus_Trigger_nTP]
        Float_t         tmp_dauplus_Trigger_TP_DEC_trRec_VELO_HLT1[200];   //[dauplus_Trigger_nTP]
        Float_t         tmp_dauplus_Trigger_TP_DEC_trRec_VELO_OFF[200];   //[dauplus_Trigger_nTP]
        Float_t         tmp_dauplus_Trigger_TP_DEC_trTOS_Hlt1TrackAllL0Decision[200];   //[dauplus_Trigger_nTP]
        Float_t         tmp_dauplus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiDecision[200];   //[dauplus_Trigger_nTP]
        Float_t         tmp_dauplus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiWideMassDecision[200];   //[dauplus_Trigger_nTP]
        Float_t         tmp_dauminus_Trigger_TP_DEC_trRec_HLT1[200];   //[dauplus_Trigger_nTP]
        Float_t         tmp_dauminus_Trigger_TP_DEC_trRec_HLT2[200];   //[dauplus_Trigger_nTP]
        Float_t         tmp_dauminus_Trigger_TP_DEC_trRec_VELO_HLT1[200];   //[dauplus_Trigger_nTP]
        Float_t         tmp_dauminus_Trigger_TP_DEC_trRec_VELO_OFF[200];   //[dauplus_Trigger_nTP]
        Float_t         tmp_dauminus_Trigger_TP_DEC_trTOS_Hlt1TrackAllL0Decision[200];   //[dauplus_Trigger_nTP]
        Float_t         tmp_dauminus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiDecision[200];   //[dauplus_Trigger_nTP]
        Float_t         tmp_dauminus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiWideMassDecision[200];   //[dauplus_Trigger_nTP]

        double taupermm = (D0_Trigger_TP_TAU[0] - D0_Trigger_TP_TAU[D0_Trigger_nTP-1])/1200.;    

        for (int thistp = 0; thistp < D0_Trigger_nTP; ++ thistp) {

                if (thistp == 0) { //First and last TP are a special case

                        tmp_D0_Trigger_TP_RAW[tmp_D0_Trigger_nTP] = D0_Trigger_TP_RAW[thistp];   
                        tmp_D0_Trigger_TP_DEC[tmp_D0_Trigger_nTP] = D0_Trigger_TP_DEC[thistp];   
                        tmp_D0_Trigger_TP_TAU[tmp_D0_Trigger_nTP] = D0_Trigger_TP_TAU[thistp];   
                        tmp_D0_Trigger_TP_IP[tmp_D0_Trigger_nTP]  = D0_Trigger_TP_IP[thistp];   
                        tmp_D0_TP_DEC_Hlt1TrackAllL0Decision[tmp_D0_Trigger_nTP] = 
                            D0_TP_DEC_Hlt1TrackAllL0Decision[thistp];   
                        tmp_D0_TP_DEC_Hlt2CharmHadD02HH_D02PiPiDecision[tmp_D0_Trigger_nTP] = 
                            D0_TP_DEC_Hlt2CharmHadD02HH_D02PiPiDecision[thistp];   
                        tmp_D0_TP_DEC_Hlt2CharmHadD02HH_D02PiPiWideMassDecision[tmp_D0_Trigger_nTP] = 
                            D0_TP_DEC_Hlt2CharmHadD02HH_D02PiPiWideMassDecision[thistp];   

                        tmp_dauplus_Trigger_TP_DEC_trRec_HLT1[tmp_D0_Trigger_nTP] = 
                            dauplus_Trigger_TP_DEC_trRec_HLT1[thistp];   
                        tmp_dauplus_Trigger_TP_DEC_trRec_HLT2[tmp_D0_Trigger_nTP] = 
                            dauplus_Trigger_TP_DEC_trRec_HLT2[thistp];   
                        tmp_dauplus_Trigger_TP_DEC_trTOS_Hlt1TrackAllL0Decision[tmp_D0_Trigger_nTP] = 
                            dauplus_Trigger_TP_DEC_trTOS_Hlt1TrackAllL0Decision[thistp];   
                        tmp_dauplus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiDecision[tmp_D0_Trigger_nTP] = 
                            dauplus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiDecision[thistp];   
                        tmp_dauplus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiWideMassDecision[tmp_D0_Trigger_nTP] = 
                            dauplus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiWideMassDecision[thistp];   

                        tmp_dauminus_Trigger_TP_DEC_trRec_HLT1[tmp_D0_Trigger_nTP] = 
                            dauminus_Trigger_TP_DEC_trRec_HLT1[thistp];   
                        tmp_dauminus_Trigger_TP_DEC_trRec_HLT2[tmp_D0_Trigger_nTP] = 
                            dauminus_Trigger_TP_DEC_trRec_HLT2[thistp];   
                        tmp_dauminus_Trigger_TP_DEC_trTOS_Hlt1TrackAllL0Decision[tmp_D0_Trigger_nTP] = 
                            dauminus_Trigger_TP_DEC_trTOS_Hlt1TrackAllL0Decision[thistp];   
                        tmp_dauminus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiDecision[tmp_D0_Trigger_nTP] = 
                            dauminus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiDecision[thistp];   
                        tmp_dauminus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiWideMassDecision[tmp_D0_Trigger_nTP] = 
                            dauminus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiWideMassDecision[thistp];   

                        tmp_dauplus_Trigger_TP_DEC_trRec_VELO_HLT1[tmp_D0_Trigger_nTP] = (*thesetps_dauplus_hlt1iter).second; 
                        tmp_dauplus_Trigger_TP_DEC_trRec_VELO_OFF[tmp_D0_Trigger_nTP]  = (*thesetps_dauplus_offliter).second;
                
                        tmp_dauminus_Trigger_TP_DEC_trRec_VELO_HLT1[tmp_D0_Trigger_nTP] = (*thesetps_dauminus_hlt1iter).second;
                        tmp_dauminus_Trigger_TP_DEC_trRec_VELO_OFF[tmp_D0_Trigger_nTP]  = (*thesetps_dauminus_offliter).second;
                
                        // Increase the number of TPs
                        tmp_D0_Trigger_nTP += 1; tmp_dauplus_Trigger_nTP += 1; tmp_dauminus_Trigger_nTP += 1; 
                } else {
                        // Now the question is, which tp are we at?
                        // Who comes first, a track, or the existing TP? If a track, which one?
                        bool processed = false;
                        while (!processed) {
                                //Find the first point
                                if (    (D0_Trigger_TP_RAW[thistp] <= (*(thesetps_dauplus_hlt1iter+1)).first) &&
                                        (D0_Trigger_TP_RAW[thistp] <= (*(thesetps_dauminus_hlt1iter+1)).first) &&
                                        (D0_Trigger_TP_RAW[thistp] <= (*(thesetps_dauplus_offliter+1)).first) &&
                                        (D0_Trigger_TP_RAW[thistp] <= (*(thesetps_dauminus_offliter+1)).first) ) {
                                        processed = true;
                                } else {
                                        //Decide which iterator to increment
                                        //
                                        if (    (*(thesetps_dauplus_hlt1iter+1)).first < (*(thesetps_dauminus_hlt1iter+1)).first && 
                                                (*(thesetps_dauplus_hlt1iter+1)).first < (*(thesetps_dauplus_offliter+1)).first && 
                                                (*(thesetps_dauplus_hlt1iter+1)).first < (*(thesetps_dauminus_offliter+1)).first ) {
                                                ++thesetps_dauplus_hlt1iter;
                                                tmp_D0_Trigger_TP_RAW[tmp_D0_Trigger_nTP] = (*thesetps_dauplus_hlt1iter).first; 
                                        } else if (     (*(thesetps_dauminus_hlt1iter+1)).first < (*(thesetps_dauplus_offliter+1)).first && 
                                                        (*(thesetps_dauminus_hlt1iter+1)).first < (*(thesetps_dauminus_offliter+1)).first ) {
                                                ++thesetps_dauminus_hlt1iter;
                                                tmp_D0_Trigger_TP_RAW[tmp_D0_Trigger_nTP] = (*thesetps_dauminus_hlt1iter).first; 
                                        } else if (     (*(thesetps_dauplus_offliter+1)).first < (*(thesetps_dauminus_offliter+1)).first ) {
                                                ++thesetps_dauplus_offliter;
                                                tmp_D0_Trigger_TP_RAW[tmp_D0_Trigger_nTP] = (*thesetps_dauplus_offliter).first;
                                        } else {
                                                ++thesetps_dauminus_offliter;
                                                tmp_D0_Trigger_TP_RAW[tmp_D0_Trigger_nTP] = (*thesetps_dauminus_offliter).first;
                                        }
                                        //
                                        tmp_D0_Trigger_TP_DEC[tmp_D0_Trigger_nTP] = D0_Trigger_TP_DEC[thistp-1];   
                                        tmp_D0_Trigger_TP_TAU[tmp_D0_Trigger_nTP] = D0_Trigger_TP_TAU[0] + taupermm*(-600.-tmp_D0_Trigger_TP_RAW[tmp_D0_Trigger_nTP]);//D0_Trigger_TP_TAU[thistp-1];   
                                        tmp_D0_Trigger_TP_IP[tmp_D0_Trigger_nTP]  = D0_Trigger_TP_IP[thistp-1];   
                                        tmp_D0_TP_DEC_Hlt1TrackAllL0Decision[tmp_D0_Trigger_nTP] = 
                                            D0_TP_DEC_Hlt1TrackAllL0Decision[thistp-1];   
                                        tmp_D0_TP_DEC_Hlt2CharmHadD02HH_D02PiPiDecision[tmp_D0_Trigger_nTP] = 
                                            D0_TP_DEC_Hlt2CharmHadD02HH_D02PiPiDecision[thistp-1];   
                                        tmp_D0_TP_DEC_Hlt2CharmHadD02HH_D02PiPiWideMassDecision[tmp_D0_Trigger_nTP] = 
                                            D0_TP_DEC_Hlt2CharmHadD02HH_D02PiPiWideMassDecision[thistp-1];   

                                        tmp_dauplus_Trigger_TP_DEC_trRec_HLT1[tmp_D0_Trigger_nTP] = 
                                            dauplus_Trigger_TP_DEC_trRec_HLT1[thistp-1];   
                                        tmp_dauplus_Trigger_TP_DEC_trRec_HLT2[tmp_D0_Trigger_nTP] = 
                                            dauplus_Trigger_TP_DEC_trRec_HLT2[thistp-1];   
                                        tmp_dauplus_Trigger_TP_DEC_trTOS_Hlt1TrackAllL0Decision[tmp_D0_Trigger_nTP] = 
                                            dauplus_Trigger_TP_DEC_trTOS_Hlt1TrackAllL0Decision[thistp-1];   
                                        tmp_dauplus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiDecision[tmp_D0_Trigger_nTP] = 
                                            dauplus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiDecision[thistp-1];   
                                        tmp_dauplus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiWideMassDecision[tmp_D0_Trigger_nTP] = 
                                            dauplus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiWideMassDecision[thistp-1];   

                                        tmp_dauminus_Trigger_TP_DEC_trRec_HLT1[tmp_D0_Trigger_nTP] = 
                                            dauminus_Trigger_TP_DEC_trRec_HLT1[thistp-1];   
                                        tmp_dauminus_Trigger_TP_DEC_trRec_HLT2[tmp_D0_Trigger_nTP] = 
                                            dauminus_Trigger_TP_DEC_trRec_HLT2[thistp-1];   
                                        tmp_dauminus_Trigger_TP_DEC_trTOS_Hlt1TrackAllL0Decision[tmp_D0_Trigger_nTP] = 
                                            dauminus_Trigger_TP_DEC_trTOS_Hlt1TrackAllL0Decision[thistp-1];   
                                        tmp_dauminus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiDecision[tmp_D0_Trigger_nTP] = 
                                            dauminus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiDecision[thistp-1];   
                                        tmp_dauminus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiWideMassDecision[tmp_D0_Trigger_nTP] = 
                                            dauminus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiWideMassDecision[thistp-1];   

                                        tmp_dauplus_Trigger_TP_DEC_trRec_VELO_HLT1[tmp_D0_Trigger_nTP] = (*thesetps_dauplus_hlt1iter).second;
                                        tmp_dauplus_Trigger_TP_DEC_trRec_VELO_OFF[tmp_D0_Trigger_nTP]  = (*thesetps_dauplus_offliter).second;
                
                                        tmp_dauminus_Trigger_TP_DEC_trRec_VELO_HLT1[tmp_D0_Trigger_nTP] = (*thesetps_dauminus_hlt1iter).second;
                                        tmp_dauminus_Trigger_TP_DEC_trRec_VELO_OFF[tmp_D0_Trigger_nTP]  = (*thesetps_dauminus_offliter).second;
                
                                        // Increase the number of TPs
                                        tmp_D0_Trigger_nTP += 1; tmp_dauplus_Trigger_nTP += 1; tmp_dauminus_Trigger_nTP += 1; 
                                }
                        }
                        tmp_D0_Trigger_TP_RAW[tmp_D0_Trigger_nTP] = D0_Trigger_TP_RAW[thistp];   
                        tmp_D0_Trigger_TP_DEC[tmp_D0_Trigger_nTP] = D0_Trigger_TP_DEC[thistp];   
                        tmp_D0_Trigger_TP_TAU[tmp_D0_Trigger_nTP] = D0_Trigger_TP_TAU[thistp];   
                        tmp_D0_Trigger_TP_IP[tmp_D0_Trigger_nTP]  = D0_Trigger_TP_IP[thistp];   
                        tmp_D0_TP_DEC_Hlt1TrackAllL0Decision[tmp_D0_Trigger_nTP] = 
                            D0_TP_DEC_Hlt1TrackAllL0Decision[thistp];   
                        tmp_D0_TP_DEC_Hlt2CharmHadD02HH_D02PiPiDecision[tmp_D0_Trigger_nTP] = 
                            D0_TP_DEC_Hlt2CharmHadD02HH_D02PiPiDecision[thistp];   
                        tmp_D0_TP_DEC_Hlt2CharmHadD02HH_D02PiPiWideMassDecision[tmp_D0_Trigger_nTP] = 
                            D0_TP_DEC_Hlt2CharmHadD02HH_D02PiPiWideMassDecision[thistp];   

                        tmp_dauplus_Trigger_TP_DEC_trRec_HLT1[tmp_D0_Trigger_nTP] = 
                            dauplus_Trigger_TP_DEC_trRec_HLT1[thistp];   
                        tmp_dauplus_Trigger_TP_DEC_trRec_HLT2[tmp_D0_Trigger_nTP] = 
                            dauplus_Trigger_TP_DEC_trRec_HLT2[thistp];   
                        tmp_dauplus_Trigger_TP_DEC_trTOS_Hlt1TrackAllL0Decision[tmp_D0_Trigger_nTP] = 
                            dauplus_Trigger_TP_DEC_trTOS_Hlt1TrackAllL0Decision[thistp];   
                        tmp_dauplus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiDecision[tmp_D0_Trigger_nTP] = 
                            dauplus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiDecision[thistp];   
                        tmp_dauplus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiWideMassDecision[tmp_D0_Trigger_nTP] = 
                            dauplus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiWideMassDecision[thistp];   

                        tmp_dauminus_Trigger_TP_DEC_trRec_HLT1[tmp_D0_Trigger_nTP] = dauminus_Trigger_TP_DEC_trRec_HLT1[thistp];   
                        tmp_dauminus_Trigger_TP_DEC_trRec_HLT2[tmp_D0_Trigger_nTP] = dauminus_Trigger_TP_DEC_trRec_HLT2[thistp];   
                        tmp_dauminus_Trigger_TP_DEC_trTOS_Hlt1TrackAllL0Decision[tmp_D0_Trigger_nTP] = 
                            dauminus_Trigger_TP_DEC_trTOS_Hlt1TrackAllL0Decision[thistp];   
                        tmp_dauminus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiDecision[tmp_D0_Trigger_nTP] = 
                            dauminus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiDecision[thistp];   
                        tmp_dauminus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiWideMassDecision[tmp_D0_Trigger_nTP] = 
                            dauminus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiWideMassDecision[thistp];   

                        tmp_dauplus_Trigger_TP_DEC_trRec_VELO_HLT1[tmp_D0_Trigger_nTP] = (*thesetps_dauplus_hlt1iter).second;
                        tmp_dauplus_Trigger_TP_DEC_trRec_VELO_OFF[tmp_D0_Trigger_nTP]  = (*thesetps_dauplus_offliter).second;
                
                        tmp_dauminus_Trigger_TP_DEC_trRec_VELO_HLT1[tmp_D0_Trigger_nTP] = (*thesetps_dauminus_hlt1iter).second;
                        tmp_dauminus_Trigger_TP_DEC_trRec_VELO_OFF[tmp_D0_Trigger_nTP]  = (*thesetps_dauminus_offliter).second;
                
                        // Increase the number of TPs
                        tmp_D0_Trigger_nTP += 1; tmp_dauplus_Trigger_nTP += 1; tmp_dauminus_Trigger_nTP += 1; 

                }
                
      }
      for (int thistptofill = 0; thistptofill < tmp_D0_Trigger_nTP; ++thistptofill) {
                D0_Trigger_TP_RAW[thistptofill] = tmp_D0_Trigger_TP_RAW[thistptofill];   
                D0_Trigger_TP_DEC[thistptofill] = tmp_D0_Trigger_TP_DEC[thistptofill];   
                D0_Trigger_TP_TAU[thistptofill] = tmp_D0_Trigger_TP_TAU[thistptofill];   
                D0_Trigger_TP_IP[thistptofill]  = tmp_D0_Trigger_TP_IP[thistptofill];   
                D0_TP_DEC_Hlt1TrackAllL0Decision[thistptofill] = 
                    tmp_D0_TP_DEC_Hlt1TrackAllL0Decision[thistptofill];   
                D0_TP_DEC_Hlt2CharmHadD02HH_D02PiPiDecision[thistptofill] = 
                    tmp_D0_TP_DEC_Hlt2CharmHadD02HH_D02PiPiDecision[thistptofill];   
                D0_TP_DEC_Hlt2CharmHadD02HH_D02PiPiWideMassDecision[thistptofill] = 
                    tmp_D0_TP_DEC_Hlt2CharmHadD02HH_D02PiPiWideMassDecision[thistptofill];   

                dauplus_Trigger_TP_DEC_trRec_HLT1[thistptofill] = 
                    tmp_dauplus_Trigger_TP_DEC_trRec_HLT1[thistptofill];   
                dauplus_Trigger_TP_DEC_trRec_HLT2[thistptofill] = 
                    tmp_dauplus_Trigger_TP_DEC_trRec_HLT2[thistptofill];   
                dauplus_Trigger_TP_DEC_trTOS_Hlt1TrackAllL0Decision[thistptofill] = 
                    tmp_dauplus_Trigger_TP_DEC_trTOS_Hlt1TrackAllL0Decision[thistptofill];   
                dauplus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiDecision[thistptofill] = 
                    tmp_dauplus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiDecision[thistptofill];   
                dauplus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiWideMassDecision[thistptofill] = 
                    tmp_dauplus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiWideMassDecision[thistptofill];   

                dauminus_Trigger_TP_DEC_trRec_HLT1[thistptofill] = 
                    tmp_dauminus_Trigger_TP_DEC_trRec_HLT1[thistptofill];   
                dauminus_Trigger_TP_DEC_trRec_HLT2[thistptofill] = 
                    tmp_dauminus_Trigger_TP_DEC_trRec_HLT2[thistptofill];   
                dauminus_Trigger_TP_DEC_trTOS_Hlt1TrackAllL0Decision[thistptofill] = 
                    tmp_dauminus_Trigger_TP_DEC_trTOS_Hlt1TrackAllL0Decision[thistptofill];   
                dauminus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiDecision[thistptofill] = 
                    tmp_dauminus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiDecision[thistptofill];   
                dauminus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiWideMassDecision[thistptofill] = 
                    tmp_dauminus_Trigger_TP_DEC_trTOS_Hlt2CharmHadD02HH_D02PiPiWideMassDecision[thistptofill];   

                dauplus_Trigger_TP_DEC_trRec_VELO_HLT1[thistptofill] = 
                    tmp_dauplus_Trigger_TP_DEC_trRec_VELO_HLT1[thistptofill];
                dauplus_Trigger_TP_DEC_trRec_VELO_OFF[thistptofill]  = 
                    tmp_dauplus_Trigger_TP_DEC_trRec_VELO_OFF[thistptofill];
        
                dauminus_Trigger_TP_DEC_trRec_VELO_HLT1[thistptofill] = 
                    tmp_dauminus_Trigger_TP_DEC_trRec_VELO_HLT1[thistptofill];
                dauminus_Trigger_TP_DEC_trRec_VELO_OFF[thistptofill]  = 
                    tmp_dauminus_Trigger_TP_DEC_trRec_VELO_OFF[thistptofill];
        
      }
      // Increase the number of TPs
      D0_Trigger_nTP = tmp_D0_Trigger_nTP;
      dauplus_Trigger_nTP = tmp_dauplus_Trigger_nTP;
      dauminus_Trigger_nTP = tmp_dauminus_Trigger_nTP;
      outTree->Fill();

    }

    cout << "Finished!" << endl;
    outTree->Write();
    outFile->Write();
}
