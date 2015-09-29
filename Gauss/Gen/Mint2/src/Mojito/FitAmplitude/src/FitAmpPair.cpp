// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT
#include "Mint/FitAmpPair.h"
#include "Mint/FitAmplitude.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/NamedParameter.h"

#include <iostream>
#include <iomanip>
#include <fstream>

#include <complex>
#include <algorithm>

//#include <boost/filesystem>
#include <sys/types.h>
#include <sys/stat.h>

//#include "boost/filesystem.hpp"

using namespace std;
using namespace MINT;

FitAmpPair::FitAmpPair()
  : _fitA1(0)
  , _fitA2(0)
  , _slow(true)
  , _beingIntegrated(true)
  , _eventDependentParameters(this)
  , _sum(0)
  , _sumName("FitAmpPair._sum")
  , _sumsq(0)
  , _sumSqName("FitAmpPair._sumSq")
  , _Nevents(0)
  , _NName("FitAmpPair._N")
  , _weightSum(0)
  , _weightSumName("FitAmpPair._weightSum")
  , _hsRe()
  , _hsIm()
  , _name("")
  , _dirName("")
  , _lastEntry(0)
{

}

FitAmpPair::FitAmpPair(FitAmplitude& a1, FitAmplitude& a2)
  : _fitA1(&a1)
  , _fitA2(&a2)
  , _slow(true)
  , _beingIntegrated(true)
  , _eventDependentParameters(this)
  , _sum(0)
  , _sumName("FitAmpPair._sum")
  , _sumsq(0)
  , _sumSqName("FitAmpPair._sumSq")
  , _Nevents(0)
  , _NName("FitAmpPair._N")
  , _weightSum(0)
  , _weightSumName("FitAmpPair._weightSum")
  , _hsRe()
  , _hsIm()
  , _name("")
  , _dirName("")
  , _lastEntry(0)
{

  std::string name_1 = _fitA1->name();
  std::string name_2 = _fitA2->name();
  if(name_1 > name_2) std::swap(_fitA1, _fitA2);
  // this ensures that the amplitudes are always stored in 
  // a well-defined order. Determines sign of imagninary part.
  // Since we want Re(A1 A2*), this doesn't matter, however
  // we have to be consistent when we save and retrive and then add to this.
  // Also ensures consistent file and directory naming etc.
  makeName();
  makeDirName();

  _eventDependentParameters.registerFitParDependence(rawAmp1());
  _eventDependentParameters.registerFitParDependence(rawAmp2());
  _eventDependentParameters.registerFitParDependence(fitAmp1().eventDependentPrefactors());
  _eventDependentParameters.registerFitParDependence(fitAmp2().eventDependentPrefactors());
}
FitAmpPair::FitAmpPair(const FitAmpPair& other)
  : _fitA1(other._fitA1)
  , _fitA2(other._fitA2)
  , _beingIntegrated(other._beingIntegrated)
  , _eventDependentParameters(other._eventDependentParameters)
  , _sum(other._sum)
  , _sumName(other._sumName)
  , _sumsq(other._sumsq)
  , _sumSqName(other._sumSqName)
  , _Nevents(other._Nevents)
  , _NName(other._NName)
  , _weightSum(other._weightSum)
  , _weightSumName(other._weightSumName)
  , _hsRe(other._hsRe)
  , _hsIm(other._hsIm)
  , _name(other._name)
  , _dirName(other._dirName)
  , _lastEntry(other._lastEntry)
{
}

bool FitAmpPair::isCompatibleWith(const FitAmpPair& other) const{
  return (fitAmp1().name() == other.fitAmp1().name() && 
	  fitAmp2().name() == other.fitAmp2().name());
}
bool FitAmpPair::add(const FitAmpPair& other){
  if(! isCompatibleWith(other)){
    cout << "ERROR in FitAmpPair::add "
	 << "trying to add two incompatible FitAmpPairs: "
	 << "\n " << this->name() << " + " << other.name() 
	 << endl;
    return false;
  }
  
  _sum        += other._sum;
  _sumsq      += other._sumsq;
  _Nevents    += other._Nevents;
  _weightSum  += other._weightSum;

  _hsRe       += other._hsRe;
  _hsIm       += other._hsIm;

  return true;
}
void FitAmpPair::addToHistograms(IDalitzEvent* evtPtr
				 , const std::complex<double>& c){
  _hsRe.addEvent(*evtPtr, c.real());
  _hsIm.addEvent(*evtPtr, c.imag());
}

