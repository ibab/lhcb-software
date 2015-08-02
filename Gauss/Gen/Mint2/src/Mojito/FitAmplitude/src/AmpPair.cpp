// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT
#include "Mint/AmpPair.h"
#include "Mint/Amplitude.h"
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

AmpPair::AmpPair(Amplitude* a1, Amplitude* a2)
  : _a1(a1)
  , _a2(a2)
  , _sum(0)
  , _sumName("AmpPair._sum")
  , _sumsq(0)
  , _sumSqName("AmpPair._sumSq")
  , _Nevents(0)
  , _NName("AmpPair._N")
  , _weightSum(0)
  , _weightSumName("AmpPair._weightSum")
  , _hsRe()
  , _hsIm()
  , _name("")
  , _dirName("")
  , _lastEntry(0)
{
  if(0 == _a1 || 0 == _a2){
    cout << "ERROR in AmpPair::AmpPair !"
	 << "\n\t One of the amplitude pointers is zero:"
	 << "\n\t _a1 = " << _a1 << ", _a2 = " << _a2
	 << "\n\t ... will crash now" << endl;
    throw "zero amp pointer in AmpPair constructor";
  }

  std::string name_1 = _a1->name();
  std::string name_2 = _a2->name();
  if(name_1 > name_2) std::swap(_a1, _a2);
  // this ensures that the amplitudes are always stored in 
  // a well-defined order. Determines sign of imagninary part.
  // Since we want Re(A1 A2*), this doesn't matter, however
  // we have to be consistent when we save and retrive and then add to this.
  // Also ensures consistent file and directory naming etc.
  makeName();
  makeDirName();

  registerFitParDependence(*_a1);
  if(_a1 != _a2)  registerFitParDependence(*_a2);
}
AmpPair::AmpPair(const AmpPair& other)
  : _a1(other._a1)
  , _a2(other._a2)
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

  removeAllFitParDependencies();
  registerFitParDependence(*_a1);
  if(_a1 != _a2)  registerFitParDependence(*_a2);
}

