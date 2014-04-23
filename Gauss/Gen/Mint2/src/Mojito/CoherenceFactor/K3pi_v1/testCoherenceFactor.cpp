// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:59 GMT
#include "Mint/FitParameter.h"
#include "Mint/NamedParameter.h"
#include "Mint/DalitzEventPattern.h"
#include "Mint/FitAmpSum.h"
#include "Mint/FitAmplitude.h"
#include "Mint/DalitzHistoSet.h"

#include "Mint/SignalGenerator.h"
#include "Mint/FastAmplitudeIntegrator.h"

#include "Mint/CoherenceFactorCalculator.h"
#include "Mint/FitComplexPolar.h"

#include "TH2F.h"
#include "Mint/IGetRealEvent.h"
#include "Mint/DalitzEventAccess.h"
#include "Mint/DalitzEventPattern.h"

#include <ctime>

#include <iostream>

#include <algorithm>

using namespace std;
using namespace MINT;


class Efficiency 
  : virtual public IGetRealEvent<IDalitzEvent>
  , public DalitzEventAccess
{
  std::string _effDir;
  TH1F   *_h_KMinus, *_h_piPlusFast, *_h_piPlusSlow, *_h_piMinus;
  int    _KMinus_index, _piPlusFast_index, _piPlusSlow_index, _piMinus_index;
  double _p_KMinus, _p_piPlusFast, _p_piPlusSlow, _p_piMinus;
  int    _KMinus_bin, _piPlusFast_bin, _piPlusSlow_bin, _piMinus_bin;
public:
  Efficiency(const DalitzEventPattern& pat
	     , const std::string& efficiencyDir = ""
	     , const std::string& fKMinus     = "kaon_ratio.root"
	     , const std::string& fpiPlusFast = "pionfastOS_ratio.root"
	     , const std::string& fpiPlusSlow = "pionslowOS_ratio.root"
	     //	     , const std::string& fpiMinus    = "pionSameS_ratio.root"
	     )
    :  DalitzEventAccess(pat)
    , _effDir(efficiencyDir)
    , _h_KMinus(0), _h_piPlusFast(0), _h_piPlusSlow(0), _h_piMinus(0)
    , _KMinus_index(-9999), _piPlusFast_index(-9999), _piPlusSlow_index(-9999), _piMinus_index(-9999)
    , _p_KMinus(-9999), _p_piPlusFast(-9999), _p_piPlusSlow(-9999), _p_piMinus(-9999)
    , _KMinus_bin(-9999), _piPlusFast_bin(-9999), _piPlusSlow_bin(-9999), _piMinus_bin(-9999)
  {
    _h_KMinus     = get_h( _effDir + "/" + fKMinus    , "kaon_ratio");
    _h_piPlusFast = get_h( _effDir + "/" + fpiPlusFast, "pion_fast_ratio");
    _h_piPlusSlow = get_h( _effDir + "/" + fpiPlusSlow, "pion_slow_ratio");
    //    _h_piMinus = get_h( _effDir + "/" + fpiMinus, "pion_same_ratio");
  }
  static TH1F* get_h(const std::string& fname, const std::string& hname){
    TH1F* h=0;
    if("" != fname){
      TFile* f = new TFile(fname.c_str());
      cout << " got file ptr: " << f << endl;
      if(0 != f){
	h = (TH1F*) f->Get(hname.c_str());
	if(0 != h){
	  cout << "got efficiency histogram " 
	       << hname << " from file " << fname << "." << endl;
	}else{
	  cout << "ERROR didn't get efficiency histogram " 
	       << hname << " from file " << fname << "!!!" << endl;
	}
      }
    }
    return h;
  }
  bool getIndices(){
    if(0 == getEvent()) return false;
    DalitzEventPattern pat = getEvent()->eventPattern();
    _KMinus_index = _piPlusFast_index = _piPlusSlow_index = _piMinus_index = -9999;
    for(unsigned int i=0; i < pat.size(); i++){
      if(-321 == pat[i]){
	_KMinus_index = i;
      }else if(211 == pat[i]){
	if(_piPlusFast_index > 0) _piPlusSlow_index = i;
	else _piPlusFast_index = i;
      }else if(-211 == pat[i]) _piMinus_index =i;
    }
    if(_KMinus_index < 0 || _piPlusFast_index < 0 || _piPlusSlow_index < 0 || _piMinus_index < 0){
      cout << "couldn't match all indices " 
	   << _KMinus_index << ", " << _piPlusFast_index << ", "
	   <<  _piPlusSlow_index << ", " << _piMinus_index << endl;
      return false;
    }
    double pf2 = getEvent()->p(_piPlusFast_index).Rho();
    double ps2 = getEvent()->p(_piPlusSlow_index).Rho();
    if(ps2 > pf2) swap(_piPlusFast_index, _piPlusSlow_index);
    return true;
  }
  bool get_p_GeV(){
    if(! getIndices()) return false;
    _p_KMinus     = getEvent()->p(_KMinus_index).Rho()/GeV;
    _p_piPlusFast = getEvent()->p(_piPlusFast_index).Rho()/GeV;
    _p_piPlusSlow = getEvent()->p(_piPlusSlow_index).Rho()/GeV;
    _p_piMinus    = getEvent()->p(_piMinus_index).Rho()/GeV;
    return true;
  }

  static double oneEff(TH1F* h, double p){
    bool dbThis=false;
    if(0 == h) return 1;
    if(dbThis)cout << "oneEff for " << h->GetName() << " and " << p << ":\n";
    double bw = (h->GetXaxis()->GetXmax() -  h->GetXaxis()->GetXmin())/h->GetXaxis()->GetNbins();
    int bin = (int) ((p - h->GetXaxis()->GetXmin())/bw + 0.5);
    double returnVal =  h->GetBinContent(bin);
    if(dbThis)cout << "\t bw = " << bw << " bin = " << bin << ", returnVal = " << returnVal << endl;
    return returnVal;
  }
  
  double RealVal(){
    bool dbThis=false;
    double prod=1;
    if(! get_p_GeV()){
      cout << "ERROR in RealVal. Failed to get momenta" << endl;
    }
    prod *= oneEff(_h_KMinus, _p_KMinus);
    prod *= oneEff(_h_piPlusFast, _p_piPlusFast);
    prod *= oneEff(_h_piPlusSlow, _p_piPlusSlow);
    //    prod *= oneEff(_h_piMinus, _p_piMinus);
    if(dbThis) cout << "RealVal(): returning eff = " << prod << endl;
    if(dbThis) cout << " RealVal(): p_K is " << _p_KMinus << endl;
    return prod;
  }  
};

