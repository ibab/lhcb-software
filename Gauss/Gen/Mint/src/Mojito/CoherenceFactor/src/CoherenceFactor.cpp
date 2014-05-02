// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:59 GMT
//
// Using Equations 5 (and around that) in
// Phys. Rev. D 68, 033003 (2003), 
// http://prola.aps.org/abstract/PRD/v68/i3/e033003
//
#include "Mint/CoherenceFactor.h"
#include "Mint/FitAmpSum.h"
#include "Mint/IReturnRealForEvent.h"
#include "Mint/IReturnComplexForEvent.h"
#include "Mint/CLHEPPhysicalConstants.h"

using namespace std;
using namespace MINT;

CoherenceFactor::CoherenceFactor(FitAmpSum& A, FitAmpSum& Abar
				 , double CSAbs
				 , double CSPhase
				 , IReturnRealForEvent<IDalitzEvent>* eff
				 , double prec
				 )
  : _A(&A)
  , _Abar(&Abar)
  , _CSAbs(CSAbs)
  , _CSPhase(CSPhase)
  , _A_plus_Abar(A+Abar)
  , _myOwnGenAplusAbar(0)
  , _eff(eff)
  , _precision(prec)
  , _sumASq(0), _sumASqSquared(0)
  , _sumAbarSq(0), _sumAbarSqSquared(0)
  , _sumAxAbarStar(0,0), _sumAxAbarStarSquared(0,0)
  , _Nevents(0)
{
}

counted_ptr< IEventGenerator<IDalitzEvent> > 
CoherenceFactor::getGenerator(){
  bool dbThis=false;
  if(0 == _myOwnGenAplusAbar){
    if(dbThis) cout << "CoherenceFactor::getGenerator():  "
		    << "making generator" << endl;
    if(_A_plus_Abar.size() <= 0) return 0;
    DalitzEventPattern pat = _A_plus_Abar.getAmpPtr(0)->getTreePattern();
    counted_ptr< IEventGenerator<IDalitzEvent> > 
      ptr( new DalitzBWBoxSet(_A_plus_Abar.makeBWBoxes(pat)));
    if(dbThis) cout << "made generator." << endl;
    _myOwnGenAplusAbar = ptr;
  }
  return _myOwnGenAplusAbar;
}
counted_ptr<IDalitzEvent> CoherenceFactor::newEvent(){
  return getGenerator()->newEvent();
}

double CoherenceFactor::getEff(IDalitzEvent& evt){
  if(0 == _eff) return 1.0;
  return _eff->RealVal(evt);
}
double CoherenceFactor::estimatedPrecision() const{
  if(_Nevents <= 0) return 9999;
  double s = sigmaAbs();
  if(s < 0) s *=-1;
  return s;
}

complex<double> CoherenceFactor::integAxAbarStar()const{
  if(_Nevents <=0) return 0;
  return _sumAxAbarStar / ((double) _Nevents);
}
double CoherenceFactor::integA()const{
  if(_Nevents <=0) return 0;
  if(_sumASq <=0) return -9999;
  return sqrt(_sumASq / ((double) _Nevents));
}
double CoherenceFactor::integAbar()const{
  if(_Nevents <=0) return 0;
  if(_sumAbarSq <=0) return -9999;
  return sqrt(_sumAbarSq / ((double) _Nevents));
}
double CoherenceFactor::integASq()const{
  if(_Nevents <=0) return 0;
  return _sumASq / ((double) _Nevents);
}
double CoherenceFactor::integAbarSq()const{
  if(_Nevents <=0) return 0;
  return _sumAbarSq / ((double) _Nevents);
}