bool AmpPair::isCompatibleWith(const AmpPair& other) const{
  return (amp1()->name() == other.amp1()->name() && 
	  amp2()->name() == other.amp2()->name());
}
bool AmpPair::add(const AmpPair& other){
  if(! isCompatibleWith(other)){
    cout << "ERROR in AmpPair::add "
	 << "trying to add two incompatible AmpPairs: "
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
void AmpPair::addToHistograms(IDalitzEvent* evtPtr
				 , const std::complex<double>& c){
  _hsRe.addEvent(*evtPtr, c.real());
  _hsIm.addEvent(*evtPtr, c.imag());
}

const std::string& AmpPair::makeName(){
  if(0 == amp1() || 0 == amp2()){
    cout << "ERROR in AmpPair::makeName():"
	 << " At least one of the amplitude pointers is 0:"
	 << " _a1 = " << _a1 << ", _a2 = " << _a2 << endl;
    cout << "I'll crash. " << endl;
    throw "zero amplitude pointers";
  }
  _name = "A(" + amp1()->name() + ")_x_A(" + amp2()->name() + ")*";
  return _name;
}

const std::string& AmpPair::makeDirName(){
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

bool AmpPair::makeDirectory(const std::string& asSubdirOf)const{
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

bool AmpPair::save(const std::string& asSubdirOf) const{
  bool success=true;
  makeDirectory(asSubdirOf); // ignore error codes from mkdir, they could
			  // be because directory already exists - if
			  // not, we'll pick it up when we try to save
			  // the files.

  success &= saveValues(asSubdirOf);
  success &= saveHistos(asSubdirOf);

  return success;
}
bool AmpPair::retrieve(const std::string& asSubdirOf){
  bool dbThis=false;
  bool success=true;

  success &= retrieveValues(asSubdirOf);
  success &= retrieveHistos(asSubdirOf);

  if(dbThis){
    cout << "after AmpPair::retrieve: pat = "
	 << histosRe().begin()->second.pattern() << ", "
	 << histosIm().begin()->second.pattern() << endl;
  }
  return success;
}


std::string AmpPair::valueFileName(const std::string& asSubdirOf)const{
  return asSubdirOf + "/" + dirName() + "/value.txt";
}
std::string AmpPair::histoReFileName(const std::string& asSubdirOf)const{
  return asSubdirOf + "/" + dirName() + "/histoRe";
}
std::string AmpPair::histoImFileName(const std::string& asSubdirOf)const{
  return asSubdirOf + "/" + dirName() + "/histoIm";
}

bool AmpPair::saveHistos(const std::string& asSubdirOf) const{
  bool sc = true;
  sc |= histosRe().saveAsDir(histoReFileName(asSubdirOf));
  sc |= histosIm().saveAsDir(histoImFileName(asSubdirOf));
  return sc;
}
bool AmpPair::retrieveHistos(const std::string& asSubdirOf){
  bool sc = true;
  sc |= histosRe().retrieveFromDir(histoReFileName(asSubdirOf));
  sc |= histosIm().retrieveFromDir(histoImFileName(asSubdirOf));
  return sc;
}

bool AmpPair::saveValues(const std::string& asSubdirOf) const{

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
    cout << "ERROR in AmpPair::saveValues of \n\t" << name()
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
bool AmpPair::retrieveValues(const std::string& fromDirectory){
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

//bool AmpPair::saveHistograms(const std::string& asSubdirOf) const;
//}

std::complex<double> AmpPair::add(IDalitzEvent* evtPtr
		       , double weight
		       , double efficiency
		       ){
  bool dbThis=false;
  
  _Nevents++;
  if(0 == evtPtr) return 0;
  
  double ps = evtPtr->phaseSpace();
  if(ps <= 0.0000){
	  if(!(_Nevents%100000)){
    cout << "WARNING in AmpPair::addToHistograms"
	 << " event with phase space = " << ps << endl;
	  }
    return 0; // should not happen.
  }

  double w = evtPtr->getWeight()
    /evtPtr->getGeneratorPdfRelativeToPhaseSpace();
  w *= weight;
  
  _weightSum += w;// / ps;
  
  if(dbThis){
    cout << " AmpPair::add, for pair "
	 << _a1->name() << " / " << _a2->name()
	 << endl;
  }

  complex<double> c=ampValue(evtPtr) * efficiency * w;
  _lastEntry = c;
  _sum   += c;
  this->addToHistograms(evtPtr, c);

  if(dbThis){
    cout << "\t c = " << c
	 << " _sum " << _sum
	 << endl;
  }
  complex<double> csq(c.real()*c.real(), c.imag()*c.imag());
  _sumsq += csq;

  return c;
}

std::complex<double> AmpPair::add(counted_ptr<IDalitzEvent> evtPtr
		       , double weight
		       , double efficiency
		       ){
  return add(evtPtr.get(), weight, efficiency);
}
complex<double> AmpPair::lastEntry() const{
  return _lastEntry;
}
complex<double> AmpPair::ampValue(IDalitzEvent* evtPtr){
  if(0 == _a1 || 0 == _a2 || 0 == evtPtr) return 0;


  complex<double> c1 = _a1->getVal(*evtPtr);
  complex<double> c2 = _a2->getVal(*evtPtr);
  complex<double> c2star = conj(c2);
  
  complex<double> val = (c1 * c2star);

  return val;
}

int AmpPair::oneOrTwo()const{
  if(isSingleAmp()) return 1;
  else return 2;
}

std::complex<double> AmpPair::integral() const{
  bool dbThis=false;
  if(dbThis){
    cout << " AmpPair::sumWithoutFitPars for pair "
	 << _a1->name() << " / " << _a2->name()
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

double AmpPair::weightSum() const{
  if(_weightSum > 0) return _weightSum;
  else return (double) _Nevents;
}

long int AmpPair::N() const{
  return _Nevents;
}

std::complex<double> AmpPair::variance() const{
  if(_Nevents <=0) return 0;
  double dN = (double) _Nevents;
  complex<double> mean   = _sum/dN;
  complex<double> meansq = _sumsq/dN;
  
  complex<double> var(meansq.real() - mean.real()*mean.real()
		      , meansq.imag() - mean.imag()*mean.imag());

  var /= dN;

  if(_weightSum > 0) var *= dN*dN/(_weightSum*_weightSum);

  return ((double) oneOrTwo()) * var;
}

bool AmpPair::isSingleAmp() const{
  return _a1 == _a2;
}

const std::string& AmpPair::name(){
  if("" == _name) makeName();
  return _name;
  //  if(isSingleAmp()) return "| " + _a1->name() + " |^2";
  //  else return "(" + _a1->name() +") * (" + _a2->name() + ")*";
}
const std::string& AmpPair::name() const{
  return _name;
}

const std::string& AmpPair::dirName(){
  if("" == _dirName) makeDirName();
  return _dirName;
}
const std::string& AmpPair::dirName() const{
  return _dirName;
}

/*
// the following routines are needed to 
// calculate the variance
double AmpPair::ReSquared() const{
  return pow(oneOrTwo() * fitParValue().real(), 2)*_sumsq.real();
}
double AmpPair::ImSquared() const{
  return pow(oneOrTwo() * fitParValue().imag(), 2)*_sumsq.imag();
}
double AmpPair::ImRe() const{
  return oneOrTwo() * oneOrTwo() 
    * fitParValue().real()*_sum.real() 
    * fitParValue().imag()*_sum.imag();

}
*/

void AmpPair::print(std::ostream& os) const{
  os << "AmpPair " << name()
     << ": N=" << N() << ", integral=" << integral();
}
bool lessByAmpPairIntegral::operator()(const AmpPair& a
					  , const AmpPair& b) const{
  return abs(a.integral()) < abs(b.integral());
}

bool lessByAmpPairIntegral_ptr::operator()(const AmpPair* a, const AmpPair* b) const{

  if(0 == a && 0==b) return false;
  if(0 == a && 0!=b) return true;
  if(0 !=a  && 0==b) return false;
  return abs(a->integral()) < abs(b->integral());
}
bool lessByAmpPairIntegral_ptr_int_pair::operator()(const std::pair<AmpPair*, int>& apair
						       , const std::pair<AmpPair*, int>& bpair) const{
  const AmpPair* a = apair.first;
  const AmpPair* b = bpair.first;

  if(0 == a && 0==b) return false;
  if(0 == a && 0!=b) return true;
  if(0 !=a  && 0==b) return false;
  return abs(a->integral()) < abs(b->integral());
}


AmpPair& AmpPair::operator+=(const AmpPair& other){
  this->add(other);
  return *this;
}
AmpPair AmpPair::operator+(const AmpPair& other) const{
  AmpPair returnVal(*this);
  returnVal += other;
  return returnVal;
}

std::ostream& operator<<(std::ostream& os, const AmpPair& fap){
  fap.print(os);
  return os;
}
//
