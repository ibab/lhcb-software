#include "evtsel.h"
#include <stdlib.h>

evtsel::evtsel(TString name, TFile *file, TFile *outputfile, int isMC) {

  TString treename = "PFJetsMC/StdPFJets";
  if(!(TTree*)file->Get(treename)) treename = "PFJets/StdPFJets";
  if(!(TTree*)file->Get(treename)) {cout << "Did not find TTree" << endl; exit(-1);}

  _intree = (TTree*)file->Get(treename);
  _vars = new selvars(_intree, isMC);
  _vars->_isMC = isMC;
  //outputfile->cd(); _vars->_tree_tmva = new TTree("tmva","tmva");

  ReadConfigFile(name,_vars);
  _vars->SetTree();

  noselection = new histograms(outputfile,"noselection",isMC, _vars->_debug);
  JetID = new histograms(outputfile,"JetID",isMC, _vars->_debug);
  IsoMuon15 = new histograms(outputfile,"IsoMuon15",isMC, _vars->_debug);
  DiMuonHighPt = new histograms(outputfile,"DiMuonHighPt",isMC, _vars->_debug);
  StripWmuHighPt_JetID = new histograms(outputfile,"StripWmuHighPt_JetID",isMC, _vars->_debug);
  NeutMuqq = new histograms(outputfile,"NeutMuqq",isMC, _vars->_debug);

  if(_vars->_debug>3) cout << "END: evtsel::evtsel()" << endl;
};

bool evtsel::loop() {

  Long64_t entries = _intree->GetEntries();
  cout << "Tree has " << entries << " entries" << endl;
  if(_vars->_events>0){ cout << "Number of events limited to " << _vars->_events << endl; system("time");}
  

  for(int j=0;j<entries;j++) {
    if(_vars->_events>0 && j>=_vars->_events) break;
    if(_vars->_debug>1) { cout << endl << "Entry " << j << endl; }
    _vars->GetEntry(j);

    //_vars->Clear();
    if(_vars->_debug>1) { cout << endl << "Noselection started" << j << endl; }
    _vars->FillVars_noselection();
    //passVeto(1,0,1.,10.,"Pre-evtsel"); //just to include pre-evtsel in the cutflow
    noselection->FillHistos(_vars->noselection);
    //_vars->Clear();

    //_vars->SetVariables(JetIDCut);
    //passVeto(1,0,1.,10.,"Pre-evtsel"); //just to include pre-evtsel in the cutflow
    if(_vars->_debug>1) { cout << endl << "JetID started" << j << endl; }
    if(_vars->FillVars_JetID())JetID->FillHistos(_vars->JetID);
    //_vars->Clear();

    if(_vars->_debug>1) { cout << endl << "StripWmuHighPt_JetID started" << j << endl; }
    if(_vars->FillVars_StripWmuHighPt_JetID())StripWmuHighPt_JetID->FillHistos(_vars->StripWmuHighPt_JetID);

    if(_vars->_debug>1) { cout << endl << "NeutMuqq started" << j << endl; }
    if(_vars->FillVars_NeutMuqq())NeutMuqq->FillHistos(_vars->NeutMuqq);

    if(_vars->_debug>1) { cout << endl << "SingleIsoMuon15 started" << j << endl; }
    //if(_vars->FillVars_SingleIsoMuon15()) IsoMuon15->FillHistos(_vars->SingleIsoMuon15);

    if(_vars->_debug>1) { cout << endl << "DiMuonHighPt started" << j << endl; }
    if(_vars->FillVars_DiMuonHighPt()) DiMuonHighPt->FillHistos(_vars->DiMuonHighPt);

    if(_vars->_debug>4) cout << "Clearing All Variables of All Analysis " << endl;

    _vars->Clear();

  }
  return true; 
};


void evtsel::finish() {
  //_vars->_tmpTree->Delete();
  _intree->Delete();
};

