// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:01 GMT
#include "Mint/DalitzPdfBaseMCInteg.h"
#include "Mint/FitAmpSum.h"
#include "Mint/DalitzBWBoxSet.h"
#include "Mint/Minimiser.h"

#include <iostream>

using namespace std;
using namespace MINT;

MinuitParameterSet* DalitzPdfBaseMCInteg::getMPS(){
  return (0 == _mps ? MinuitParameterSet::getDefaultSet() : _mps);
}
const MinuitParameterSet* DalitzPdfBaseMCInteg::getMPS() const{
  return (0 == _mps ? MinuitParameterSet::getDefaultSet() : _mps);
}


bool DalitzPdfBaseMCInteg::getNorm(){
  _integrating = true;

  if(!_mcint.initialised())_mcint.initialise(_pat, getAmps(), getEventGenerator(), gRandom, _precision);
  else _mcint.resetIntegrand(getAmps());
  _norm = _mcint.getVal();

  _integrating = false;
  return _norm > 0;
}

IEventGenerator<IDalitzEvent>* DalitzPdfBaseMCInteg::makeDefaultGenerator(){
  if(_pat.empty()) return 0;
  if(0 == getAmps()) return 0;
  counted_ptr<IEventGenerator<IDalitzEvent> > 
    ptr(getAmps()->makeEventGenerator(_pat));
  _defaultGenerator = ptr;
  return _defaultGenerator.get();
}

IEventGenerator<IDalitzEvent>* DalitzPdfBaseMCInteg::getEventGenerator(){
  if(0 == _generator) makeDefaultGenerator();
  return _generator;
}
const IEventGenerator<IDalitzEvent>* DalitzPdfBaseMCInteg::getEventGenerator()const{
  if(0 == _generator){
    cout << "WARNING in DalitzPdfBaseMCInteg::getEventGenerator() const"
	 << " returning 0 pointer." << endl;
  }
  return _generator;
}

void DalitzPdfBaseMCInteg::setIntegrationPrecision(double prec){
  _precision = prec;
  _mcint.setPrecision(_precision);
}

void DalitzPdfBaseMCInteg::parametersChanged(){
  getNorm();
  _val= -1;
}
DalitzPdfBaseMCInteg::DalitzPdfBaseMCInteg(const DalitzEventPattern& pat, IFastAmplitudeIntegrable* amps
						, double prec, IEventGenerator<IDalitzEvent>* generator, MinuitParameterSet* mps)
  : PdfBase<IDalitzEvent>()
  , _mps(mps)
  , _pat(pat)
  , _norm(-1)
  , _precision(prec)
  , _amps(amps)
  , _generator(generator)
  , _integrating(0)
  , _defaultGenerator(0)
  , _val(-1)
{
}


DalitzPdfBaseMCInteg::DalitzPdfBaseMCInteg(const DalitzPdfBaseMCInteg& other)
  : IReturnRealForEvent<IDalitzEvent>()
  , IPdf<IDalitzEvent>()
  , IDalitzPdf()
  , PdfBase<IDalitzEvent>(other)
  , _mps(other._mps)
  , _pat(other._pat)
  , _norm(other._norm)
  , _precision(other._precision)
  , _amps(other._amps)
  , _generator(other._generator)
  , _integrating(other._integrating)
  , _defaultGenerator(other._defaultGenerator)
  , _val(-1)
{
}

DalitzPdfBaseMCInteg::~DalitzPdfBaseMCInteg(){
}

IFastAmplitudeIntegrable* DalitzPdfBaseMCInteg::getAmps(){
  if(0 == _amps && (! _pat.empty())){
    _amps = new FitAmpSum(_pat, getMPS());
  }
  return _amps;
}

bool DalitzPdfBaseMCInteg::integrating(){
  return _integrating;
}

double DalitzPdfBaseMCInteg::phaseSpace(IDalitzEvent& evt){
  return evt.phaseSpace();
}
double DalitzPdfBaseMCInteg::getVal(IDalitzEvent& evt){
  //if(_integrating) return getVal_withPs(evt);else 
  return getVal_noPs(evt);
}
double DalitzPdfBaseMCInteg::getVal_noPs(IDalitzEvent& evt){

  if(_integrating){
    // shouldn't really do that - use getVal or getVal_withPs
    // when you integrate (automatically done in getVal()):
    return un_normalised_noPs(evt);
  }else{
    if(_norm == -1) getNorm();
    double num = un_normalised_noPs(evt);
     _val= num/_norm;
      
    return num/_norm;
  }
}
double DalitzPdfBaseMCInteg::getVal_withPs(IDalitzEvent& evt){

  if(_integrating){
    return un_normalised_noPs(evt)*phaseSpace(evt);
  }else{
    if(_norm == -1) getNorm();
    double num = un_normalised_noPs(evt)*phaseSpace(evt);
    return num/_norm;
  }
}


DalitzHistoSet DalitzPdfBaseMCInteg::histoSet(){
    // non-const version to satisfy IDalitzPdf
    return (_mcint.histoSet());
}

/*
bool DalitzPdfBaseMCInteg::makePlots(const std::string& filename) const{
  return histoSet().save(filename);
}
DalitzHistoSet DalitzPdfBaseMCInteg::histoSet() const{
  return (_faint.histoSet());
}


void DalitzPdfBaseMCInteg::saveEachAmpsHistograms(const std::string& prefix) const{
  _faint.saveEachAmpsHistograms(prefix);
  return;
}

std::vector<DalitzHistoSet> DalitzPdfBaseMCInteg::GetEachAmpsHistograms(){
  return _faint.GetEachAmpsHistograms();
}

DalitzHistoSet DalitzPdfBaseMCInteg::interferenceHistoSet() const{
    return (_faint.interferenceHistoSet());
}
DalitzHistoSet DalitzPdfBaseMCInteg::interferenceHistoSet(){
    // non-const version to satisfy IDalitzPdf
    return (_faint.interferenceHistoSet());
}
void DalitzPdfBaseMCInteg::saveInterferenceHistograms(const std::string& prefix) const{
    _faint.saveInterferenceHistograms(prefix);
    return;
}
std::vector<DalitzHistoSet> DalitzPdfBaseMCInteg::GetInterferenceHistograms(){
    return _faint.GetInterferenceHistograms();
}
*/
void DalitzPdfBaseMCInteg::endFit(){

  //_faint.doFinalStats();
}

void DalitzPdfBaseMCInteg::doFinalStats(Minimiser* mini){
  //_faint.doFinalStats(mini);
}



//
