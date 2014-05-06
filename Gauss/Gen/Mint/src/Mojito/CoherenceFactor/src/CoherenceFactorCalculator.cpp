// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:59 GMT
//
// Using Equations 5 (and around that) in
// Phys. Rev. D 68, 033003 (2003), 
// http://prola.aps.org/abstract/PRD/v68/i3/e033003
//
#include "Mint/CoherenceFactorStoreAndEvaluate.h"
#include "Mint/CoherenceFactorCalculator.h"
#include "Mint/FitAmpSum.h"
#include "Mint/IGetRealEvent.h"
#include "Mint/IGetComplexEvent.h"
#include "Mint/CLHEPPhysicalConstants.h"

#include <cstdio>

using namespace std;
using namespace MINT;

CoherenceFactorCalculator::
CoherenceFactorCalculator( FitAmpSum& A, FitAmpSum& Abar
			   , double CSAbs
			   , double CSPhase
			   , IGetRealEvent<IDalitzEvent>* eff
			   , double prec
			   , const std::string& name
			  )
  : _A_plus_Abar(A+Abar)
  , _myOwnGenAplusAbar(0)
  , _precision(prec)
  , _name(name)
{
   counted_ptr< CoherenceFactorStoreAndEvaluate>
     ptr_flat(new CoherenceFactorStoreAndEvaluate(A, Abar
						  , CSAbs, CSPhase
						  , 0, prec));
   _cfList.push_back(ptr_flat);

   counted_ptr< CoherenceFactorStoreAndEvaluate>
     ptr_eff(new CoherenceFactorStoreAndEvaluate(A, Abar
						 , CSAbs, CSPhase
						 , eff, prec));
   _cfList.push_back(ptr_eff);
 
}

counted_ptr< IEventGenerator<IDalitzEvent> > 
CoherenceFactorCalculator::getGenerator(){
  bool dbThis=false;
  if(0 == _myOwnGenAplusAbar){
    if(dbThis) cout << "CoherenceFactorCalculator::getGenerator():  "
		    << "making generator" << endl;
    counted_ptr< IEventGenerator<IDalitzEvent> > 
      ptr( new DalitzBWBoxSet(_A_plus_Abar.makeBWBoxes()));
    if(dbThis) cout << "made generator." << endl;
    _myOwnGenAplusAbar = ptr;
  }
  return _myOwnGenAplusAbar;
}
counted_ptr<IDalitzEvent> CoherenceFactorCalculator::newEvent(){
  return getGenerator()->newEvent();
}

double CoherenceFactorCalculator::effA() const{
  if(_cfList.size() < 2) return 1;
  double d = _cfList[0]->integASq();
  if(d <=0){
    return -1;
  }else{
    return _cfList[1]->integASq()/d;
  }
}
double CoherenceFactorCalculator::effAbar() const{
  if(_cfList.size() < 2) return 1;
  double d = _cfList[0]->integAbarSq();
  if(d <=0){
    return -1;
  }else{
    return _cfList[1]->integAbarSq()/d;
  }
}