bool evtsel::passVarCut(Float_t min, Float_t max, Float_t var, TString varname)
{
  if(max>1.e7 && min<-1.e7) return true; //no cut to be applied
  //if(_vars->_debug>3) cout << "Will apply cut " << varname << endl;
  bool passmin = false; bool passmax = false;
  if(max> 1.e7) { passmax = true; } // no cut
  if(min<-1.e7) { passmin = true; } // no cut
  if(max<1.e7 && var<=max) {
    passmax = true;
    TString cutname(varname+Form(" <= %1.2f",max));
    Int_t bin = cutflow[evttype]->GetXaxis()->FindBin(cutname);
    if(bin==-1) for(int b=1;b<100;b++) if(cutflow[evttype]->GetBinContent(b)==0) {
      bin=b; cutflow[evttype]->GetXaxis()->SetBinLabel(bin,cutname); cutflow_w[evttype]->GetXaxis()->SetBinLabel(bin,cutname);
      break;
    }
    //if(_vars->_debug>4) cout << "Passed cut max " << cutname << endl;
    cutflow[evttype]->Fill(bin);
    //cutflow_w[evttype]->Fill(bin,_vars->eventWeight);
  }
  if(min>-1.e7 && var>=min) {
    passmin = true;
    TString cutname(varname+Form(" >= %1.2f",min));
    Int_t bin = cutflow[evttype]->GetXaxis()->FindBin(cutname);
    if(bin==-1) for(int b=1;b<100;b++) if(cutflow[evttype]->GetBinContent(b)==0) {
      bin=b; cutflow[evttype]->GetXaxis()->SetBinLabel(bin,cutname); cutflow_w[evttype]->GetXaxis()->SetBinLabel(bin,cutname);
      break;
    }
    //if(_vars->_debug>4) cout << "Passed cut min " << cutname << endl;
    cutflow[evttype]->Fill(bin);
    //cutflow_w[evttype]->Fill(bin,_vars->eventWeight);
  }
  bool pass = passmin && passmax;
  TString cutname(varname+Form(" is %1.2f want >= %1.2f & <= %1.2f",var,min,max));
  //if(_vars->_debug>4 && pass ) cout << "Passed cut " << cutname << endl;
  //if(_vars->_debug>4 && !pass ) cout << "Failed cut " << cutname << endl;
//  if(!pass) { cout << "Failed " << cutname << endl; }
  return pass;
}; //passVarCut

bool evtsel::passRequirement(bool isreq, bool ishas, Float_t weight, TString reqname)
{
  //if(_vars->_debug>3) cout << "Will apply req " << reqname << endl;
  if(!isreq) return true;
  if(!ishas) return false;
  //if(_vars->_isMC) _vars->eventWeight *= weight;
  Int_t bin = cutflow[evttype]->GetXaxis()->FindBin(reqname);
  if(bin==-1) for(int b=1;b<100;b++) if(cutflow[evttype]->GetBinContent(b)==0) {
    bin=b; cutflow[evttype]->GetXaxis()->SetBinLabel(bin,reqname); cutflow_w[evttype]->GetXaxis()->SetBinLabel(bin,reqname);
    break;
  }
  cutflow[evttype]->Fill(bin);
  //cutflow_w[evttype]->Fill(bin,_vars->eventWeight);
  //if(_vars->_debug>3) cout << "Passed" << endl;
  return true;
}; //passRequirement

bool evtsel::passVeto(bool isveto, bool ishas, Float_t weight, Float_t rnd, TString vetoname)
{
  if(!isveto) return true;
/*  if(_vars->_isMC) if(ishas && rnd < weight) return false;
  if(!_vars->_isMC) if(ishas) return false;
  if(vetoname=="Pre-evtsel") { // do something special so we can have the total preevtsel number in the cutflow table
     Int_t bin = cutflow[2]->GetXaxis()->FindBin(vetoname);
     if(bin==-1) for(int b=1;b<100;b++) if(cutflow[2]->GetBinContent(b)==0) {
	bin=b; cutflow[2]->GetXaxis()->SetBinLabel(bin,vetoname); cutflow_w[2]->GetXaxis()->SetBinLabel(bin,vetoname);
	break;
     }
     cutflow[2]->Fill(bin);
     cutflow_w[2]->Fill(bin,_vars->eventWeight);

     bin = cutflow[3]->GetXaxis()->FindBin(vetoname);
     if(bin==-1) for(int b=1;b<100;b++) if(cutflow[3]->GetBinContent(b)==0) {
	bin=b; cutflow[3]->GetXaxis()->SetBinLabel(bin,vetoname); cutflow_w[3]->GetXaxis()->SetBinLabel(bin,vetoname);
	break;
     }
     cutflow[3]->Fill(bin);
     cutflow_w[3]->Fill(bin,_vars->eventWeight);
  } else {
     Int_t bin = cutflow[evttype]->GetXaxis()->FindBin(vetoname);
     if(bin==-1) for(int b=1;b<100;b++) if(cutflow[evttype]->GetBinContent(b)==0) {
	bin=b; cutflow[evttype]->GetXaxis()->SetBinLabel(bin,vetoname); cutflow_w[evttype]->GetXaxis()->SetBinLabel(bin,vetoname);
	break;
     }
     cutflow[evttype]->Fill(bin);
     cutflow_w[evttype]->Fill(bin,_vars->eventWeight);
  }
*/
  return true;
}; //passVeto

void evtsel::ReadConfigFile(TString name, selvars*& vs)
{
  TEnv *base = new TEnv("./configfiles/evtsel_base.cfg");
  TEnv *env = new TEnv(name);
//Note: all min values should be -1e8 and max values 1e8

  vs->_debug          		= env->GetValue("Debug", base->GetValue("Debug", 0));
  vs->_events         		= env->GetValue("Events", base->GetValue("Events",-1));
  vs->_type         		= env->GetValue("Type", base->GetValue("Type","dummy"));

  env->Print();

}; //ReadConfigFile
