///////////////////////////////////////////////////////////////////
//To execute the script type:
// root minuittag.C\+ | tee output/out
//REMEMBER: make a backup copy of tag.opts first !
// ....and wait for optimization of your tagger
//at the end tag.opts will contain as last lines the optimal cuts
//grep VALUE output/out | sort //to check progress
//////////////////////////////////////////////////////////////////

#include <Riostream.h>
#include <iostream>
#include <string>
#include <sstream>
#include "TVirtualFitter.h"
#include "TStopwatch.h"
#include "TString.h"

vector<TString> varnames(0);
vector<Double_t> vstart(0);

void fcn (Int_t &npar,Double_t *gin,Double_t &f,Double_t *par,Int_t iflag){

  //prepare new option file
  int iout;
  TString c="echo ", equal=" = ", quote="\"", add=" >> ", ff="tag.opts"; 
  //append property definition
  cout<<"\n------------"<<endl;
  for (unsigned int i=0; i!=varnames.size(); i++) {
    ostringstream strs;
    strs << par[i];// damn root
    TString value(strs.str());
    TString command = c +quote+varnames.at(i)+equal +value +quote +add+ff;
    cout<<"   command: "<<command<<endl;
    iout = system(command);
  }

  //Run 
  iout = system("make run");

  //read result of this run
  ifstream indata; 
  indata.open("output/logperf.txt"); 
  double eff_mu, eff_ele, eff_kaon, eff_same, eff_vtx, eff_comb;
  indata >> eff_mu >> eff_ele >> eff_kaon >> eff_same >> eff_vtx >> eff_comb;
  indata.close();

  //variable to minimize ///////////////
  f = -eff_kaon;
  ////////////////////////////////////

  cout<<"VALUE = "<<-f<<"   \t"<<gin<<" "<<npar<<" "<<iflag<<endl;
  //exit(0);
}

///////////////////////////////////////////////////////////////
void minuittag() {

  ///////////////////////////////////////////////////////////////
  //remember to also change the f above.

  //Muon OS, eff_mu
  // varnames.push_back("Muon_Pt_cut");   vstart.push_back(1.1);
  // varnames.push_back("Muon_P_cut");    vstart.push_back(0.0);
  // varnames.push_back("Muon_lcs_cut");  vstart.push_back(2.2);
  // varnames.push_back("Muon_PIDm_cut"); vstart.push_back(2.0);

  //Electrons, eff_ele
  // varnames.push_back("Ele_Pt_cut");    vstart.push_back(1.1);
  // varnames.push_back("Ele_P_cut");     vstart.push_back(0.0);
  // varnames.push_back("Ele_lcs_cut");   vstart.push_back(2.5);
  // varnames.push_back("Ele_ghost_cut"); vstart.push_back(-15.0);
  // varnames.push_back("VeloChargeMin"); vstart.push_back(0.0);
  // varnames.push_back("VeloChargeMax"); vstart.push_back(1.3);
  // varnames.push_back("EoverP");        vstart.push_back(0.85);
  // varnames.push_back("Ele_PIDe_cut");  vstart.push_back(4.0);

  //kaon OS, eff_kaon
  varnames.push_back( "Kaon_P_cut" );    vstart.push_back(4);
  varnames.push_back( "Kaon_Pt_cut" );   vstart.push_back(0.4);
  varnames.push_back( "Kaon_IPs_cut" );  vstart.push_back(3.8);
  varnames.push_back( "Kaon_IP_cut" );   vstart.push_back(1.5);
  varnames.push_back( "Kaon_LCS_cut" );  vstart.push_back(2.5);
  varnames.push_back( "Kaon_ipPU_cut" ); vstart.push_back(4);
  varnames.push_back( "Kaon_PIDk" );     vstart.push_back(0.0);
  varnames.push_back( "Kaon_PIDkp" );    vstart.push_back(-1);
  varnames.push_back( "Kaon_ghost_cut" );vstart.push_back(-14);

  //kaon SS, eff_same
  // varnames.push_back( "KaonSame_Pt_cut" );   vstart.push_back(0.45);
  // varnames.push_back( "KaonSame_P_cut" );    vstart.push_back(4.0);
  // varnames.push_back( "KaonSame_IP_cut" );   vstart.push_back(3.0);
  // varnames.push_back( "KaonSame_Phi_cut" );  vstart.push_back(1.1);
  // varnames.push_back( "KaonSame_Eta_cut" );  vstart.push_back(1.0);
  // varnames.push_back( "KaonSame_dQ_cut" );   vstart.push_back(1.6);
  // varnames.push_back( "KaonS_LCS_cut" );     vstart.push_back(2.0);
  // varnames.push_back( "KaonSPID_kS_cut" );   vstart.push_back(1.0);
  // varnames.push_back( "KaonSPID_kpS_cut" );  vstart.push_back(-1.0);

  //Pions SS, eff_same
//   varnames.push_back("PionSame_Pt_cut");    vstart.push_back(0.6);
//   varnames.push_back("PionSame_P_cut");     vstart.push_back(0.0);
//   varnames.push_back("PionSame_IPs_cut");   vstart.push_back(3.5);
//   varnames.push_back("PionS_LCS_cut");      vstart.push_back(2.0);
//   varnames.push_back("PionSame_dQ_cut");    vstart.push_back(3.0);
//   varnames.push_back("PionSame_dQ_extra_cut");vstart.push_back(1.5);
//   varnames.push_back("Pion_ghost_cut");     vstart.push_back(-30.0);
//   varnames.push_back("PionSame_PIDNoK_cut");vstart.push_back(3.0);
//   varnames.push_back("PionSame_PIDNoP_cut");vstart.push_back(10.0);
//   varnames.push_back("PionProbMin");        vstart.push_back(0.55);

  //vertex charge, eff_vtx
  // varnames.push_back("PowerK"); vstart.push_back(0.35);
  // varnames.push_back("MinimumVCharge"); vstart.push_back(0.12);

  ///////////////////////////////////////////////////////////////
  
  // Initialize TMinuit via generic fitter 
  TStopwatch timer; timer.Start();
  TVirtualFitter::SetDefaultFitter("Minuit");
  TVirtualFitter* minuit = TVirtualFitter::Fitter(0, varnames.size());
  minuit->SetFCN(fcn);

  Double_t arglist[100];
  arglist[0] = 1;
  minuit->ExecuteCommand("SET PRINT", arglist, 1);
  //1 e' il numero di parametri che sono riempiti in arglis

  for (unsigned int i=0; i!=varnames.size(); i++) 
    minuit->SetParameter(i, varnames.at(i), 
			 vstart.at(i), fabs(vstart.at(i)/4.),
			 -fabs(vstart.at(i)*10.), fabs(vstart.at(i)*10.) );

  arglist[0] = 3; minuit->ExecuteCommand("CALL FCN", arglist, 1);

  //minuit->FixParameter(0);
  //minuit->ReleaseParameter(0);

  arglist[0] = 1500;
  cout<<"....lets start !"<<endl;
  minuit->ExecuteCommand("SIMPLEX",arglist,0);//MIGRAD,SIMPLEX,HESSE,MINOS

  arglist[0] = 3;
  minuit->ExecuteCommand("CALL FCN", arglist, 1);

  printf("CPUTime at the end of job = %f seconds\n",timer.CpuTime());
  
  //////////////////////////////////////////////////////////
}