int coherenceFactor(){
  time_t startTime = time(0);
  NamedParameter<int> EventPattern("Event Pattern", 421, -321, 211, 211, -211);
  NamedParameter<double> CSFactor("CSFactor", (double) 1);
  NamedParameter<double> CSPhase("CSPhase", (double) 0);
  NamedParameter<double> precision("Precision", (double) 1.e-2);
  NamedParameter<string> cfName("cfName", (std::string) "CoherenceF");

  cout << "phase before degFac " << CSPhase << endl;
  CSPhase *= FitComplexPolar::degFac();
  cout << " .. after: " << CSPhase << endl;

  NamedParameter<std::string> EfficiencyDir("EfficiencyDir", (std::string) "eff_histos");

  DalitzEventPattern pat(EventPattern);
  DalitzEventPattern patBar(EventPattern);
  patBar[0].antiThis();

  cout << "Good morning from coherenceFactor()!" << endl;
  cout << "pat = " << pat << ", patBar = " << patBar << endl;
  FitAmplitude::AutogenerateFitFile();

  cout << " making Amps" << endl;
  FitAmpSum Amps(pat);
  cout << " making AmpsBar" << endl;
  
  counted_ptr<FitAmpSum> AmpsBarPtr;

  if(pat.selfConjugateFS()){
    AmpsBarPtr = (counted_ptr<FitAmpSum>)Amps.GetCPConjugateSameFitParameters();
  }else{
    AmpsBarPtr = counted_ptr<FitAmpSum>(new FitAmpSum(patBar));
  }
  cout << " I made the amplitudes. Now comes the coherence factor"
       << endl;

  NamedParameter<int> CheckFractionsFirst("CheckFractionsFirst", 0);
  if(1 == CheckFractionsFirst){
    cout << " but first I'll check the fractions" << endl;
    SignalGenerator sg(&Amps);
    sg.setWeighted();
    //    SignalGenerator sgbar(AmpsBar.get());
    NamedParameter<double> IntegPrecision("IntegPrecision", 1.e-3);
    FastAmplitudeIntegrator integ(pat, &Amps, &sg, gRandom, IntegPrecision);
    cout << "integrator value: " << integ.getVal() << endl;
    integ.doFinalStats();
    cout << " done stats - now plotting" << endl;
    DalitzHistoSet histos = integ.histoSet();
    histos.save("histos.root");
    histos.draw("histoPlots_");
    return 0;
  }

  Efficiency eff(pat, EfficiencyDir);

  CoherenceFactorCalculator CF(Amps, *AmpsBarPtr, CSFactor, CSPhase, &eff
			       , precision
			       , cfName
			       );

  cout << " I created the coherence factor class. Now calculate."
       << endl;

  CF.evaluate();

  cout << "CoherenceFactor() calculated, it's "
       << CF.evaluate()
       << " = " << abs(CF.evaluate()) 
       << " * exp( i * " <<  arg(CF.evaluate()) << " )"
       << " and this took "
       << difftime(time(0), startTime)
       << " s."
       << endl;

  CF.printResult();

  return 0;
}


int main(){

  return coherenceFactor();

}
//
