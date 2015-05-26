// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT
#include "Mint/FitParameter.h"
#include "Mint/MinuitParameterSet.h"

#include <iostream>
using namespace std;
using namespace MINT;

const char* FitParameter::_initString = " 1\t0.0\t0.0\t0.0\t0.0";
const char* FitParameter::getInitString(){ return _initString;}

FitParameter::FitParameter(const std::string& name
			   , const char* fname
			   , MinuitParameterSet* setPtr
			   , FIX_OR_WHAT fow
			   , NamedParameterBase::VERBOSITY vb
			   )
  : INamedParameter()
  , NamedParameterBase(name, fname, vb)
  , IMinuitParameter()
  //  , _minPtr(0)
  , _pset(0)
  , _psetIndex(-9999)
  //  , _pN(-1)
  , _iFixInit((int)fow)
  , _meanInit(0)
  , _stepInit(0)
  , _minInit(0)
  , _maxInit(0)
  , _scanParameters(name + "_Scan", fname, NamedParameterBase::QUIET)
{
  defaultInit();
  setFromParsedFile();
  MinuitParameterSet* ps = setPtr;
  if(0 == ps) ps = MinuitParameterSet::getDefaultSet();
  addToParSet(ps);
  initToResult();
  //  cout << "FitParameter constructed " << this->name() << endl;
}
FitParameter::FitParameter(const std::string& name
			   , int fix
			   , double mean
			   , double step
			   , double mi
			   , double ma
			   , MinuitParameterSet* setPtr
			   , NamedParameterBase::VERBOSITY vb
			   , const char* fname
			   )
  : INamedParameter()
  , NamedParameterBase(name, fname, vb)
  , IMinuitParameter()
  //  , _minPtr(0)
  , _pset(0)
  , _psetIndex(-9999)
  //  , _pN(-1)
  , _iFixInit(fix)
  , _meanInit(mean)
  , _stepInit(step)
  , _minInit(mi)
  , _maxInit(ma)
  , _scanParameters(name + "_Scan", 0, NamedParameterBase::QUIET)
{
  _gotInitialised = true;
  setFromParsedFile();// so the above values are for initialisation but over-ruled by what's in the file
  MinuitParameterSet* ps = setPtr;
  if(0 == ps) ps = MinuitParameterSet::getDefaultSet();
  
  addToParSet(ps);

  initToResult();
  //cout << "FitParameter constructed a " << this->name() << endl;
}

FitParameter::FitParameter(const std::string& name
			   , MinuitParameterSet* setPtr
			   , FIX_OR_WHAT fow
			   , NamedParameterBase::VERBOSITY vb
			   )
  : INamedParameter()
  , NamedParameterBase(name, 0, vb)
  , IMinuitParameter()
  //  , _minPtr(0)
  , _pset(0)
  , _psetIndex(-9999)
  //  , _pN(-1)
  , _iFixInit((int) fow)
  , _meanInit(0)
  , _stepInit(0)
  , _minInit(0)
  , _maxInit(0)
  , _scanParameters(name + "_Scan", 0, NamedParameterBase::QUIET)
{
  defaultInit();
  setFromParsedFile();
  MinuitParameterSet* ps = setPtr;
  if(0 == ps) ps = MinuitParameterSet::getDefaultSet();
  addToParSet(ps);
  initToResult();
  //cout << "FitParameter constructed b " << this->name() << endl;
}

// not so clear if I should keep copy constructor...
// made it protected for now.
FitParameter::FitParameter(const FitParameter& other) 
  : INamedParameter()
  , NamedParameterBase(other)
  , IMinuitParameter()
  //  , _minPtr(other._minPtr)
  , _pset(other._pset)
  , _psetIndex(other._psetIndex)
  //  , _pN(other._pN)
  , _iFixInit(other._iFixInit)
  , _meanInit(other._meanInit)
  , _stepInit(other._stepInit)
  , _minInit(other._minInit)
  , _maxInit(other._maxInit)
  , _scanParameters(other._scanParameters)
  , _meanResult(other._meanResult)
  , _errResult(other._errResult)
  , _errPosResult(other._errPosResult)
  , _errNegResult(other._errNegResult)
{
  //cout << "FitParameter copy-constructed " << this->name() << endl;
}

