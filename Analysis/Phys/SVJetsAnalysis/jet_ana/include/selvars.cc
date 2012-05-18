#include "selvars.h"

#ifdef __MAKECINT__ 
#pragma link C++ class vector<vector<double> >+; 
#endif

selvars::selvars(TTree *tree, int isMC) {
   _tree = tree;
   _entries = _tree->GetEntries();

   _debug = 5;
   _isMC = isMC;

   if(_isMC) { cout << "Monte Carlo" << endl; }
   else { cout << "DATA" << endl; }


   SingleIsoMuon15 = new TES();
   JetID = new TES();
   noselection = new TES();
   DiMuonHighPt = new TES();
   StripWmuHighPt_JetID = new TES();
   NeutMuqq = new TES();

};//selvars::selvars

void selvars::SetTree() {

  if(_debug>3) cout << "setting tree" << endl;

  _tree->SetBranchStatus("*",0);

  setbranch("NMaxjets",NMaxjets);
  setbranch("stdjet_px",stdjet_px);
  setbranch("stdjet_py",stdjet_py);
  setbranch("stdjet_pz",stdjet_pz);
  setbranch("stdjet_e",stdjet_e);
  setbranch("stdjet_bestpvz",stdjet_bestpvz);
  setbranch("stdjet_n90",stdjet_n90);
  setbranch("stdjet_mtf",stdjet_mtf);
  setbranch("stdjet_Ntopotriggers",stdjet_Ntopotriggers);
  setbranch("stdjet_N_HasPVInfo",stdjet_N_HasPVInfo);

  setbranch("NMaxFatjets",NMaxFatjets);
  setbranch("fatjet_px",fatjet_px);
  setbranch("fatjet_py",fatjet_py);
  setbranch("fatjet_pz",fatjet_pz);
  setbranch("fatjet_e",fatjet_e);
  setbranch("fatjet_bestpvz",fatjet_bestpvz);

  setbranch("NMaxMuons",NMaxMuons);
  setbranch("stdloosemuon_px",stdloosemuon_px);
  setbranch("stdloosemuon_py",stdloosemuon_py);
  setbranch("stdloosemuon_pz",stdloosemuon_pz);
  setbranch("stdloosemuon_e",stdloosemuon_e);
  setbranch("stdloosemuon_pdgid",stdloosemuon_pdgid);
  setbranch("stdloosemuon_bestpvz",stdloosemuon_bestpvz);

}; 

void selvars::GetEntry(int i) {
   int begin_time = 0;
   if(i==0)int begin_time = clock()/CLOCKS_PER_SEC;
   if(!_tree) { cout << "No Tree!" << endl; }
   if(i%(_entries/100)==0) {
	   int time_elapsed = (clock()/CLOCKS_PER_SEC) - begin_time ;
	   float time_per_percent = 100.01*i/_entries > 0 ? time_elapsed / (100.01*i/_entries) : 1e-10;
	   cout << "\rProcessed " << i << " events or "  << int(100.01*i/_entries)  << " %  Elapsed time: " << time_elapsed << " Time per percent : " << time_per_percent << " Estimated time left(min): " << time_per_percent * (100 - int(100.01*i/_entries)) /60. << flush;
   }
   _tree->GetEntry(i);
  // SetVariables();
};

//void selvars::SetVariables(){SetVariables(truep);};

