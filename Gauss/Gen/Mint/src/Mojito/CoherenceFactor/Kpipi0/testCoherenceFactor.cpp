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

using namespace std;
using namespace MINT;


class Efficiency 
  : virtual public IGetRealEvent<IDalitzEvent>
  , public DalitzEventAccess
{
  TH2F* _h;
  int _K_index, _piPlus_index, _piZero_index;
public:
  Efficiency(const DalitzEventPattern& pat, const std::string& fname="")
    :  DalitzEventAccess(pat)
  {
    if("" != fname){
      TFile* f = new TFile(fname.c_str());
      cout << " got file ptr: " << f << endl;
      if(0 != f){
	_h = (TH2F*) f->Get("eff");
	if(0 != _h){
	  cout << "got efficiency histogram!" << endl;
	}else{
	  cout << "didn't get efficiency histogram!" << endl;
	}
      }
    }
  }
  bool getIndices(){
    if(0 == getEvent()) return false;
    DalitzEventPattern pat = getEvent()->eventPattern();
    _K_index = _piPlus_index = _piZero_index = -9999;
    for(unsigned int i=0; i < pat.size(); i++){
      if(321 == abs((int) pat[i])) _K_index = i;
      else if(211 == abs((int) pat[i])) _piPlus_index = i;
      else if(111 == abs((int) pat[i])) _piZero_index = i;
    }
    if(_K_index < 0 || _piPlus_index < 0 || _piZero_index < 0){
      cout << "couldn't match all indices " 
	   << _K_index << ", " << _piPlus_index << ", " <<  _piZero_index << endl;
      return false;
    }else{
      return true;
    }
  }
  double get_s_KPiPlus_GeV(){
    if(! getIndices()) return -9999;
    return getEvent()->s(_K_index, _piPlus_index)/(GeV*GeV);
  }
  double get_s_KPiZero_GeV(){
    if(! getIndices()) return -9999;
    return getEvent()->s(_K_index, _piZero_index)/(GeV*GeV);
  }
  int XBin(){
    if(0 == getEvent()) return -9999;
    double bw = (_h->GetXaxis()->GetXmax() -  _h->GetXaxis()->GetXmin())/_h->GetXaxis()->GetNbins();
    return (int) ((get_s_KPiPlus_GeV() - _h->GetXaxis()->GetXmin())/bw + 0.5);
  }
  int YBin(){
    if(0 == getEvent()) return -9999;
    double bw = (_h->GetYaxis()->GetXmax() -  _h->GetYaxis()->GetXmin())/_h->GetYaxis()->GetNbins();
    return (int) ((get_s_KPiZero_GeV()  - _h->GetYaxis()->GetXmin())/bw + 0.5);
  }
  double RealVal(){
    if(0 == _h) return 1;
    double returnVal = _h->GetBinContent(XBin(), YBin());
    //cout << "returning " << returnVal << " for " << get_s_KPiPlus_GeV() << ", " << get_s_KPiZero_GeV() << endl;
    if(returnVal < 0) return 0;
    return returnVal;
  }  
};

int coherenceFactor(){
  time_t startTime = time(0);
  NamedParameter<int> EventPattern("Event Pattern", 421, -321, 211, 211, -211);
  NamedParameter<double> CSFactor("CSFactor", (double) 1);
  NamedParameter<double> CSPhase("CSPhase", (double) 0);

  cout << "phase before degFac " << CSPhase << endl;
  CSPhase *= FitComplexPolar::degFac();
  cout << " .. after: " << CSPhase << endl;

  NamedParameter<std::string> EfficiencyFile("EfficiencyFile", (std::string) "");

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

  Efficiency eff(pat, EfficiencyFile);

  CoherenceFactorCalculator CF(Amps, *AmpsBarPtr, CSFactor, CSPhase, &eff);

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
