// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:01 GMT
#include "Mint/DalitzPdfBaseFastInteg.h"
#include "Mint/FitAmpSum.h"
#include "Mint/DalitzBWBoxSet.h"
#include "Mint/Minimiser.h"

#include <iostream>

using namespace std;
using namespace MINT;

MinuitParameterSet* DalitzPdfBaseFastInteg::getMPS(){
  return (0 == _mps ? MinuitParameterSet::getDefaultSet() : _mps);
}
const MinuitParameterSet* DalitzPdfBaseFastInteg::getMPS() const{
  return (0 == _mps ? MinuitParameterSet::getDefaultSet() : _mps);
}

bool DalitzPdfBaseFastInteg::saveIntegrator(const std::string& fname) const{
  return _faint.save(fname);
}

bool DalitzPdfBaseFastInteg::getNorm(){
  _integrating = true;
  if( ! _faint.initialised()){
    if(_pat.empty()){
      cout << "DalitzPdfBaseFastInteg::getNorm: don't know event pattern!"
	   << endl;
      _integrating = false;
      return false;
    }
    if(0 == getAmps()){
      cout << "DalitzPdfBaseFastInteg::getNorm: don't have amps!"
	   << endl;
      _integrating = false;
      return false;
    }

    /*
    if("" != _commaSepList_of_SavedIntegrators){
      _faint.initialiseFromFile(evtPtr->eventPattern()
				, _amps
				, _commaSepList_of_SavedIntegrators);
    }else{
    */
    _faint.initialise(_commaSepList_of_SavedIntegrators
		      , _pat
		      , getAmps()
		      //, _efficiency
		      , getEventGenerator()
		      , gRandom
		      , _precision
		      );
      //}
    
#ifdef CHECK_INTEGRATOR
    cout << "WARNING in DalitzPdfBaseFastInteg::getNorm!!!"
	 << " using old, inefficient integrator."
	 << " Won't work for efficiencies!!"
	 << endl;
    _mcint.initialise(evtPtr->eventPattern()
		      , getAmps()//this
		      , getEventGenerator()
		      , gRandom
		      , _precision
		      );
#endif
  }

  _norm = _faint.getVal();


#ifdef CHECK_INTEGRATOR
  double fastVal = _norm;
  double oldVal = _mcint.getVal();
  cout << "DalitzPdfBaseFastInteg::getNorm:"
       << " compare mcint integral with fast: "
       << "\n\t old: " << oldVal << " , fast: " << fastVal
       << ", ratio " << oldVal/fastVal
       << endl;
  _norm = oldVal;
#endif


  _integrating = false;
  return _norm > 0;
}
IEventGenerator<IDalitzEvent>* DalitzPdfBaseFastInteg::makeDefaultGenerator(){
  if(_pat.empty()) return 0;
  if(0 == getAmps()) return 0;
  counted_ptr<IEventGenerator<IDalitzEvent> > 
    ptr(getAmps()->makeEventGenerator(_pat));
  _defaultGenerator = ptr;
  return _defaultGenerator.get();
}

IEventGenerator<IDalitzEvent>* DalitzPdfBaseFastInteg::getEventGenerator(){
  if(0 == _generator) makeDefaultGenerator();
  return _generator;
}
const IEventGenerator<IDalitzEvent>* DalitzPdfBaseFastInteg::getEventGenerator()const{
  if(0 == _generator){
    cout << "WARNING in DalitzPdfBaseFastInteg::getEventGenerator() const"
	 << " returning 0 pointer." << endl;
  }
  return _generator;
}

void DalitzPdfBaseFastInteg::setIntegrationPrecision(double prec){
  _precision = prec;
#ifdef CHECK_INTEGRATOR
  _mcint.setPrecision(_precision);
#endif
  _faint.setPrecision(_precision);
}

void DalitzPdfBaseFastInteg::parametersChanged(){
  getNorm();
  _val= -1;
  _redoGradNorm=true;
}