//ClearVariables();
void selvars::FillVars_noselection(){

	if(_debug>3) cout << "FillVars_noselection:" << endl;
	if(noselection->stdjets.size() > 0) cout << "Warning -> noselection->Stdjets already filled " << endl;
	if(_debug>3) cout << "Filling noselection->stdjets" << endl;
	FillParticlesVector( noselection->stdjets, stdjet_px,stdjet_py,stdjet_pz,stdjet_e,stdjet_pdgid,NMaxjets);
	if(_debug>3) cout << "Filling Aditional Info noselection->stdjets" << endl;
	FillAditionalInfo( noselection->stdjets, stdjet_n90, stdjet_mtf, stdjet_bestpvz, stdjet_Ntopotriggers, stdjet_N_HasPVInfo, NMaxjets);

	if(_debug>3) cout << "Filling StdMuons noselection" << endl;
	FillParticlesVector(noselection->stdmuons, stdloosemuon_px,stdloosemuon_py,stdloosemuon_pz,stdloosemuon_e,stdloosemuon_pdgid,NMaxMuons);
	if(_debug>3) cout << "Filling StdMuons noselection best pv_z" << endl;
	Fillbpvz(noselection->stdmuons, stdloosemuon_bestpvz,NMaxMuons);

	if(_debug>3) cout << "Filling noselection->fatjets" << endl;
	FillParticlesVector( noselection->fatjets, fatjet_px,fatjet_py,fatjet_pz,fatjet_e,NMaxFatjets);
	Fillbpvz(noselection->fatjets, fatjet_bestpvz,NMaxFatjets);
	//if(_debug>3) cout << "Sorting Them By Pt" << endl;
	//sort(noselection->stdmuons.begin(),noselection->stdmuons.end(),order_by_pt);
	//sort(noselection->stdjets.begin(),noselection->stdjets.end(),order_by_pt);

	if(_debug>1)if(noselection->fatjets.size() < 0.5) cout << "Warning: No Fatjet on the Event" << endl;

	if(_debug>3) cout << "Def Aditional Variables (lmuon, lDiMuon, ...)" << endl;
	noselection->DefVars();
	if(_debug>4) cout << "Jet Size End of fiallvars_noselection : " << noselection->stdjets.size() << endl;
	if(_debug>4) cout << "Muon Size End of fiallvars_noselection : " << noselection->stdmuons.size() << endl;

}

bool selvars::FillVars_JetID(){

for (int i = 0 ; i < noselection->stdjets.size() ; i++) if(JetIDCut(noselection->stdjets[i])){ JetID->stdjets.push_back(noselection->stdjets[i]); JetID->stdmuons = noselection->stdmuons ;}

if(JetID->stdjets.size() < 1)return false;
JetID->DefVars();
return true;

}

bool selvars::FillVars_StripWmuHighPt_JetID(){

if(noselection->stdmuons.size() < 1)return false;
if(noselection->stdjets.size() < 1)return false;

Particle &lmu = *(noselection->stdmuons[0]);

if(lmu.Pt() < 15000)return false;
if(JetID->stdjets.size() < 1)return false;

StripWmuHighPt_JetID->stdmuons = noselection->stdmuons;
StripWmuHighPt_JetID->stdjets = JetID->stdjets;
StripWmuHighPt_JetID->fatjets = noselection->fatjets;

StripWmuHighPt_JetID->DefVars();

return true;
}

bool selvars::FillVars_NeutMuqq(){

if(StripWmuHighPt_JetID->stdmuons.size() * StripWmuHighPt_JetID->stdjets.size() == 0) return false;

if(StripWmuHighPt_JetID->stdjets.size() < 1.5)return false;

TES * vars = StripWmuHighPt_JetID;

if(vars->fatjets.size() < 0.5)return false;

if(vars->stdmuons.size() > 0.5){
	Particle *lmu = vars->stdmuons[0];
	Particle *ljet = 0;
	Particle *nljet = 0;

	int i = 0;
	for(; i < vars->stdjets.size() ; i++)if(Abs(lmu->bpvz - vars->stdjets[i]->bpvz) < 1e-6){ljet = vars->stdjets[i]; break;}

	for(int j = i+1 ; j < vars->stdjets.size() ; j++)if(Abs(lmu->bpvz - vars->stdjets[j]->bpvz) < 1e-6){nljet = vars->stdjets[j]; break;}


        //if(ljet == 0 || nljet == 0)return false;

	//Particle *fatj = 0;
	//for(int j = 0 ; j < vars->fatjets.size() ; j++)if(Abs(lmu->bpvz - vars->fatjets[j]->bpvz) < 1e-6){fatj = vars->fatjets[j]; break;}
	//if(fatj == 0)return false;

	//if(ljet->Pt() < 15000)return false;
	//if(ljet->momentum.DeltaPhi(nljet->momentum) > 1.5) return false;
 
	//if(fatj->momentum.DeltaR(nljet->momentum) > 1.) return false;
	//if(fatj->momentum.DeltaPhi(ljet->momentum) > 1.) return false;
}

NeutMuqq->stdmuons = StripWmuHighPt_JetID->stdmuons;
NeutMuqq->stdjets = StripWmuHighPt_JetID->stdjets;
NeutMuqq->fatjets = noselection->fatjets;

NeutMuqq->DefVars();

return true;
}

