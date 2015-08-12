#include "Mint/Histo_BW.h"
#include "Mint/CLHEPPhysicalConstants.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TH2D.h"
#include "TH1D.h"
#include "Mint/Utils.h"
#include "Mint/DalitzEventList.h"
#include "Mint/phaseSpaceIntegrals.h"
#include <cmath>
#include <complex>

using namespace std;
using namespace MINT;


double Histo_BW::GofM(){

    if(_runningWidthHist==0)return mumsWidth();
    
    double ps_m  = _runningWidthHist->Interpolate(mumsRecoMass2()/(GeV*GeV));
    double ps_m0 = _runningWidthHist->Interpolate(mumsMass()*mumsMass()/(GeV*GeV));
    
    double ps_ratio = 0.;
    if(ps_m0>0)ps_ratio= ps_m/ps_m0 ;
    
    return mumsWidth() * ps_ratio;
}

TH1D* Histo_BW::get_histo(TFile* f, const std::string& hname){
    TH1D* h=0;
    if(0 != f){
        //cout << " got file ptr: " << f << endl;
        h = (TH1D*) f->Get(hname.c_str());
        if(0 != h){
            cout << "Found running width histogram in file " << f->GetName() << "." << endl;
        }else{
            cout << "ERROR: Couldn't find running width histogram in file " << f->GetName() << ". I'll return a constant width." << endl;
        }
    }
    else {
        cout << "Didn't find running width file for " << (BW_BW::resonanceProperties()->nameFromPid(abs(mumsPID()))) 
        << ". I'll produce it now assuming a flat 3 body phase space." << endl;
        h= producePhaseSpaceHist();
        if(0 == h) cout << "ERROR: Couldn't produce running width histogram, I'll return a constant width" << endl;
    }
    return h;
}

Double_t phaseSpace(Double_t *x, Double_t *par){

    //x[0]: mumsRecoMass()^2
    //par[0]: mumsMass()
    //par[i]: final state masses (i=1,2,3)
    
    PhaseSpaceIntegral3body ps;
    double ps_val = ps.getVal(sqrt(x[0])*GeV,par[1],par[2],par[3]);
    double ps0_val =  ps.getVal(par[0],par[1],par[2],par[3]);
    
    double ps_ratio=0.;
    if(ps0_val>0)ps_ratio= ps_val/ps0_val;
    //cout << ps_ratio << endl;
    
    return ps_ratio * par[0]/(sqrt(x[0])*GeV);
}

TH1D* Histo_BW::producePhaseSpaceHist(){
        
    std::vector<int> asi = _theDecay.getVal().asi();
    double min = 0.;//getEvent()->eventPattern().sijMin(asi)/(GeV*GeV)*0.75;
    double max = mumsMass()*mumsMass()/(GeV*GeV)*5.;//getEvent()->eventPattern().sijMax(asi)/(GeV*GeV)*1.25;

    TFile* f= new TFile(("RunningWidth_"+ (BW_BW::resonanceProperties()->nameFromPid(abs(mumsPID()))) + ".root").c_str(),"RECREATE");  
    TH1D* h= new TH1D("RunningWidth","RunningWidth",1000,min,max); 
    
    TF1 *ps = new TF1("phaseSpace",phaseSpace,0,max,4);
    std::vector<const AssociatedDecayTreeItem*> adti = _theDecay.finalState(); 
    //cout << adti[0]->mass() << " " <<  adti[1]->mass() << " " <<  adti[2]->mass() << endl;
    ps->SetParameters(mumsMass(),adti[0]->mass(),adti[1]->mass(),adti[2]->mass());

    for (int i=1; i<= h->GetNbinsX(); i++) {
        h->SetBinContent(i,ps->Eval(h->GetXaxis()->GetBinCenter(i)));    
    }  
    
    h->Write();
    f->Write();  
    
    return h;

}




//