DalitzPdfBaseFastInteg::DalitzPdfBaseFastInteg(const DalitzEventPattern& pat
					       , IEventGenerator<IDalitzEvent>*
						   generator
					        , IFastAmplitudeIntegrable* amps
						, double prec
						)
  : PdfBase<IDalitzEvent>()
  , _mps(0)
  , _pat(pat)
  , _norm(-1)
  , _precision(prec)
  , _amps(amps)
  , _countedAmps(0)
    //, _efficiency(0)
  , _generator(generator)
  , _integrating(0)
  , _defaultGenerator(0)
  , _commaSepList_of_SavedIntegrators("")
  , _val(-1)
  , _gradNorm(0)
  , _redoGradNorm(true)
{
  setup();
  _gradNorm= new Double_t[this->getMPS()->size()];  
  for(unsigned int i=0; i < this->getMPS()->size(); i++) _gradNorm[i]= 0.;
}

DalitzPdfBaseFastInteg::DalitzPdfBaseFastInteg(const DalitzEventPattern& pat
					       , IEventGenerator<IDalitzEvent>*
						   generator
						//, counted_ptr<IGetDalitzEvent> eff
						, MinuitParameterSet* mps
						, double prec
					       ) 
  : PdfBase<IDalitzEvent>()
  , _mps(mps)
  , _pat(pat)
  , _norm(-1)
  , _precision(prec)
  , _amps(0)
  , _countedAmps(0)
    //  , _efficiency(eff)
  , _generator(generator)
  , _integrating(0)
  , _defaultGenerator(0)
  , _commaSepList_of_SavedIntegrators("")
  , _val(-1)
  , _gradNorm(0)
  , _redoGradNorm(true)
{
  setup();
  _gradNorm= new Double_t[this->getMPS()->size()];  
  for(unsigned int i=0; i < this->getMPS()->size(); i++) _gradNorm[i]= 0.;
  //  cout << "pset pointer in DalitzPdfBaseFastInteg " << _mps << " = " << getMPS() << endl;
}

DalitzPdfBaseFastInteg::DalitzPdfBaseFastInteg(const DalitzPdfBaseFastInteg& other)
  : IReturnRealForEvent<IDalitzEvent>()
  , IPdf<IDalitzEvent>()
  , IDalitzPdf()
  , PdfBase<IDalitzEvent>(other)
  , _mps(other._mps)
  , _pat(other._pat)
  , _norm(other._norm)
  , _precision(other._precision)
  , _amps(other._amps)
  , _countedAmps(other._countedAmps)
    //, _efficiency(other._efficiency)
  , _generator(other._generator)
  , _integrating(other._integrating)
  , _defaultGenerator(other._defaultGenerator)
  , _commaSepList_of_SavedIntegrators(other._commaSepList_of_SavedIntegrators)
  , _val(-1)
  , _gradNorm(0)
  , _redoGradNorm(true)
{
  setup();
  _gradNorm= new Double_t[this->getMPS()->size()];  
  for(unsigned int i=0; i < this->getMPS()->size(); i++) _gradNorm[i]= 0.;
}

DalitzPdfBaseFastInteg::~DalitzPdfBaseFastInteg(){
    delete[] _gradNorm;
}

void DalitzPdfBaseFastInteg::setup(){
  makeAmps(); // it's important to do this at construction time, otherwise the MinuitParameterSet will be empty.
}
bool DalitzPdfBaseFastInteg::makeAmps(){
  cout << "DalitzPdfBaseFastInteg::makeAmps() with MPS pointer " << getMPS() 
       << " and _amps pointer " << _amps << endl;
  if(0 == _amps && (! _pat.empty())){
      cout << "DalitzPdfBaseFastInteg::getAmps() making _amps with pointer " << getMPS() 
	   << endl;
    _amps = new FitAmpSum(_pat, getMPS());
    counted_ptr<IFastAmplitudeIntegrable> cp(_amps);
    _countedAmps = cp;
  }
  return (bool) _amps;
}

bool DalitzPdfBaseFastInteg::integrating(){
  return _integrating;
}