void FitParameter::setCurrentValToInit(){
  _meanResult   = _currentFitVal = _meanInit;
}

void FitParameter::resetToInit(){
  _meanResult   = _currentFitVal = _meanInit;
  _errResult    = _stepInit;
  _errPosResult = -9999;
  _errNegResult = -9999;
}
void FitParameter::initToResult(){
  // I once gave it this counter-intuitive name
  // (the picture was: init->Result) keep it for compatibility.
  resetToInit();
}
FitParameter::~FitParameter(){
  //cout << "FitParameter destructor destroying " << this->name() << endl;
  if(0 != _pset) _pset->unregister(this);
}

bool FitParameter::addToParSet(MinuitParameterSet* ps){
  // now the same as setParSet.
  if(0 == ps) return false;
  setParSet(ps);
  return true;
}
bool FitParameter::setParSet(MinuitParameterSet* ps){
  //  if(0 == ps) return false;
  if(ps == _pset) return true;
  if(0 != _pset) _pset->unregister(this);
  _pset = ps;
  _psetIndex = -9999;
  if(0 != ps) ps->add(this);
  return true;
}
bool FitParameter::setParSetIndex(int psetIndex){
  _psetIndex = psetIndex;
  return true;
}
int FitParameter::parSetIndex() const{
  return _psetIndex;
}

bool FitParameter::setFromParsedLine(const ParsedParameterLine& line){
  const std::vector<std::string>& vsl = line.parsedStrings();
  if(vsl.size() < 3){
    if(! _quiet) cout << "ERROR in FitParameter::setFromParsedLine"
		      << " need at least 3 parameters:\n"
		      << " iFix, initial mean"
		      << endl;
    return false;
  }
  if(vsl[0] != _name){
    if(! _quiet) std::cout << "ERROR in FitParameter::setFromParsedLine"
			   << " wrong parameter name. My name is " << _name
			   << " the line's first element is " << vsl[0]
			   << endl;
    return false;
  }
  unsigned int start=0;
  //  if(vsl.size() >=7) ++start; // 1st parameter is index number
  // above was for compatibility with Mikhail's code, not needed anymore

  if(vsl.size() > ++start) stringToAnything(vsl[start], _iFixInit);
  if(vsl.size() > ++start) stringToAnything(vsl[start], _meanInit);
  if(vsl.size() > ++start) stringToAnything(vsl[start], _stepInit);
  if(vsl.size() > ++start) stringToAnything(vsl[start], _minInit);
  if(vsl.size() > ++start) stringToAnything(vsl[start], _maxInit);
  initToResult();

  return true;
}

/*
const TMinuit* FitParameter::getMinuit() const{
  return _minPtr;
}
TMinuit* FitParameter::getMinuit(){
  return _minPtr;
}
*/

int FitParameter::iFixInit() const{ return _iFixInit;}
double FitParameter::meanInit() const{ return _meanInit;}
double FitParameter::stepInit() const{ return _stepInit;}
double FitParameter::minInit() const{ return _minInit;}
double FitParameter::maxInit() const{ return _maxInit;}

bool FitParameter::scan() const{
  return _scanParameters.gotInitialised();
}
double FitParameter::scanMin() const{
  if(_scanParameters.size() < 2) return 0.0;
  else return _scanParameters.getVal(0);
}
double FitParameter::scanMax() const{
  if(_scanParameters.size() < 2) return 0.0;
  else return _scanParameters.getVal(1);
}