complex<double> CoherenceFactor::cvarAxAbarStar()const{
  return complexVar(_sumAxAbarStar, _sumAxAbarStarSquared);
}
double CoherenceFactor::realAorAbarVar(double s, double sq)const{
  if(_Nevents <=0 ) return 0;
  if(s == 0) return -9999;
  double   m = s/((double) _Nevents);
  double msq = sq/((double) _Nevents); 
  double varAsq = (msq - m*m)/((double) _Nevents);
  
  return 0.5 * varAsq / fabs(s);
}
double CoherenceFactor::varA()const{
  return realAorAbarVar(_sumASq, _sumASqSquared);
}
double CoherenceFactor::varAbar()const{
  return realAorAbarVar(_sumAbarSq, _sumAbarSqSquared);
}

complex<double> CoherenceFactor::complexVar(complex<double> s
					    , complex<double> s_sq
					    )const{
  if(_Nevents <=0) return 0;
  complex<double> m   = s        / ((double) _Nevents);
  complex<double> mos = s_sq / ((double) _Nevents);
  complex<double> som(m.real()*m.real(), m.imag()*m.imag());
  return (mos - som)/((double)_Nevents);
}


complex<double> CoherenceFactor::var()const{
  bool dbThis=false;

  complex<double> m   = Rval();
  complex<double> som(m.real()*m.real(), m.imag()*m.imag());
  
  double den = integA()*integA() * integAbar()*integAbar();
  if(den <= 0) return complex<double>(-9999,-9999);

  double ar = cvarAxAbarStar().real() / den;
  double ai = cvarAxAbarStar().imag() / den;
  complex<double> a(ar, ai);
  complex<double> b = varA()/(integA()*integA()) * som;
  complex<double> c = varAbar()/(integAbar()*integAbar()) * som;

  if(dbThis){
    cout << " cvarAxAbarStar() << " << cvarAxAbarStar() << endl;
    cout << "var() = " << a << " + " << b << " + " << c
	 << "  = " << a + b + c << endl;
  }
  return a + b + c;
}

complex<double> CoherenceFactor::sigma() const{
  return complex<double>(sqrt(fabs(var().real())), sqrt(fabs(var().imag())));
}
double CoherenceFactor::varAbs()const{
  double a = var().real() * 2*fabs(Rval().real()/norm(Rval()));
  double b = var().imag() * 2*fabs(Rval().imag()/norm(Rval()));
  return a+b;
}
double CoherenceFactor::sigmaAbs()const{
  double va = varAbs();
  if(va < 0) return -9999;
  return sqrt(va);
}
double CoherenceFactor::varAngle()const{
  double a= fabs(Rval().real());
  double b= fabs(Rval().imag());
  double den = a*a + b*b;
  if(den <= 0)return -9999;
  
  return var().real()*b/den + var().imag()*a/den;
}
double CoherenceFactor::sigmaAngle()const{
  double va = varAngle();
  if(va < 0) return -9999;
  return sqrt(va);
}
complex<double> CoherenceFactor::Rval() const{
  if(_Nevents <= 0 || integA() <= 0 || integAbar() <= 0) return -9999;
  return integAxAbarStar() / (integA() * integAbar());
}

