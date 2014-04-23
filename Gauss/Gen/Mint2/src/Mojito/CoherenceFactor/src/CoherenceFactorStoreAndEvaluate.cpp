// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:59 GMT
//
// Using Equations 5 (and around that) in
// Phys. Rev. D 68, 033003 (2003), 
// http://prola.aps.org/abstract/PRD/v68/i3/e033003
//
#include "Mint/CoherenceFactorStoreAndEvaluate.h"
#include "Mint/FitAmpSum.h"
#include "Mint/IReturnRealForEvent.h"
#include "Mint/IReturnComplexForEvent.h"
#include "Mint/CLHEPPhysicalConstants.h"

using namespace std;
using namespace MINT;

CoherenceFactorStoreAndEvaluate::
CoherenceFactorStoreAndEvaluate(FitAmpSum& A
				, FitAmpSum& Abar
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
  , _eff(eff)
  , _precision(prec)
  , _sumASq(0), _sumASqSquared(0)
  , _sumAbarSq(0), _sumAbarSqSquared(0)
  , _sumAxAbarStar(0,0), _sumAxAbarStarSquared(0,0)
  , _histoA(), _histoAbar(), _histoBoth()
  , _tries(0)
  , _Nevents(0)
{
}

double CoherenceFactorStoreAndEvaluate::getEff(IDalitzEvent& evt){
  if(0 == _eff) return 1.0;
  return _eff->RealVal(evt);
}
double CoherenceFactorStoreAndEvaluate::estimatedPrecision() const{
  if(_Nevents <= 0) return 9999;
  double s = sigmaAbs();
  if(s < 0) s *= -1;
  return s;
}

complex<double> CoherenceFactorStoreAndEvaluate::integAxAbarStar()const{
  if(_Nevents <=0) return 0;
  return _sumAxAbarStar / ((double) _Nevents);
}
double CoherenceFactorStoreAndEvaluate::integA()const{
  if(_Nevents <=0) return 0;
  if(_sumASq <=0) return -9999;
  return sqrt(_sumASq / ((double) _Nevents));
}
double CoherenceFactorStoreAndEvaluate::integAbar()const{
  if(_Nevents <=0) return 0;
  if(_sumAbarSq <=0) return -9999;
  return sqrt(_sumAbarSq / ((double) _Nevents));
}
double CoherenceFactorStoreAndEvaluate::integASq()const{
  if(_Nevents <=0) return 0;
  return _sumASq / ((double) _Nevents);
}
double CoherenceFactorStoreAndEvaluate::integAbarSq()const{
  if(_Nevents <=0) return 0;
  return _sumAbarSq / ((double) _Nevents);
}

complex<double> CoherenceFactorStoreAndEvaluate::cvarAxAbarStar()const{
  return complexVar(_sumAxAbarStar, _sumAxAbarStarSquared);
}
double CoherenceFactorStoreAndEvaluate::realAorAbarVar(double s, double sq)const{
  if(_Nevents <=0 ) return 0;
  if(s == 0) return -9999;
  double   m = s/((double) _Nevents);
  double msq = sq/((double) _Nevents); 
  double varAsq = (msq - m*m)/((double) _Nevents);
  
  return 0.5 * varAsq / fabs(s);
}
double CoherenceFactorStoreAndEvaluate::varA()const{
  return realAorAbarVar(_sumASq, _sumASqSquared);
}
double CoherenceFactorStoreAndEvaluate::varAbar()const{
  return realAorAbarVar(_sumAbarSq, _sumAbarSqSquared);
}


double CoherenceFactorStoreAndEvaluate::varASq()const{
  if(_Nevents <=0) return -1;
  double mean     = _sumASq/_Nevents;
  double meanOfSq = _sumASqSquared/_Nevents;
  double v = meanOfSq - mean*mean;
  return v/_Nevents;
}
double CoherenceFactorStoreAndEvaluate::varAbarSq()const{
  if(_Nevents <=0) return -1;
  double mean     = _sumAbarSq/_Nevents;
  double meanOfSq = _sumAbarSqSquared/_Nevents;
  double v = meanOfSq - mean*mean;
  return v/_Nevents;
}

complex<double> CoherenceFactorStoreAndEvaluate::complexVar(complex<double> s
							    , complex<double> s_sq
							    )const{
  if(_Nevents <=0) return 0;
  complex<double> m   = s        / ((double) _Nevents);
  complex<double> mos = s_sq / ((double) _Nevents);
  complex<double> som(m.real()*m.real(), m.imag()*m.imag());
  return (mos - som)/((double)_Nevents);
}

complex<double> CoherenceFactorStoreAndEvaluate::var()const{
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

complex<double> CoherenceFactorStoreAndEvaluate::sigma() const{
  return complex<double>(sqrt(fabs(var().real())), sqrt(fabs(var().imag())));
}
double CoherenceFactorStoreAndEvaluate::varAbs()const{
  double a = var().real() * 2*fabs(Rval().real()/norm(Rval()));
  double b = var().imag() * 2*fabs(Rval().imag()/norm(Rval()));
  return a+b;
}
double CoherenceFactorStoreAndEvaluate::sigmaAbs()const{
  double va = varAbs();
  if(va < 0) return -9999;
  return sqrt(va);
}
double CoherenceFactorStoreAndEvaluate::varAngle()const{
  double a= fabs(Rval().real());
  double b= fabs(Rval().imag());
  double den = a*a + b*b;
  if(den <= 0)return -9999;
  
  return var().real()*b/den + var().imag()*a/den;
}
double CoherenceFactorStoreAndEvaluate::varAngleDeg()const{
  return varAngle() * 180*180/(pi*pi);
}
double CoherenceFactorStoreAndEvaluate::sigmaAngle()const{
  double va = varAngle();
  if(va < 0) return -9999;
  return sqrt(va);
}
double CoherenceFactorStoreAndEvaluate::sigmaAngleDeg()const{
  double va = varAngleDeg();
  if(va < 0) return -9999;
  return sqrt(va);
}
complex<double> CoherenceFactorStoreAndEvaluate::Rval() const{
  if(_Nevents <= 0 || integA() <= 0 || integAbar() <= 0) return -9999;
  return integAxAbarStar() / (integA() * integAbar());
}

void CoherenceFactorStoreAndEvaluate::printLong(std::ostream& os) const{
  os << "Integrals from CoherenceFactorStoreAndEvaluate\n"
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
void CoherenceFactorStoreAndEvaluate::print(std::ostream& os
					    , const std::string& pref)const{
  os << pref << "\t int |A| = " << this->integA()
     << "\t int |Abar| = " << this->integAbar()
     << "\n" << pref << "\t int (A Abar*) = " << this->integAxAbarStar()
     << '\n' << pref << "\tR exp(i xi) = " << abs(Rval()) 
     << " * exp( i * " << arg(Rval())*180.0/pi << " deg )"
     << "\n" << pref << "\t     sig(R) = " << sigmaAbs()
     << "  sig(xi) = " << sigmaAngle()*180.0/pi << " deg";
}
double CoherenceFactorStoreAndEvaluate::absR() const{
  return abs(Rval());
}
double CoherenceFactorStoreAndEvaluate::phaseR() const{
  return arg(Rval());
}
double CoherenceFactorStoreAndEvaluate::phaseRdeg() const{
  return arg(Rval())*180.0/pi;
}
complex<double> CoherenceFactorStoreAndEvaluate::A_Value(IDalitzEvent& evt){
  return A()->ComplexVal(evt);
}
complex<double> CoherenceFactorStoreAndEvaluate::Abar_Value(IDalitzEvent&
							    evt){
  bool dbThis=false;
  if(dbThis) cout << "CoherenceFactorStoreAndEvaluate::Abar_Value" << endl;
  complex<double> val = Abar()->ComplexVal(evt);
  if(dbThis) cout << "got value: " << val << endl;
  return val;
}

bool CoherenceFactorStoreAndEvaluate::addEvent(counted_ptr<IDalitzEvent>& 
					       evtPtr){
  bool dbThis=false;
  if(dbThis) cout << "Hello from CoherenceFactorStoreAndEvaluate::getCVal()" 
		  << endl;

  //  int printEvery = 10000;

  _tries++;
  bool printout = false;//_tries < 10 || (0 == _tries%printEvery);
  
  if(0 == evtPtr){
    if(dbThis) cout << "WARNING in CoherenceFactorStoreAndEvaluate::getR()"
		    << " got evtPtr = " << evtPtr << endl;
    return false;
  }

  if(10 == _tries){
    cout << "=======================\n";
    cout << "all A amplitudes\n";
    _A->printAllAmps();
    cout << "all Abar ampltiudes\n";
    _Abar->printAllAmps();
    cout <<endl;
  }
  double w = evtPtr->getWeight()
    /evtPtr->getGeneratorPdfRelativeToPhaseSpace();
  
  w*= getEff(evtPtr);
  
  w = sqrt(w); // sqrt because we apply the weight to the amplitudes, 
  //              while the integrals (as one might expect) all 
  //              contain terms |A|^2, |Abar|^2, or |A*Abar|.
  
  complex<double> c_a=
    A_Value(evtPtr.get()) * w;
  
  complex<double> c_abar=
    Abar_Value(evtPtr.get()) *w*polar(_CSAbs, _CSPhase);
  
  double d_a = norm(c_a);
  double d_abar = norm(c_abar);

  _histoA.addEvent(*evtPtr, d_a);
  _histoAbar.addEvent(*evtPtr, d_abar);
  _histoBoth.addEvent(*evtPtr, norm(c_a + c_abar));
  
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
    cout << "INFO from CoherenceFactorStoreAndEvaluate::getR()"
	 << "\n\t _tries: " << _tries << ",  for " << _Nevents << " events."
	 << endl;
    // << "\n\t c_a, c_abar: " << c_a << ", " << c_abar
    //  << "\n\t value: " << Rval()
    //  << " = " << abs(Rval()) << " * exp( i * " << arg(Rval()) << " )"
    //  << " +/- " << estimatedPrecision()
    //  << endl;
    print();
    cout << " --------------------------------- " << endl;
  }
  return true;
}

const DalitzHistoSet& CoherenceFactorStoreAndEvaluate::getHistoA() const{
  return _histoA;}
const DalitzHistoSet& CoherenceFactorStoreAndEvaluate::getHistoAbar() const{
  return _histoAbar;}
const DalitzHistoSet& CoherenceFactorStoreAndEvaluate::getHistoBoth() const{
  return _histoBoth;}


bool CoherenceFactorStoreAndEvaluate::
saveHistos(const std::string& fname) const{
  bool s = true;
  s &= _histoA.save(fname + "_A.root");
  s &= _histoAbar.save(fname + "_Abar.root");
  s &= _histoBoth.save(fname + "_Both.root");
  return s;
}

bool CoherenceFactorStoreAndEvaluate::
drawHistos(const std::string& fname) const{
  bool s=true;
  s &= _histoA.draw(fname + "_A");
  s &= _histoAbar.draw(fname + "_Abar");
  s &= _histoBoth.draw(fname + "_Both");
  return s;
}
bool CoherenceFactorStoreAndEvaluate::
drawHistosWith(const CoherenceFactorStoreAndEvaluate& other
	       , const std::string& fname
	       , double sf
	       ) const{
  bool s=true;
  s &= _histoA.drawWithFit(other._histoA * sf, fname + "_A");
  s &= _histoAbar.drawWithFit(other._histoAbar * sf, fname + "_Abar");
  s &= _histoBoth.drawWithFit(other._histoBoth * sf, fname + "_Both");
  return s;
}
bool CoherenceFactorStoreAndEvaluate::
drawHistoRatios(const CoherenceFactorStoreAndEvaluate& other
	       , const std::string& fname
	       , double sf
	       ) const{
  bool s=true;
  s &= (_histoA    / (other._histoA    * sf)).draw(fname + "_ARatio");
  s &= (_histoAbar / (other._histoAbar * sf)).draw(fname + "_AbarRatio");
  s &= (_histoBoth / (other._histoBoth * sf)).draw(fname + "_BothRatio");
  return s;
}
void CoherenceFactorStoreAndEvaluate::
scaleHistos(double sf){
  _histoA.scale(sf);
  _histoAbar.scale(sf);
  _histoBoth.scale(sf);
}
