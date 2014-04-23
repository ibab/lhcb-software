//
#include "KKpipiEfficiency.h"

#include <iostream>
#include "TH1F.h"
#include "TFile.h"

#include "Mint/IDalitzEventAccess.h"
#include "Mint/CLHEPSystemOfUnits.h"

using namespace std;
using namespace MINT;

KKpipiEfficiency::KKpipiEfficiency(const std::string& fname
				   , IDalitzEventAccess* daddy) 
  : DalitzEventAccess(daddy)
  , _hKFast(0), _hKSlow(0)
  , _hPiFast(0), _hPiSlow(0)
  , _pKFast(-9999), _pKSlow(-9999)
  , _pPiFast(-9999), _pPiSlow(-9999)
{
  TFile* f = new TFile(fname.c_str());
  _hKFast   = get_h(f, "fast_kaonR");
  _hKSlow   = get_h(f, "slow_kaonR");
  _hPiFast  = get_h(f, "fast_pionR");
  _hPiSlow  = get_h(f, "slow_pionR");
}

TH1F* KKpipiEfficiency::get_h(TFile* f, const std::string& hname){
  TH1F* h=0;
  if(0 == f) return 0;
  cout << " got file ptr: " << f << endl;
  if(0 != f){
    h = (TH1F*) f->Get(hname.c_str());
    if(0 != h){
      cout << "got efficiency histogram " 
	   << hname << "." << endl;
    }else{
      cout << "ERROR didn't get efficiency histogram " 
	   << hname << "!!!" << endl;
    }
  }
  return h;
}

void KKpipiEfficiency::resetP(){
  _pKFast = _pKSlow = _pPiFast = _pPiSlow = -9999;
}
bool KKpipiEfficiency::getP(){
  bool useCMFrame=true;
  
  if(0 == getEvent()) return false;
  DalitzEventPattern pat = getEvent()->eventPattern();
  counted_ptr<IDalitzEvent> counted_evtPtr(0);
  IDalitzEvent* evtPtr=0;
  if(useCMFrame){
    TVector3 p0(0,0,0);
    counted_evtPtr = counted_ptr<IDalitzEvent>(new DalitzEvent(getEvent()));
    evtPtr = counted_evtPtr.get();
    evtPtr->setMothers3Momentum(p0);
  }else{
    evtPtr = getEvent();
  }
  
  resetP();
  
  double pK[2]={-9999, -9999};
  double pPi[2]={-9999, -9999};
  int iK=0, ip=0;
  for(unsigned int i=1; i < pat.size(); i++){
    if(abs(pat[i]) == 321) pK[iK++]=evtPtr->p(i).Rho()/GeV;
    if(abs(pat[i]) == 211) pPi[ip++]=evtPtr->p(i).Rho()/GeV;
  }
  if(iK != 2 || ip !=2){
    cout << "ERROR could not match all indices. K: " 
	 << iK << ", Pi: " << ip << "." << endl;
    return false;
  }
  if(pK[0] > pK[1]){
    _pKFast = pK[0]; _pKSlow = pK[1];
  }else{
    _pKFast = pK[1]; _pKSlow = pK[0];
  }
  if(pPi[0] > pPi[1]){
    _pPiFast = pPi[0]; _pPiSlow = pPi[1];
  }else{
    _pPiFast = pPi[1]; _pPiSlow = pPi[0];
  }
  return true;
}

double KKpipiEfficiency::oneEff(TH1F* h, double p){
  bool dbThis=false;
  if(0 == h) return 1;
  if(dbThis)cout << "oneEff for " << h->GetName() << " and " << p << ":\n";
  double bw = (h->GetXaxis()->GetXmax() -  
	       h->GetXaxis()->GetXmin()
	       )/h->GetXaxis()->GetNbins();
  int bin = (int) ((p - h->GetXaxis()->GetXmin())/bw + 0.5);
  double returnVal =  h->GetBinContent(bin);
  if(dbThis)cout << "\t p= " << p << "\t bw = " << bw << " bin = " << bin 
		 << ", returnVal = " << returnVal << endl;
  return returnVal;
}

double KKpipiEfficiency::eff(){
  bool dbThis=false;
  double prod=1;
  if(! getP()){
    cout << " couldn't get momenta - returning 0" << endl;
    return 0;
  }
  prod *= oneEff(_hKFast, _pKFast);
  prod *= oneEff(_hKSlow, _pKSlow);
  prod *= oneEff(_hPiFast, _pPiFast);
  prod *= oneEff(_hPiSlow, _pPiSlow);
  if(dbThis) cout << "RealVal(): returning eff = " << prod << endl;
  return prod;
}  

double KKpipiEfficiency::simple(){
  if(0 == getEvent()) return 0;
  return getEvent()->s(1,3) > 0.7*GeV*GeV ? 1.0 : 0.1;
}

double KKpipiEfficiency::RealVal(){
  //    return simple();
  return eff();
}
//