const std::string& FitAmpPair::makeName(){
  _name = "A(" + fitAmp1().name() + ")_x_fitA(" + fitAmp2().name() + ")*";
  return _name;
}

const std::string& FitAmpPair::makeDirName(){
  _dirName="";
  if("" == _name) makeName();

  std::string::const_iterator preEnd = _name.end();
  preEnd--;
  
  for(std::string::const_iterator it = _name.begin();
      it != _name.end(); it++){
    if(it != preEnd){
      std::string::const_iterator next = it; next++;
      if('-' == *it && '>' == *next){
	_dirName += "_to_";
	it++;
	continue;
      }
    }
    if('*' == *it) _dirName += "star";
    else if(' ' == *it) _dirName += "_";
    //    else if('(' == *it) _dirName +="_bra_";
    //    else if(')'==*it) _dirName +="_ket_";
    else if('['==*it) _dirName += "_sqbra_";
    else if(']'==*it) _dirName += "_sqket_";
    //    else if(','==*it) _dirName += "_comma_";
    //    else if('-' == *it) _dirName += "min";
    //    else if('+' == *it) _dirName += "pls";
    else _dirName += *it;
  }
  return _dirName;
}

bool FitAmpPair::makeDirectory(const std::string& asSubdirOf)const{
  /*
    A mode is created from or'd permission bit masks defined
     in <sys/stat.h>:
           #define S_IRWXU 0000700     RWX mask for owner 
           #define S_IRUSR 0000400     R for owner 
           #define S_IWUSR 0000200     W for owner 
           #define S_IXUSR 0000100     X for owner 

           #define S_IRWXG 0000070     RWX mask for group 
           #define S_IRGRP 0000040     R for group 
           #define S_IWGRP 0000020     W for group 
           #define S_IXGRP 0000010     X for group 

           #define S_IRWXO 0000007     RWX mask for other 
           #define S_IROTH 0000004     R for other 
           #define S_IWOTH 0000002     W for other 
           #define S_IXOTH 0000001     X for other 

           #define S_ISUID 0004000     set user id on execution 
           #define S_ISGID 0002000     set group id on execution 
           #define S_ISVTX 0001000     save swapped text even after use
   */

  mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO 
              | S_ISUID | S_ISGID; 
  // see above for meaning. I want everybody to be allowed to read/write/exec.
  // Not sure about the last two bits.

  int zeroForSuccess = 0;
  zeroForSuccess |= mkdir( (asSubdirOf ).c_str(), mode );
  zeroForSuccess |= mkdir( (asSubdirOf + "/" + dirName() ).c_str(), mode );
  return (0 == zeroForSuccess);
}

bool FitAmpPair::save(const std::string& asSubdirOf) const{
  bool success=true;
  makeDirectory(asSubdirOf); // ignore error codes from mkdir, they could
			  // be because directory already exists - if
			  // not, we'll pick it up when we try to save
			  // the files.

  success &= saveValues(asSubdirOf);
  success &= saveHistos(asSubdirOf);

  return success;
}
bool FitAmpPair::retrieve(const std::string& asSubdirOf){
  bool dbThis=false;
  bool success=true;

  success &= retrieveValues(asSubdirOf);
  success &= retrieveHistos(asSubdirOf);

  if(dbThis){
    cout << "after FitAmpPair::retrieve: pat = "
	 << histosRe().begin()->second.pattern() << ", "
	 << histosIm().begin()->second.pattern() << endl;
  }
  return success;
}


std::string FitAmpPair::valueFileName(const std::string& asSubdirOf)const{
  return asSubdirOf + "/" + dirName() + "/value.txt";
}
std::string FitAmpPair::histoReFileName(const std::string& asSubdirOf)const{
  return asSubdirOf + "/" + dirName() + "/histoRe";
}
std::string FitAmpPair::histoImFileName(const std::string& asSubdirOf)const{
  return asSubdirOf + "/" + dirName() + "/histoIm";
}

bool FitAmpPair::saveHistos(const std::string& asSubdirOf) const{
  bool sc = true;
  sc |= histosRe().saveAsDir(histoReFileName(asSubdirOf));
  sc |= histosIm().saveAsDir(histoImFileName(asSubdirOf));
  return sc;
}
bool FitAmpPair::retrieveHistos(const std::string& asSubdirOf){
  bool sc = true;
  sc |= histosRe().retrieveFromDir(histoReFileName(asSubdirOf));
  sc |= histosIm().retrieveFromDir(histoImFileName(asSubdirOf));
  return sc;
}