void CoherenceFactor::printResult(std::ostream& os) const{
  os << "Integrals from CoherenceFactor\n"
     << "\t int |A|^2: " << integASq() 
     << ", int |Abar|^2 " << integAbarSq();
  double rBSq = _CSAbs*_CSAbs;
  if(rBSq > 0){
    os << "\n\t int |Abar|^2/rB^2 " << integAbarSq()/rBSq;
  }
  if(integAbarSq() > 0){
    os << "\n ratio int |A|^2 / int |Abar|^2 = " << integASq()/integAbarSq()
       << ", ratio * rB^2 = " << rBSq*integASq()/integAbarSq();
  }
  os << "\n\n Coherence factor result (same result, different formats):"
     << "\n\tR exp(i xi) = " << Rval() << " +/- " << sigma()
     << "\n\tR exp(i xi) = " << abs(Rval()) 
     << " * exp( i * " << arg(Rval()) << " )"
     << "\n\t     sig(R) = " << sigmaAbs()
     << "  sig(xi) = " << sigmaAngle()
     << "\n\tR exp(i xi) = " << abs(Rval()) 
     << " * exp( i * " << arg(Rval())*180.0/pi << " deg )"
     << "\n\t     sig(R) = " << sigmaAbs()
     << "  sig(xi) = " << sigmaAngle()*180.0/pi << " deg";
  if(arg(Rval()) < 0){
    cout  << "\n\tR exp(i xi) = " << abs(Rval()) 
	  << " * exp( i * " << arg(Rval())*180.0/pi + 360. << " deg )"
	  << "\n\t     sig(R) = " << sigmaAbs()
	  << "  sig(xi) = " << sigmaAngle()*180.0/pi << " deg";
  }
  cout << endl;
}
complex<double> CoherenceFactor::A_Value(IDalitzEvent& evt){
  //  bool dbThis=false;
  return A()->ComplexVal(evt);
}
complex<double> CoherenceFactor::Abar_Value(IDalitzEvent& evt){
  bool dbThis=false;
  if(dbThis) cout << "CoherenceFactor::Abar_Value" << endl;
  complex<double> val = Abar()->ComplexVal(evt);
  if(dbThis) cout << "got value: " << val << endl;
  return val;
}

complex<double> CoherenceFactor::getCVal(){
  bool dbThis=false;
  if(dbThis) cout << "Hello from CoherenceFactor::getCVal()" << endl;
  long int rediculousNumber = 1000000000;
  long int tries=0;

  int printEvery = 10000;
  
  while(estimatedPrecision() > _precision && tries < rediculousNumber){
    for(int i=0; i < 1000; i++){
      tries++;
      bool printout = tries < 10 || (0 == tries%printEvery);
      
      counted_ptr<IDalitzEvent> evtPtr(newEvent());
      
      if(0 == evtPtr){
	if(dbThis) cout << "WARNING in CoherenceFactor::getR()"
			<< " got evtPtr = " << evtPtr << endl;
	continue;
      }
      //cout << "evtPtr->phaseSpace() " << evtPtr->phaseSpace() << endl;

      double w = evtPtr->getWeight()
	/evtPtr->getGeneratorPdfRelativeToPhaseSpace();
      
      w*= getEff(*evtPtr);
      
      w = sqrt(w); // since we apply the weight to the amplitudes, 
      //              while the integrals (as one might expect) all 
      //              contain terms |A|^2, |Abar|^2, or |A*Abar|.

      complex<double> c_a=
	A_Value(*evtPtr) * w;

      complex<double> c_abar=
	Abar_Value(*evtPtr) *w*polar(_CSAbs, _CSPhase);
      
      double d_a    = norm(c_a);
      double d_abar = norm(c_abar);

      _sumASq += d_a;
      _sumASqSquared += d_a*d_a;

      _sumAbarSq += d_abar;
      _sumAbarSqSquared += d_abar*d_abar;
      
      complex<double> c_prod = c_a * conj(c_abar);
      _sumAxAbarStar += c_prod;
      complex<double> c_prodSquared(c_prod.real() * c_prod.real()
				    , c_prod.imag() * c_prod.imag()
				    );
      _sumAxAbarStarSquared += c_prodSquared;
      
      _Nevents++;
      
      if(printout){
	cout << " DalitzEvent::EventCounter() "
	     << DalitzEvent::eventCounter()
	     << endl;
	cout << "INFO from CoherenceFactor::getR()"
	     << "\n\t tries: " << tries << ",  for " << _Nevents << " events."
	     << endl;
	  // << "\n\t c_a, c_abar: " << c_a << ", " << c_abar
	  //  << "\n\t value: " << Rval()
	  //  << " = " << abs(Rval()) << " * exp( i * " << arg(Rval()) << " )"
	  //  << " +/- " << estimatedPrecision()
	  //  << endl;
	printResult();
	cout << " --------------------------------- " << endl;
      }
    }
  }
  return Rval();
}
