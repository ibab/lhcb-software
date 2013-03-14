// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:01 GMT
#include "Mint/DalitzPdfBaseFastInteg.h"
#include "Mint/FitAmpSum.h"
#include "Mint/DalitzBWBoxSet.h"
#include "Mint/Minimiser.h"

#include <iostream>

using namespace std;
using namespace MINT;

bool DalitzPdfBaseFastInteg::saveIntegrator(const std::string& fname) const{
  return _faint.save(fname);
}

bool DalitzPdfBaseFastInteg::getNorm(){
  _integrating = true;
  if( ! _faint.initialised()){
    IDalitzEvent* evtPtr = getEvent();
    if(0==evtPtr){
      cout << "DalitzPdfBaseFastInteg::getNorm: can't get event!"
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
		      , evtPtr->eventPattern()
		      , _amps
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
		      , _amps//this
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
  if(0 == _amps) return 0;
  counted_ptr<IEventGenerator<IDalitzEvent> > 
    ptr(_amps->makeEventGenerator());
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
}
DalitzPdfBaseFastInteg::DalitzPdfBaseFastInteg( IDalitzEventAccess* events
						, IEventGenerator<IDalitzEvent>*
						   generator
					        , IFastAmplitudeIntegrable* amps
						, double prec
						)
  : PdfBase<IDalitzEvent>(events)
  , _norm(-1)
  , _precision(prec)
  , _amps(amps)
  , _countedAmps(0)
    //, _efficiency(0)
  , _generator(generator)
  , _integrating(0)
  , _defaultGenerator(0)
  , _commaSepList_of_SavedIntegrators("")
{
   if(0 == _amps){
     _amps = new FitAmpSum(this);
     counted_ptr<IFastAmplitudeIntegrable> cp(_amps);
     _countedAmps = cp;
   }
   else{
     _amps->setDaddy(this);
   }
   if(0 == _generator) makeDefaultGenerator();
}

DalitzPdfBaseFastInteg::DalitzPdfBaseFastInteg(IDalitzEventList* events
					       , IEventGenerator<IDalitzEvent>*
					         generator
					       , IFastAmplitudeIntegrable* amps
					       , double prec
					       ) 
  : PdfBase<IDalitzEvent>(events)
  , _norm(-1)
  , _precision(prec)
  , _amps(amps)
  , _countedAmps(0)
    //, _efficiency(0)
  , _generator(generator)
  , _integrating(0)
  , _defaultGenerator(0)
  , _commaSepList_of_SavedIntegrators("")
{
   if(0 == _amps){
     _amps = new FitAmpSum(this);
     counted_ptr<IFastAmplitudeIntegrable> cp(_amps);
     _countedAmps = cp;
   }
   else{
     _amps->setDaddy(this);
   }
   if(0 == _generator) makeDefaultGenerator();
}

/*
DalitzPdfBaseFastInteg::DalitzPdfBaseFastInteg( IDalitzEventAccess* events
						, IEventGenerator<IDalitzEvent>*
						   generator
					        , IFastAmplitudeIntegrable* amps
						, counted_ptr<IGetDalitzEvent> eff
						, double prec
						)
  : PdfBase<IDalitzEvent>(events)
  , _norm(-1)
  , _precision(prec)
  , _amps(amps)
  , _countedAmps(0)
  , _efficiency(eff)
  , _generator(generator)
  , _integrating(0)
  , _defaultGenerator(0)
{
   if(0 == _amps){
     _amps = new FitAmpSum(this);
     counted_ptr<IFastAmplitudeIntegrable> cp(_amps);
     _countedAmps = cp;
   }
   else{
     _amps->setDaddy(this);
   }
   if(0 == _generator) makeDefaultGenerator();
}
DalitzPdfBaseFastInteg::DalitzPdfBaseFastInteg(IDalitzEventList* events
					       , IEventGenerator<IDalitzEvent>*
					           generator
					        , IFastAmplitudeIntegrable* amps
						, counted_ptr<IGetDalitzEvent> eff
					       , double prec
					       ) 
  : PdfBase<IDalitzEvent>(events)
  , _norm(-1)
  , _precision(prec)
  , _amps(amps)
  , _countedAmps(0)
  , _efficiency(eff)
  , _generator(generator)
  , _integrating(0)
  , _defaultGenerator(0)
{
   if(0 == _amps){
     _amps = new FitAmpSum(this);
     counted_ptr<IFastAmplitudeIntegrable> cp(_amps);
     _countedAmps = cp;
   }
   else{
     _amps->setDaddy(this);
   }
   if(0 == _generator) makeDefaultGenerator();
}
*/

DalitzPdfBaseFastInteg::DalitzPdfBaseFastInteg( IDalitzEventAccess* events
						, IEventGenerator<IDalitzEvent>*
						   generator
						//, counted_ptr<IGetDalitzEvent> eff
						, MinuitParameterSet* mps
						, double prec
					       ) 
  : PdfBase<IDalitzEvent>(events)
  , _norm(-1)
  , _precision(prec)
  , _amps(0)
  , _countedAmps(0)
    //  , _efficiency(eff)
  , _generator(generator)
  , _integrating(0)
  , _defaultGenerator(0)
  , _commaSepList_of_SavedIntegrators("")
{
  MinuitParameterSet *thisMps = (0 == mps ? MinuitParameterSet::getDefaultSet() : mps);
  _amps = new FitAmpSum(this, thisMps);

  counted_ptr<IFastAmplitudeIntegrable> cp(_amps);
  _countedAmps = cp;
  if(0 == _generator) makeDefaultGenerator();
}

DalitzPdfBaseFastInteg::DalitzPdfBaseFastInteg(IDalitzEventList* events
					       , IEventGenerator<IDalitzEvent>*
					          generator
					       //, counted_ptr<IGetDalitzEvent> eff
					       , MinuitParameterSet* mps
					       , double prec
					       ) 
  : PdfBase<IDalitzEvent>(events)
  , _norm(-1)
  , _precision(prec)
  , _amps(0)
  , _countedAmps(0)
    //, _efficiency(eff)
  , _generator(generator)
  , _integrating(0)
  , _defaultGenerator(0)
  , _commaSepList_of_SavedIntegrators("")
{
  MinuitParameterSet *thisMps = (0 == mps ? MinuitParameterSet::getDefaultSet() : mps);
  _amps = new FitAmpSum(this, thisMps);
  counted_ptr<IFastAmplitudeIntegrable> cp(_amps);
  _countedAmps = cp;
  if(0 == _generator) makeDefaultGenerator();
}

DalitzPdfBaseFastInteg::DalitzPdfBaseFastInteg(const DalitzPdfBaseFastInteg& other)
  : IBasicEventAccess<IDalitzEvent>()
  , IEventAccess<IDalitzEvent>()
  , IReturnReal()
  , IGetRealEvent<IDalitzEvent>()
  , IPdf<IDalitzEvent>()
  , IDalitzEventAccess()
  , IDalitzPdf()
  , PdfBase<IDalitzEvent>(other)
  , _norm(other._norm)
  , _precision(other._precision)
  , _amps(other._amps)
  , _countedAmps(other._countedAmps)
    //, _efficiency(other._efficiency)
  , _generator(other._generator)
  , _integrating(other._integrating)
  , _defaultGenerator(other._defaultGenerator)
  , _commaSepList_of_SavedIntegrators(other._commaSepList_of_SavedIntegrators)
{
}

DalitzPdfBaseFastInteg::~DalitzPdfBaseFastInteg(){
}

const IFastAmplitudeIntegrable* DalitzPdfBaseFastInteg::getAmps() const{
  return _amps;
}
IFastAmplitudeIntegrable* DalitzPdfBaseFastInteg::getAmps(){
  return _amps;
}

bool DalitzPdfBaseFastInteg::integrating(){
  return _integrating;
}

double DalitzPdfBaseFastInteg::phaseSpace(){
  IDalitzEvent* evt = getEvent();
  if(0 == evt) return 0;
  return evt->phaseSpace();
}
double DalitzPdfBaseFastInteg::getVal(){
  if(_integrating) return getVal_withPs();
  else return getVal_noPs();
}
double DalitzPdfBaseFastInteg::getVal_noPs(){
  bool dbThis = false;
  static double maxVal=0;

  if(_integrating){
    // shouldn't really do that - use getVal or getVal_withPs
    // when you integrate (automatically done in getVal()):
    return un_normalised_noPs();
  }else{
    if(_norm == -1) getNorm();
    double num = un_normalised_noPs();
    if(dbThis){
      double val=num/_norm;
      if(fabs(val) > maxVal){
	maxVal=fabs(val);
	if(dbThis){
	  cout  << "biggest un_normalised / norm, yet: " 
		<< num << " / " <<_norm 
		<< " = " << num/_norm
		<< ". ps = " << phaseSpace()
		<< endl;
	  cout << "for event: "; getEvent()->print(); cout << endl;
	}
      }
    }

    /*
    cout << "\n DalitzPdfBaseFastInteg::getVal_noPs():"
	 << " for event ptr " << getEvent()
	 << " returning "
	 << num << " / " << _norm << endl;
    */
    return num/_norm;
  }
}
double DalitzPdfBaseFastInteg::getVal_withPs(){

  if(_integrating){
    return un_normalised_noPs()*phaseSpace();
  }else{
    if(_norm == -1) getNorm();
    double num = un_normalised_noPs()*phaseSpace();
    return num/_norm;
  }
}

bool DalitzPdfBaseFastInteg::makePlots(const std::string& filename) const{
  return histoSet().save(filename);
}
DalitzHistoSet DalitzPdfBaseFastInteg::histoSet() const{
  double dN = numEvents();
  if(0 == dN) dN=1;
  cout << " dN = " << dN << endl;
  return (_faint.histoSet() * dN);
}
DalitzHistoSet DalitzPdfBaseFastInteg::histoSet(){
    // non-const version to satisfy IDalitzPdf
    double dN = numEvents();
    if(0 == dN) dN=1;
    cout << " dN = " << dN << endl;
    return (_faint.histoSet() * dN);
}

void DalitzPdfBaseFastInteg::saveEachAmpsHistograms(const std::string& prefix) const{
  _faint.saveEachAmpsHistograms(prefix);
  return;
}

std::vector<DalitzHistoSet> DalitzPdfBaseFastInteg::GetEachAmpsHistograms(){
  return _faint.GetEachAmpsHistograms();
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


double DalitzPdfBaseFastInteg::getVal(IDalitzEvent* evt){
  this->setEvent(evt);
  double result = this->getVal();
  this->resetEventRecord();
  return result;
}

double DalitzPdfBaseFastInteg::getVal_noPs(IDalitzEvent* evt){
  this->setEvent(evt);
  double result = this->getVal_noPs();
  this->resetEventRecord();
  return result;
}

double DalitzPdfBaseFastInteg::getVal_withPs(IDalitzEvent* evt){
  this->setEvent(evt);
  double result = this->getVal_withPs();
  this->resetEventRecord();
  return result;
}


//