/*
int FitParameter::parNumber() const{
  if(_pN < 0 || _pN > getMinuit()->fMaxpar){
    cout << "ERROR IN FitParameter "
	 << name()
	 << " index out of bounds" << endl;
    throw "index out of bounds in FitParameter";
  }
  return _pN;
}
*/

/*
void FitParameter::associate(TMinuit* tm, int parNum){
  _minPtr = tm;
  _pN = parNum;
}
*/

/*
bool FitParameter::MinuitOK() const{
  bool OK = (0!= getMinuit());
  if(!OK) return false;
  OK &= (_pN >= 0);
  if(!OK) return false;
  OK &= (_pN < getMinuit()->fMaxpar);
  if(!OK) return false;
  OK &= (1 != getMinuit()->fEmpty);
  return OK;
}
*/

double FitParameter::getCurrentFitVal() const{
  return _meanResult;
}

void FitParameter::setCurrentFitVal(double cfv){
  _meanResult = cfv;
}

void FitParameter::setResult(double fitMean
			     , double fitErr
			     , double fitErrPos
			     , double fitErrNeg){
  _meanResult   = fitMean;
  _errResult    = fitErr;
  _errPosResult = fitErrPos;
  _errNegResult = fitErrNeg;

}

double FitParameter::mean() const{
  return _meanResult;
}
double FitParameter::min() const{
  return minInit();
}
double FitParameter::max() const{
  return maxInit();
}
double FitParameter::errPos(){
  return _errPosResult;
}
double FitParameter::errNeg(){
  return _errNegResult;
}
double FitParameter::err() const{
  return _errResult;
}

void FitParameter::defaultInit(){
  // defaults to fixed - i.e. won't be varied in fit
  // unless explicitly intitialised.
  // Otherwise all set to zero.
  this->setFromInitStringNoName(getInitString());
}
void FitParameter::print(std::ostream& os) const{
  // ... in a format that can be read back in
  os << "\"" <<  name() << "\""
     << "\t" << iFixInit()
     << "\t" << mean()
     << "\t" << err()
     << "\t" << min()
     << "\t" << max();
}
void FitParameter::printVal(std::ostream& os) const{
  // ... in a format that can be read back in, w/o the name
  os << iFixInit()
     << "\t" << mean()
     << "\t" << err()
     << "\t" << min()
     << "\t" << max();
}
void FitParameter::printResultVsInput(std::ostream& os) const{

  double pull = -9999;
  if(err() != 0) pull = (mean() - meanInit() )/err();


  os << "\"" <<  name() << "\""
     << "\t" << iFixInit()
     << "\t" << mean() << " - " << meanInit() << " / " << err()
     << " = \t" << pull;
}
void FitParameter::printFormat(std::ostream& os, int namelength){ // static
  std::string nameStr = "** name";
  if(namelength > 0) nameStr.resize(namelength, ' ');
  os << nameStr;
  os << "\t" << "Fix?"
     << "\t" << "init"
     << "\t" << "step"
     << "\t" << "min"
     << "\t" << "max"
     << std::endl;
}
void FitParameter::printResultFormat(std::ostream& os, int namelength){ // static
  std::string nameStr = "** name";
  if(namelength > 0) nameStr.resize(namelength, ' ');
  os << nameStr;
  os << "\t" << "Fix?"
     << "\t" << "mean"
     << "\t" << "err"
     << "\t" << "min"
     << "\t" << "max"
     << std::endl;
}

void FitParameter::fix(){
  _iFixInit = 2;
}
void FitParameter::fixToInit(){
  initToResult();
  fix();
}
void FitParameter::fixAndHide(){
  _iFixInit = 1;
}
void FitParameter::fixToInitAndHide(){
  initToResult();
  fixAndHide();
}
void FitParameter::unFix(){
  _iFixInit = 0;
}

std::ostream& operator<<(std::ostream& os, const FitParameter& fp){
  fp.print(os);
  return os;
}

//