double CoherenceFactorCalculator::effAVar() const{
  double d = _cfList[0]->integASq();
  if(d <=0){
    return -1;
  }else{
    double vn = _cfList[1]->varASq();
    double vd = _cfList[0]->varASq();
    return vn/(d*d) + effA()*effA()*vd/(d*d);
  }
}
double CoherenceFactorCalculator::effAbarVar() const{
  double d = _cfList[0]->integAbarSq();
  if(d <=0){
    return -1;
  }else{
    double vn = _cfList[1]->varAbarSq();
    double vd = _cfList[0]->varAbarSq();
    return vn/(d*d) + effAbar()*effAbar()*vd/(d*d);
  }
}
double CoherenceFactorCalculator::effASigma() const{
  double v = effAVar();
  if(v < 0) return -1;
  return sqrt(v);
}
double CoherenceFactorCalculator::effAbarSigma() const{
  double v = effAbarVar();
  if(v < 0) return -1;
  return sqrt(v);
}
void CoherenceFactorCalculator::printResult(std::ostream& os) const{
  double R0 = -9999;
  if(_cfList.size() >=2){
    R0 = _cfList[0]->absR();
    os << "Efficiencies from CoherenceFactorCalculater\n";
    os << " A: " << effA() << " +/- " << effASigma();
    os << ", Abar: " << effAbar() << " +/- " << effAbarSigma();
  }
  os << "\n Coherence Factors";
  for(unsigned int i=0; i < _cfList.size(); i++){
    if(0 == i){
      os << "\n * No efficiency effects "
    	 << " - 100\% efficient detector\n";
    }else{
      os << "\n * Including efficiency effects\n";
    }
    _cfList[i]->print(os, "\t" + anythingToString((int) i) + ")");
    if(i > 0){
      os << "\n ....difference..relative..to..no..efficiency..effects:....";
      os << "\n              diff R=  " << _cfList[i]->absR() - R0
	 << " +/- " << sqrt(_cfList[i]->varAbs() + _cfList[0]->varAbs());
      if(R0 > 0){
	os << "\n     relative diff R=  " 
	   << ((_cfList[i]->absR() - _cfList[0]->absR())/R0)
	   << " +/- " << (sqrt(_cfList[i]->varAbs() + _cfList[0]->varAbs())/R0);
	  os << "\n          diff phase=  " 
	     << _cfList[i]->phaseRdeg() - _cfList[0]->phaseRdeg()
	     << " +/- " << sqrt(_cfList[i]->varAngleDeg() + _cfList[0]->varAngleDeg())
	     << "\n";
      }
    }
  }
  os << endl;
}

complex<double> CoherenceFactorCalculator::evaluate(){
  bool dbThis=false;
  if(dbThis) cout << "Hello from CoherenceFactorCalculator::evaluate()" << endl;

  long int rediculousNumber = 1000000000;
  int tries=0;
  int printEvery      =  10000;
  int makeHistosEvery = 100000;
  while(estimatedPrecision() > _precision && tries < rediculousNumber){
    for(int i=0; i < 1000; i++){
      tries++;
      bool printout = tries < 10 || (0 == tries%printEvery);
      bool makeHistos = (0 == tries%makeHistosEvery);
      counted_ptr<IDalitzEvent> evtPtr(newEvent());
      if(0 == evtPtr){
	if(dbThis) cout << "WARNING in CoherenceFactor::getR()"
			<< " got evtPtr = " << evtPtr << endl;
	continue;
      }

      for(unsigned int i=0; i < _cfList.size(); i++){
	_cfList[i]->addEvent(evtPtr);
      }
      
      if(printout){
	cout << " DalitzEvent::EventCounter() "
	     << DalitzEvent::eventCounter()
	     << endl;
	cout << "INFO from CoherenceFactor::getR()"
	     << "\n\t tries: " << tries
	     << endl;
	  // << "\n\t c_a, c_abar: " << c_a << ", " << c_abar
	  //  << "\n\t value: " << Rval()
	  //  << " = " << abs(Rval()) << " * exp( i * " << arg(Rval()) << " )"
	  //  << " +/- " << estimatedPrecision()
	  //  << endl;
	printResult();
	cout << " --------------------------------- " << endl;
      }
      if(makeHistos){
	/*
	for(unsigned int i=0; i < _cfList.size(); i++){
	  char fname[1000];
	  sprintf(fname, "%s_%d", _name.c_str(), i);
	  _cfList[i]->saveHistos(fname);
	  _cfList[i]->drawHistos(fname);
	}
	*/
	if(_cfList.size() >=2){
	  _cfList[1]->drawHistosWith(*_cfList[0], _name + "AmpCompare", effA());
	  _cfList[1]->drawHistoRatios(*_cfList[0], _name + "Eff");
	}
      }
    }
  }
  return _cfList[1]->Rval();
}

void CoherenceFactorCalculator::setPrecision(double prec){
  _precision = prec;
  for(unsigned int i=0; i < _cfList.size(); i++){
    _cfList[i]->setPrecision(prec);
  }
}
double CoherenceFactorCalculator::estimatedPrecision() const{
  double prec = -9999;
  for(unsigned int i=0; i < _cfList.size(); i++){
    double tp = _cfList[i]->estimatedPrecision();
    if(tp > prec) prec = tp;
  }
  return prec;
}