bool FitAmpPair::saveValues(const std::string& asSubdirOf) const{

  NamedParameter<double> n_sumRe(_sumName + ".real()"
				 , NamedParameterBase::QUIET);
  NamedParameter<double> n_sumIm(_sumName + ".imag()"
				 ,NamedParameterBase::QUIET);
  NamedParameter<double> n_sumSqRe(_sumSqName + ".real()"
				   , NamedParameterBase::QUIET);
  NamedParameter<double> n_sumSqIm(_sumSqName + ".imag()"
				   , NamedParameterBase::QUIET);
  NamedParameter<long int> n_Nevents(_NName
				     , NamedParameterBase::QUIET);
  NamedParameter<double> n_weightSum(_weightSumName
				     , NamedParameterBase::QUIET);

  n_sumRe     = _sum.real();
  n_sumIm     = _sum.imag();
  n_sumSqRe   = _sumsq.real();
  n_sumSqIm   = _sumsq.imag();
  n_Nevents   = _Nevents;
  n_weightSum = _weightSum;

  std::string fname = valueFileName(asSubdirOf);
  ofstream os(fname.c_str());
  if(os.bad()){
    cout << "ERROR in FitAmpPair::saveValues of \n\t" << name()
	 << "\n\t unable to create file: "
	 << "\n\t" << fname << endl;
    return false;
  }
  os << setprecision(20)
     << name() 
     << '\n' << n_sumRe 
     << '\n' << n_sumIm
     << '\n' << n_sumSqRe 
     << '\n' << n_sumSqIm
     << '\n' << n_Nevents
     << '\n' << n_weightSum
     << endl;
  os.close();
  return true;
}
bool FitAmpPair::retrieveValues(const std::string& fromDirectory){
  bool dbThis=false;
  std::string fname = valueFileName(fromDirectory);
  if(dbThis)cout << "trying to retreive values from: " << fname << endl;
  if(dbThis)cout << "current sum " << _sum << endl;

  NamedParameter<double> n_sumRe(_sumName + ".real()", fname.c_str()
				 );
  n_sumRe.reloadFile(fname.c_str());
  NamedParameter<double> n_sumIm(_sumName + ".imag()", fname.c_str()
				 );//,NamedParameterBase::QUIET);
  NamedParameter<double> n_sumSqRe(_sumSqName + ".real()", fname.c_str()
				   );//, NamedParameterBase::QUIET);
  NamedParameter<double> n_sumSqIm(_sumSqName + ".imag()", fname.c_str()
				   );//, NamedParameterBase::QUIET);
  NamedParameter<long int> n_Nevents(_NName, fname.c_str()
				     );// , NamedParameterBase::QUIET);
  NamedParameter<double> n_weightSum(_weightSumName, fname.c_str()
				     );//, NamedParameterBase::QUIET);

  complex<double> n_sum(n_sumRe, n_sumIm);

  if(dbThis) cout << "n_sumRe " << n_sumRe << "n_sum " << n_sum << endl;
  _sum = n_sum;
  if(dbThis) cout << "_sum = " << _sum << endl;
  complex<double> n_sumsq(n_sumSqRe, n_sumSqIm);
  _sumsq = n_sumsq;
  _Nevents = n_Nevents;
  _weightSum = n_weightSum;

  return true;
}

//bool FitAmpPair::saveHistograms(const std::string& asSubdirOf) const;
//}


double FitAmpPair::add(IDalitzEvent* evtPtr
		       , double weight
		       , double efficiency
		       ){
  if(0 == evtPtr) return 0;
  return add(*evtPtr, weight, efficiency);
}
double FitAmpPair::add(IDalitzEvent& evt
		       , double weight
		       , double efficiency
		       ){
  bool dbThis=false;
  
  _Nevents++;
  
  double ps = evt.phaseSpace();
  if(ps <= 0.0000){
    if(!(_Nevents%100000)){
      cout << "WARNING in FitAmpPair::addToHistograms"
	   << " event with phase space = " << ps << endl;
    }
    return 0; // should not happen.
  }

  double w = evt.getWeight()
    /evt.getGeneratorPdfRelativeToPhaseSpace();
  w *= weight;
  
  _weightSum += w;// / ps;
  
  if(dbThis){
    cout << " FitAmpPair::add, for pair "
	 << fitAmp1().name() << " / " << fitAmp2().name()
	 << endl;
  }

  complex<double> c=ampValue(evt) * efficiency * w;
  _lastEntry = c;
  _sum   += c;

  if(slow() && 0.0 != c) this->addToHistograms(&evt, c);

  if(dbThis){
    cout << "\t c = " << c
	 << " _sum " << _sum
	 << endl;
  }
  complex<double> csq(c.real()*c.real(), c.imag()*c.imag());
  _sumsq += csq;

  return (c * fitParValue()).real();
}

