// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:02 GMT
#include "Mint/DalitzPdfNormChecker.h"

#include "Mint/IDalitzPdf.h"
#include "TRandom.h"

using namespace std;
using namespace MINT;

DalitzPdfNormChecker::DalitzPdfNormChecker(IDalitzPdf* pdf
					   , const DalitzEventPattern& pat
					   , double prec
					   , TRandom* rnd)
  : _pdf(pdf)
  , _rnd(rnd)
  , _pat(pat)
  , _t01(2,3,4)
  , _s12(1,2)
  , _s23(2,3)
  , _s34(3,4)
  , _t40(1,2,3)
  , _askingPrecision(prec)
  , _sum(0.0)
  , _sumsq(0.0)
  , _NInside(0)
  , _sumPhaseSpace(0.0)
  , _sumPhaseSpaceSq(0.0)
  , _Nevents(0)
{
  makeMiMa();
}

void DalitzPdfNormChecker::makeMiMa(){
  double safetyFactor=1.001; 

  _s12.setMinMax( _pat.sijMin(1,2)/safetyFactor
		  , _pat.sijMax(1,2)*safetyFactor);
  _s23.setMinMax( _pat.sijMin(2,3)/safetyFactor
		  , _pat.sijMax(2,3)*safetyFactor);
  if(_pat.numDaughters() >= 4){
    _t01.setMinMax( _pat.sijMin(2,3,4)/safetyFactor
		    , _pat.sijMax(2,3,4)*safetyFactor);
    _s34.setMinMax( _pat.sijMin(3,4)/safetyFactor
		    , _pat.sijMax(3,4)*safetyFactor);
    _t40.setMinMax( _pat.sijMin(1,2,3)/safetyFactor
		    , _pat.sijMax(1,2,3)*safetyFactor);
  }

}

counted_ptr<DalitzEvent> DalitzPdfNormChecker::makeEventForOwner( double scale0
							, double scale1
							, double scale2
							, double scale3
							, double scale4
							) const{
  bool dbThis=false;

  double t01 = _t01.min() + scale0*(_t01.max() - _t01.min());
  double s12 = _s12.min() + scale1*(_s12.max() - _s12.min());
  double s23 = _s23.min() + scale2*(_s23.max() - _s23.min());
  double s34 = _s34.min() + scale3*(_s34.max() - _s34.min());
  double t40 = _t40.min() + scale4*(_t40.max() - _t40.min());

  Calculate4BodyProps c4b(t01, s12
			  , s23, s34
			  , t40
			  , _pat[0].mass()
			  , _pat[1].mass(), _pat[2].mass()
			  , _pat[3].mass(), _pat[4].mass()
			  );
  if(c4b.phaseSpaceFactor() <=0){
    if(dbThis){
      cout << " for \n  " 
	   << t01 << "( " << _t01 << "), \n  "
	   << s12 << "( " << _s12 << "), \n  "
	   << s23 << "( " << _s23 << "), \n  "
	   << s34 << "( " << _s34 << "), \n  "
	   << t40 << "( " << _t40 << "), \n  "
	   << " outside phase space!! "
	   << endl;
    }
    return counted_ptr<DalitzEvent>(0);
  }
  counted_ptr<DalitzEvent> evt(new DalitzEvent(_pat, t01, s12, s23, s34, t40));
  if(0 == evt){
    cout << "WARNING DalitzPdfNormChecker::make4Event()"
	 << " making the event failed at 'new DalitzEvent(...)'"
	 << " this shouldn't really happen."
	 << endl;
    return evt;
  }
  if(evt->phaseSpace() <= 0){
    return counted_ptr<DalitzEvent>(0);
  }
  
  return evt;
}

counted_ptr<DalitzEvent> DalitzPdfNormChecker::makeEventForOwner() const{
  if(_pat.numDaughters() == 3) return make3Event();
  if(_pat.numDaughters() == 4) return make4Event();
  cout << "ERROR in DalitzPdfNormChecker::makeEvent() can only make events"
       << " with 3 or 4 daughters. You want : " << _pat
       << endl;
  return counted_ptr<DalitzEvent>(0);
}