double DalitzPdfBaseFastInteg::phaseSpace(IDalitzEvent& evt){
  return evt.phaseSpace();
}
double DalitzPdfBaseFastInteg::getVal(IDalitzEvent& evt){
  if(_pat.empty()) _pat = evt.eventPattern();
  if(_integrating) return getVal_withPs(evt);
  else return getVal_noPs(evt);
}
double DalitzPdfBaseFastInteg::getVal_noPs(IDalitzEvent& evt){
  bool dbThis = false;
  static double maxVal=0;
  if(_pat.empty()) _pat = evt.eventPattern();

  if(_integrating){
    // shouldn't really do that - use getVal or getVal_withPs
    // when you integrate (automatically done in getVal()):
    return un_normalised_noPs(evt);
  }else{
    if(_norm == -1) getNorm();
    double num = un_normalised_noPs(evt);
    if(dbThis){
      double val=num/_norm;
      if(fabs(val) > maxVal){
	maxVal=fabs(val);
	if(dbThis){
	  cout  << "biggest un_normalised / norm, yet: " 
		<< num << " / " <<_norm 
		<< " = " << num/_norm
		<< ". ps = " << phaseSpace(evt)
		<< endl;
	  cout << "for event: "; evt.print(); cout << endl;
	}
      }
    }

    /*
    cout << "\n DalitzPdfBaseFastInteg::getVal_noPs():"
	 << " for event ptr " << getEvent()
	 << " returning "
	 << num << " / " << _norm << endl;
    */
      
    _val= num/_norm;

    return num/_norm;
  }
}
double DalitzPdfBaseFastInteg::getVal_withPs(IDalitzEvent& evt){
  if(_pat.empty()) _pat = evt.eventPattern();

  if(_integrating){
    return un_normalised_noPs(evt)*phaseSpace(evt);
  }else{
    if(_norm == -1) getNorm();
    double num = un_normalised_noPs(evt)*phaseSpace(evt);
    return num/_norm;
  }
}

void DalitzPdfBaseFastInteg::Gradient(IDalitzEvent& evt,Double_t* grad, MINT::MinuitParameterSet* mps){
    
    if(_val == -1)un_normalised_noPs(evt);
    if(_norm == -1) getNorm();
    
    unsigned int N= mps->size();
    
    Double_t gradAmpSum[N];
    for(unsigned int j=0; j < N; j++) gradAmpSum[j]= 0.;   
    
    _amps->Gradient(evt,gradAmpSum,mps);
    
    if(_redoGradNorm){
        for(unsigned int j=0; j < N; j++) _gradNorm[j]= 0.;   
        _faint.Gradient(mps, _gradNorm);
        _redoGradNorm=false;
    }
    

    for (unsigned int i=0; i<N; i++) {
        if(mps->getParPtr(i)->hidden() || mps->getParPtr(i)->iFixInit())continue;
        grad[i]= gradAmpSum[i]/_norm- _val/_norm*_gradNorm[i];
    }
    
    
}

void DalitzPdfBaseFastInteg::GradientForLasso(Double_t* grad){
    _faint.GradientForLasso(this->getMPS(),grad);
}

bool DalitzPdfBaseFastInteg::makePlots(const std::string& filename) const{
  return histoSet().save(filename);
}
DalitzHistoSet DalitzPdfBaseFastInteg::histoSet() const{
  return (_faint.histoSet());
}
DalitzHistoSet DalitzPdfBaseFastInteg::histoSet(){
    // non-const version to satisfy IDalitzPdf
  return (_faint.histoSet());
}

void DalitzPdfBaseFastInteg::saveEachAmpsHistograms(const std::string& prefix) const{
  _faint.saveEachAmpsHistograms(prefix);
  return;
}

std::vector<DalitzHistoSet> DalitzPdfBaseFastInteg::GetEachAmpsHistograms(){
  return _faint.GetEachAmpsHistograms();
}

DalitzHistoSet DalitzPdfBaseFastInteg::interferenceHistoSet() const{
    return (_faint.interferenceHistoSet());
}
DalitzHistoSet DalitzPdfBaseFastInteg::interferenceHistoSet(){
    // non-const version to satisfy IDalitzPdf
    return (_faint.interferenceHistoSet());
}
void DalitzPdfBaseFastInteg::saveInterferenceHistograms(const std::string& prefix) const{
    _faint.saveInterferenceHistograms(prefix);
    return;
}
std::vector<DalitzHistoSet> DalitzPdfBaseFastInteg::GetInterferenceHistograms(){
    return _faint.GetInterferenceHistograms();
}

void DalitzPdfBaseFastInteg::endFit(){

  //_faint.doFinalStats();
}

void DalitzPdfBaseFastInteg::doFinalStats(Minimiser* mini){
  _faint.doFinalStats(mini);
}

void DalitzPdfBaseFastInteg::setIntegratorFileName(const std::string& commaSeparatedList){
  _commaSepList_of_SavedIntegrators=commaSeparatedList;
}


//