double FitAmpPair::reAdd(IDalitzEvent& evt
		       , double weight
		       , double efficiency
		       ){
  bool dbThis=false;
  

  _Nevents++;
  
  double ps = evt.phaseSpace();
  if(ps <= 0.0000){
    if(!(_Nevents%100000)){
      cout << "WARNING in FitAmpPair::addToHistograms"
	   << " event with phase space = " << ps << endl;
    }
    return 0; // should not happen.
  }

  double w = evt.getWeight()
    /evt.getGeneratorPdfRelativeToPhaseSpace();
  w *= weight;
  
  _weightSum += w;// / ps;
  
  if(dbThis){
    cout << " FitAmpPair::add, for pair "
	 << fitAmp1().name() << " / " << fitAmp2().name()
	 << endl;
  }

  complex<double> c=ampValue(evt) * efficiency * w;
  _lastEntry = c;
  _sum   += c;

  if(slow() && 0.0 != c) this->addToHistograms(&evt, c);

  if(dbThis){
    cout << "\t c = " << c
	 << " _sum " << _sum
	 << endl;
  }
  complex<double> csq(c.real()*c.real(), c.imag()*c.imag());
  _sumsq += csq;

  return (c * fitParValue()).real();
}


double FitAmpPair::add(counted_ptr<IDalitzEvent> evtPtr
		       , double weight
		       , double efficiency
		       ){
  return add(evtPtr.get(), weight, efficiency);
}
complex<double> FitAmpPair::lastEntry() const{
  return _lastEntry;
}

std::complex<double> FitAmpPair::valNoFitPars() const{
  bool dbThis=false;
  if(dbThis){
    cout << " FitAmpPair::sumWithoutFitPars for pair "
	 << fitAmp1().name() << " / " << fitAmp2().name()
	 << endl;
  }
  double dN = (double) _Nevents;
  std::complex<double> total = ((std::complex<double>)oneOrTwo()) * _sum;
  std::complex<double> returnVal;

  if(_weightSum > 0){
    returnVal = total/_weightSum;
  }else{
    returnVal = total/dN;
  }

  if(dbThis){
    cout << "\t returning " << returnVal << endl;
    cout << "\t   = Real( " << oneOrTwo()
	 << " * " << _sum
	 << " / " << _Nevents
	 << " )"
	 << endl;
  }  
  return returnVal;
}
double FitAmpPair::integral() const{

  return complexVal().real();
}
double FitAmpPair::weightSum() const{
  if(_weightSum > 0) return _weightSum;
  else return (double) _Nevents;
}

long int FitAmpPair::N() const{
  return _Nevents;
}

void FitAmpPair::reset(){
  _beingIntegrated = true;
  _sum = 0;
  _sumsq = 0;
  _Nevents = 0;
  _weightSum = 0;
  if(slow()) _hsRe.clearAllHistos();
  if(slow()) _hsIm.clearAllHistos();
  _lastEntry = 0;
}

bool FitAmpPair::needToReIntegrate() const{
  return _eventDependentParameters.changedSinceLastCall();
}
void FitAmpPair::rememberEventDependentParameters(){
  _eventDependentParameters.rememberFitParValues();
}

void FitAmpPair::startReIntegration(){
  reset();
  _beingIntegrated=true;
}
void FitAmpPair::startIntegration(){
  _beingIntegrated=true;
}

void FitAmpPair::endIntegration(){
  _beingIntegrated=false;
  rememberEventDependentParameters();
}

bool FitAmpPair::acceptEvents() const{
  return _beingIntegrated;
}
/* now inline:
DalitzHistoSet FitAmpPair::histoSet() const{
  return histoSetRe();
}
*/