bool selvars::FillVars_DiMuonHighPt(){
	if(_debug>4) cout << "FillVars_DiMuonHighPt" << endl;
	if(_debug>4) cout << "Muon Size End of fiallvars_DiMuonHighPt : " << noselection->stdmuons.size() << endl;
	if(_debug>4) cout << "Muon Size End of fiallvars_DiMuonHighPt : " << (noselection->stdmuons.size() > 1) << endl;

	if(noselection->stdmuons.size() > 1) {

	if(_debug>4) cout << "Declaring lmu and nlmu" << endl;
	Particle *lmu = noselection->stdmuons[0];
	Particle *nlmu = 0;
	for(int i =1 ; i < noselection->stdmuons.size() ; i++){if(Abs(lmu->bpvz - noselection->stdmuons[i]->bpvz) < 1e-6)if(lmu->pdgid * noselection->stdmuons[i]->pdgid < 0){nlmu = noselection->stdmuons[i]; break;};}
	if(nlmu == 0)return false;

	if(_debug>4) cout << "PtCut" << endl;
	if(lmu->Pt() < 20000 || nlmu->Pt() < 20000)return false;
	//if(lmu->Pt() < 15000)return false;
	Particle Z0Cand(lmu->momentum + nlmu->momentum);
	//Z0 window
	if(Abs(Z0Cand.M() - 90000) > 30000)return false;

	if(_debug>4) cout << "Comparing bpvz of the two leading Muons" << endl;
	if(Abs(lmu->bpvz - nlmu->bpvz) < 1e-6) if( lmu->pdgid * nlmu->pdgid < 0){
		if(_debug>4) cout << "Pushing back selected muon1" << endl;
		DiMuonHighPt->stdmuons.push_back(lmu) ;
		if(_debug>4) cout << "Pushing back selected muon2" << endl;
		DiMuonHighPt->stdmuons.push_back(nlmu) ;}
	else return false;
	}
	else return false;

	
        if(_debug > 4) cout << "Size of stdmuons below 2" << endl;
	if(_debug>4) cout << "Muon Size inside DiMuonHighPt(2) : " << DiMuonHighPt->stdmuons.size() << endl;
	//if(DiMuonHighPt->stdmuons.size() <= 1)return false;

	DiMuonHighPt->stdjets = JetID->stdjets;

	if(_debug>4) cout << "End FillVars_DiMuonHighPt" << endl;
	DiMuonHighPt->DefVars();
	if(_debug>4) cout << "End FillVars_DiMuonHighPt" << endl;
	return true;

}

bool selvars::FillVars_SingleIsoMuon15(){

if(_debug>4) cout << "Stdjets size " << noselection->stdjets.size() << endl;
if(_debug>4) cout << "Stdmuons size " << noselection->stdmuons.size() << endl;
//require isolated muon and remove muon-jet from stdjets
if(noselection->stdmuons.size() < 1) return false;
Particle& lmuon = *(noselection->stdmuons[0]);
//if(lmuon == NULL) return false;
if(lmuon.Pt()<15000) return false;
int imuonjet=-1; float tmpdr=0.5;
for (int i = 0 ; i < noselection->stdjets.size() ; i++) if(lmuon.momentum.DeltaR(noselection->stdjets[i]->momentum)<tmpdr)
{ tmpdr = lmuon.momentum.DeltaR(noselection->stdjets[i]->momentum); imuonjet = i;}
if(imuonjet<0) {cout << "Warning! Muon was not reco as a jet? Reject event!" << endl; return false;} 
if(Abs(lmuon.momentum.Pt() - noselection->stdjets[imuonjet]->momentum.Pt()) > 2000) return false;
float evtpvz = noselection->stdjets[imuonjet]->bpvz;

for (int i = 0 ; i < noselection->stdjets.size() ; i++){

	if(i!=imuonjet && noselection->stdjets[i]->bpvz != evtpvz){
		SingleIsoMuon15->stdjets.push_back(noselection->stdjets[i]);
		SingleIsoMuon15->stdmuons = noselection->stdmuons;
	}

	//stdjets.erase(stdjets.begin()+imuonjet);
	//for (int i = 0 ; i < stdjets.size() ; i++) while(i < stdjets.size() ? fabs(stdjets[i]->bpvz - evtpvz) > 1e-3 : false) stdjets.erase(stdjets.begin()+i);

}

SingleIsoMuon15->DefVars();
//for (int i = 0 ; i < stdjets.size() ; i++) while(i < stdjets.size() ? !jet_cut(stdjets[i]) : false) stdjets.erase(stdjets.begin()+i);

return true;

}

