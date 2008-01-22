#include "iostream.h"
#include "HitResiduals.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TStyle.h"



void preparePlots(TChain* theChain, int res[5], TH1F* pull, TH1F* residual){

  
// make the HitResiduals object
HitResiduals t(theChain);

//
// Loop over entries
//

   Int_t nentries = Int_t(t.fChain->GetEntriesFast());

   for (Int_t jentry=0; jentry<nentries;jentry++) {
     // 
     // Load in the event
     //
     Int_t ientry = t.LoadTree(jentry);
     if (ientry < 0) break;
     t.fChain->GetEntry(jentry);  
   
     //Velo residuals
     if (res[0] == 0){
       for (int i=0; i<t.nVelo; i++){
	 if ((res[1] == -1 || res[1] == t.Velo_rType[i]) &&
	     (res[2] == -1 || res[2] == t.Velo_sensor[i])){
	   residual->Fill(t.Velo_res[i]);
	   pull->Fill(t.Velo_pull[i]);
	 }
       }
       continue;
     }
     
      //TT residuals
      if (res[0] == 1){
	for (int i=0; i<t.nTT; i++){
	  if ((res[1] == -1 || res[1] == t.TT_station[i]) &&
	      (res[2] == -1 || res[2] == t.TT_layer[i])   &&
	      (res[3] == -1 || res[3] == t.TT_region[i])  &&
	      (res[4] == -1 || res[4] == t.TT_sector[i])){
	    residual->Fill(t.TT_res[i]);
	    pull->Fill(t.TT_pull[i]);
	  }
	}
	continue;
      }
      
      //OT residuals
      if (res[0] == 2){	
	for (int i=0; i<t.nOT; i++){
	  if ((res[1] == -1 || res[1] == t.OT_station[i]) &&
	      (res[2] == -1 || res[2] == t.OT_layer[i])   &&
	      (res[3] == -1 || res[3] == t.OT_quarter[i])  &&
	      (res[4] == -1 || res[4] == t.OT_module[i])){
	    residual->Fill(t.OT_res[i]);
	    pull->Fill(t.OT_pull[i]);
	  }
	}
	continue;
      }
      
      //IT residuals
      if (res[0] == 3){ 	
	for (int i=0; i<t.nIT; i++){
	  if ((res[1] == -1 || res[1] == t.IT_station[i]) &&
	      (res[2] == -1 || res[2] == t.IT_layer[i])   &&
	      (res[3] == -1 || res[3] == t.IT_box[i])     &&
	      (res[4] == -1 || res[4] == t.IT_sector[i])){
	    residual->Fill(t.IT_res[i]);
	    pull->Fill(t.IT_pull[i]);
	  }
	}
	continue;
      }
   }

 
}

void loop(TChain* theChain1, TChain* theChain2, int res[5]) {
  
  TH1F* pull1;
  TH1F* residual1; 

  if (res[0] == 0){
     std::string title = "Velo Residual ";
     residual1 = new TH1F("residual1", "Velo Residual ",101,-0.3,0.3); 
     pull1 = new TH1F("pull1","Velo Pull ",101,-4,4); 
   }
   if (res[0] == 1){
     residual1 = new TH1F("residual1","TT Residual",101,-1,1); 
     pull1 = new TH1F("pull1","TT Pull",101,-4,4); 
   }
   if (res[0] == 2){
     residual1 = new TH1F("residual1","OT Residual",101,-1,1);  
     pull1 = new TH1F("pull1","OT Pull",101,-4,4); 
   }
   if (res[0] == 3){
     residual1 = new TH1F("residual1","IT Residual",101,-0.5,0.5);
     pull1 = new TH1F("pull1","IT Pull",101,-4,4); 
   }

   residual1->SetXTitle("mm");

  preparePlots(theChain1, res, pull1, residual1);
  
  
  TH1F* pull2;
  TH1F* residual2;
    if (res[0] == 0){
     std::string title = "Velo Residual ";
     residual2 = new TH1F("residual2", "Velo Residual ",101,-0.3,0.3); 
     pull2 = new TH1F("pull2","Velo Pull ",101,-4,4); 
   }
   if (res[0] == 1){
     residual2 = new TH1F("residual2","TT Residual",101,-1,1); 
     pull2 = new TH1F("pull2","TT Pull",101,-4,4); 
   }
   if (res[0] == 2){
     residual2 = new TH1F("residual2","OT Residual",101,-1,1);  
     pull2 = new TH1F("pull2","OT Pull",101,-4,4); 
   }
   if (res[0] == 3){
     residual2 = new TH1F("residual2","IT Residual",101,-0.5,0.5);
     pull2 = new TH1F("pull2","IT Pull",101,-4,4); 
   }

   residual2->SetXTitle("mm");

  preparePlots(theChain2, res, pull2, residual2);

  gROOT->SetStyle("Plain");
  TStyle *style = gROOT->GetStyle("Plain");
  style->SetOptFit(111);

  
  TCanvas* c1 = new TCanvas("c1","c1",800,600);
  c1->Divide(1,2);
  c1->cd(1);
  residual1->Draw();
  residual2->Draw("Same");
  residual2->SetLineColor(kRed);
  c1->cd(2);
  //pull1->Fit("gaus");
   pull1->Draw();
   pull2->Draw("same");
   pull2->SetLineColor(kRed);



}
  

  

  