DalitzHistoSet FitAmpPair::histoSetRe() const{
  bool dbThis=false;
  if(dbThis){
    cout << " FitAmpPair::histograms, for pair "
	 << fitAmp1().name() << " / " << fitAmp2().name()
	 << endl;
    cout << " r " << fitParValue().real() << ", " 
	 << histosRe().begin()->second.histo()->Integral()
	 << " i " << fitParValue().imag() << ", " 
	 << histosIm().begin()->second.histo()->Integral()
	 << endl;
  }

  
  return ( // this calculates a DalitzHistoSet:
	  oneOrTwo() * 
	  (fitParValue().real() * histosRe() 
	   - fitParValue().imag() * histosIm())
	   )/_weightSum;// / ((double)_Nevents);

  //  total /= (double)_Nevents;
}
DalitzHistoSet FitAmpPair::histoSetIm() const{
  // needed by Lauren's code, only
  bool dbThis=false;
  if(dbThis){
    cout << " FitAmpPair::histograms, for pair "
	 << fitAmp1().name() << " / " << fitAmp2().name()
	 << endl;
    cout << " r " << fitParValue().real() << ", " 
	 << histosRe().begin()->second.histo()->Integral()
	 << " i " << fitParValue().imag() << ", " 
	 << histosIm().begin()->second.histo()->Integral()
	 << endl;
  }
  
  return ( // this calculates a DalitzHistoSet:
	  oneOrTwo() * 
	  (fitParValue().real() * histosIm() 
	   + fitParValue().imag() * histosRe())
	   )/_weightSum;// / ((double)_Nevents);

  //  total /= (double)_Nevents;
}

double FitAmpPair::variance() const{
  if(_Nevents <=0) return 0;
  double dN = (double) _Nevents;
  complex<double> mean   = _sum/dN;
  complex<double> meansq = _sumsq/dN;
  
  complex<double> var(meansq.real() - mean.real()*mean.real()
		      , meansq.imag() - mean.imag()*mean.imag());

  var /= dN;

  if(_weightSum > 0) var *= dN*dN/(_weightSum*_weightSum);

  return oneOrTwo() * (fitParValue() * var).real();
}

bool FitAmpPair::isSingleAmp() const{
  return _fitA1 == _fitA2;
}

const std::string& FitAmpPair::name(){
  if("" == _name) makeName();
  return _name;
  //  if(isSingleAmp()) return "| " + _fitA1->name() + " |^2";
  //  else return "(" + _fitA1->name() +") * (" + _fitA2->name() + ")*";
}
const std::string& FitAmpPair::name() const{
  return _name;
}

const std::string& FitAmpPair::dirName(){
  if("" == _dirName) makeDirName();
  return _dirName;
}
const std::string& FitAmpPair::dirName() const{
  return _dirName;
}

/*
// the following routines are needed to 
// calculate the variance
double FitAmpPair::ReSquared() const{
  return pow(oneOrTwo() * fitParValue().real(), 2)*_sumsq.real();
}
double FitAmpPair::ImSquared() const{
  return pow(oneOrTwo() * fitParValue().imag(), 2)*_sumsq.imag();
}
double FitAmpPair::ImRe() const{
  return oneOrTwo() * oneOrTwo() 
    * fitParValue().real()*_sum.real() 
    * fitParValue().imag()*_sum.imag();

}
*/

void FitAmpPair::print(std::ostream& os) const{
  os << "FitAmpPair " << name()
     << ": N=" << N() << ", integral=" << integral();
}
bool lessByFitAmpPairIntegral::operator()(const FitAmpPair& a
					  , const FitAmpPair& b) const{
  return a.integral() < b.integral();
}

bool lessByFitAmpPairIntegral_ptr::operator()(const FitAmpPair* a, const FitAmpPair* b) const{

  if(0 == a && 0==b) return false;
  if(0 == a && 0!=b) return true;
  if(0 !=a  && 0==b) return false;
  return a->integral() < b->integral();
}
bool lessByFitAmpPairIntegral_ptr_int_pair::operator()(const std::pair<FitAmpPair*, int>& apair
						       , const std::pair<FitAmpPair*, int>& bpair) const{
  const FitAmpPair* a = apair.first;
  const FitAmpPair* b = bpair.first;

  if(0 == a && 0==b) return false;
  if(0 == a && 0!=b) return true;
  if(0 !=a  && 0==b) return false;
  return a->integral() < b->integral();
}


FitAmpPair& FitAmpPair::operator+=(const FitAmpPair& other){
  this->add(other);
  return *this;
}
FitAmpPair FitAmpPair::operator+(const FitAmpPair& other) const{
  FitAmpPair returnVal(*this);
  returnVal += other;
  return returnVal;
}

std::ostream& operator<<(std::ostream& os, const FitAmpPair& fap){
  fap.print(os);
  return os;
}
//