counted_ptr<DalitzEvent> DalitzPdfNormChecker::make3Event() const{

  cout << " ERROR DalitzPdfNormChecker::make3Event(): I don't work, yet. "
       << "\n    Please improve me. Sorry, will have to crash, now"
       << endl;
  throw "no making 3 body events in DalitzPdfNormChecker, yet";

  /*
  double s12 = _s12.min() + _rnd->Rndm()*(_s12.max() - _s12.min());
  double s23 = _s23.min() + _rnd->Rndm()*(_s23.max() - _s23.min());

  _evt = new DalitzEvent(_pat, s12, s23);
  if(_evt->phaseSpace() <= 0){
    deleteEvent();
    return 0;
  }
  
  return _evt;
  */
  return counted_ptr<DalitzEvent>(0);
}
counted_ptr<DalitzEvent> DalitzPdfNormChecker::make4Event() const{
  //counted_ptr<DalitzEvent> evtttt(new DalitzEvent(_pat, _rnd));
  //evtttt->setGeneratorPdfRelativeToPhaseSpace(evtttt->phaseSpace());
  //return evtttt;

  return makeEventForOwner( _rnd->Rndm()
			    , _rnd->Rndm()
			    , _rnd->Rndm()
			    , _rnd->Rndm()
			    , _rnd->Rndm()
			    );
}


double DalitzPdfNormChecker::currentAbsolutePrecision()const{
  if(_Nevents <= 0) return 9999;
  double dN = (double) _Nevents;
  double mean = _sum/dN;
  double meansq = _sumsq/dN;
  double var = meansq - mean*mean;
  if(var < 0){
    cout << "ERROR ind DalitzPdfNormChecker::currentPrecision()"
	 << ". Variance = " << var
	 << ". We have a problem."
	 << endl;
    return 9999;
  }
  return sqrt(var/dN)*boxSize(); // sigma on mean.
}
double DalitzPdfNormChecker::currentRelativePrecision()const{
  if(_Nevents <= 0) return 9999;
  if(result() == 0.0) return 9999;
  return currentAbsolutePrecision()/result();
}

double DalitzPdfNormChecker::resultError() const{
  return currentAbsolutePrecision();
}

bool DalitzPdfNormChecker::sufficientPrecision() const{
  return _Nevents > 1000 && currentRelativePrecision() < _askingPrecision;
}
double DalitzPdfNormChecker::checkNorm(){

  long unsigned int stupidNumber = 2000000000;
  
  int printEvery=50000;
  for(long unsigned int i=0; 
      ( i < stupidNumber && (! sufficientPrecision())); 
      i++
      ){
    counted_ptr<DalitzEvent> evtPtr(makeEventForOwner());
    if(0 == i%printEvery){
      print();
      cout << endl;
      cout << " current ptr  " << evtPtr.get() << endl;
      if(0 != evtPtr){
	cout << " phase space = " << evtPtr->phaseSpace() << endl;
      }
    }
    _Nevents++;
    if(0 == evtPtr) continue;
    if(evtPtr->phaseSpace() <= 0) continue;
    double val = _pdf->getVal_withPs(*evtPtr);
    if(_Nevents < 20 || 0 == i%printEvery){
      cout << " for _Nevents = " << _Nevents << ", current val = " << val << endl;
      cout << "event: " << *evtPtr << endl;
      cout << " ------------------ " << endl;
    }
    
    _sum   += val;
    _sumsq += val*val;

    double ps = evtPtr->phaseSpace();
    _NInside++;
    _sumPhaseSpace   += ps;
    _sumPhaseSpaceSq += ps*ps;
  }
  if(_Nevents <=0) return -9999;
  
  cout << "DalitzPdfNormChecker::checkNorm(): "
       << "\n\t result: ";
  print();
  cout << endl;
  
  return result();
}

void DalitzPdfNormChecker::print(std::ostream& os)const{
  os << " DalitzPdfNormChecker after " << _Nevents  << " events: ";
  if(_Nevents <= 0) return;
  os << "\n\t result: " << result() << " +/- " << resultError();
  double dN = (double) _Nevents;
  //  double shouldBe = dInside/(dN);
  double shouldBe = _sumPhaseSpace/dN * boxSize();
  //shouldBe *= _sumPhaseSpace/dN;
  //shouldBe = boxSize();
  os << "\n this should be " << shouldBe;
}

double DalitzPdfNormChecker::boxSize() const{
  //  return 1;
  double dt01 = (_t01.max() - _t01.min());
  double ds12 = (_s12.max() - _s12.min());
  double ds23 = (_s23.max() - _s23.min());
  double ds34 = (_s34.max() - _s34.min());
  double dt40 = (_t40.max() - _t40.min());

  if(_pat.numDaughters() == 3){
    return ds12*ds23;
  }else if(_pat.numDaughters() == 4){
    return dt01*ds12*ds23*ds34*dt40;
  }else{
    cout << "ERROR in DalitzPdfNormChecker::boxSize()"
	 << " don't know how to handle "
	 << _pat.numDaughters()
	 << " final state particles. Will return -9999"
	 << endl;
    return -9999.;
  }
}
 
double DalitzPdfNormChecker::result() const{
  if(_Nevents <=0 )return -9999;
  return _sum/((double)_Nevents) * boxSize();
}


std::ostream& operator<<(std::ostream& os, const DalitzPdfNormChecker& nc){
  nc.print(os);
  return os;
}