void selvars::Clear(){

if(_debug>4) cout << "Cleaning JetID TES " << endl;
JetID->ClearVariables();
if(_debug>4) cout << "SingleIsoMuon15 JetID TES " << endl;
SingleIsoMuon15->ClearVariables();
if(_debug>4) cout << "Cleaning StripWmuHighPt_JetID TES " << endl;
StripWmuHighPt_JetID->ClearVariables();
if(_debug>4) cout << "Cleaning NeutMuqq TES " << endl;
NeutMuqq->ClearVariables();
if(_debug>4) cout << "Cleaning DiMuonHighPt TES " << endl;
DiMuonHighPt->ClearVariables();
if(_debug>4) cout << "Cleaning Noselection TES " << endl;
noselection->ClearVariables_noselection();
if(_debug>4) cout << "All Clean " << endl;

}
/*bool selvars::SetVariables(bool (*jet_cut)(Particle *), int evt_type)
{
if(_debug>4) cout << "Stdjets size " << stdjets.size() << endl;
if(_debug>4) cout << "Stdmuons size " << stdmuons.size() << endl;

if(evt_type==1) { 
//require isolated muon and remove muon-jet from stdjets
	if(stdmuons.size() < 1) return false;
        if(lmuon->Pt()<15000) return false;
	int imuonjet=-1; float tmpdr=0.5;
	for (int i = 0 ; i < stdjets.size() ; i++) if(lmuon->momentum.DeltaR(stdjets[i]->momentum)<tmpdr)
        { tmpdr = lmuon->momentum.DeltaR(stdjets[i]->momentum); imuonjet = i;}
        if(imuonjet<0) {cout << "Warning! Muon was not reco as a jet? Reject event!" << endl; return false;} 
	if(lmuon->momentum.Pt()-stdjets[imuonjet]->momentum.Pt() > 2000) return false;
        float evtpvz = stdjets[imuonjet]->bpvz;
	stdjets.erase(stdjets.begin()+imuonjet);
	for (int i = 0 ; i < stdjets.size() ; i++) while(i < stdjets.size() ? fabs(stdjets[i]->bpvz - evtpvz) > 1e-3 : false) stdjets.erase(stdjets.begin()+i);
        
        
}

//for (int i = 0 ; i < stdjets.size() ; i++) while(i < stdjets.size() ? !jet_cut(stdjets[i]) : false) stdjets.erase(stdjets.begin()+i);

DefVars();

return true;
}
*/
/*
sort(stdjets.begin(),stdjets.end(),order_by_pt);
sort(stdmuons.begin(),stdmuons.end(),order_by_pt);
if(stdjets.size())ljet = stdjets[0];
if(stdjets.size()>1)nljet = stdjets[1];


if(stdjets.size()>1 && ljet->bpvz == nljet->bpvz)lDiJet = new Particle(ljet->momentum+nljet->momentum);
if(stdjets.size()>1 && stdmuons.size() > 0 && ljet->bpvz == nljet->bpvz && ljet->bpvz == lmuon->bpvz)lDiJetMu = new Particle(ljet->momentum+nljet->momentum+lmuon->momentum);

if(stdjets.size()>1)for (int i = 0 ; i<stdjets.size() ; i++)for (int j = i+1 ; j<stdjets.size() ; j++ ){
	if(stdjets[i]->bpvz != stdjets[j]->bpvz)continue;
	Particle*p = new Particle(stdjets[i]->momentum+stdjets[j]->momentum);
	p->jet_daughters_vector.push_back(stdjets[i]);
	p->jet_daughters_vector.push_back(stdjets[j]);
	sort(p->jet_daughters_vector.begin(),p->jet_daughters_vector.end(),order_by_pt);
	DiJet_samePV.push_back( p ) ;
}

if(DiJet_samePV.size())for (int i = 0 ; i<DiJet_samePV.size() ; i++)for (int j = 0 ; j<stdmuons.size() ; j++ ){
	if(DiJet_samePV[i]->bpvz != stdmuons[j]->bpvz)continue;
	Particle *p = new Particle(DiJet_samePV[i]->momentum+stdmuons[j]->momentum);
	p->jet_daughters_vector.push_back(DiJet_samePV[i]->jet_daughters_vector[0]);
	p->jet_daughters_vector.push_back(DiJet_samePV[i]->jet_daughters_vector[1]);
	p->muon_daughters_vector.push_back(stdmuons[j]);
	DiJetMu_samePV.push_back(p);
}

return true;

} // SetVariables
*/
