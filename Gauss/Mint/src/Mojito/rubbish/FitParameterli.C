// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:12 GMT
#include "FitParameterli.h"
#include "MinuitParameterSet.h"

FitParameterli::FitParameterli(const std::string& name, MinuitParameterSet* setPtr)
  : NamedParameterBase(name)
    , IMinuitParameter()
    , _myMinuit(0)
    , _pN(-1)
    , _iFixInit(0)
    , _meanInit(0)
    , _stepInit(0)
    , _minInit(0)
    , _maxInit(0)
{
  setFromParsedFile();
  if(0 != setPtr) setPtr->add(this);
}

// not so clear if I should keep copy constructor...
// made it protected for now.
FitParameterli::FitParameterli(const FitParameterli& other) 
  : NamedParameterBase(other)
    , IMinuitParameter()
    , _myMinuit(other._myMinuit) // copy pointer
    , _pN(other._pN)
    , _iFixInit(other._iFixInit)
    , _meanInit(other._meanInit)
    , _stepInit(other._stepInit)
    , _minInit(other._minInit)
    , _maxInit(other._maxInit)
{

}

FitParameterli::~FitParameterli(){
  if(0 != _pset) _pset->unregister(this);
}

bool FitParameterli::setFromParsedLine(const ParsedParameterLine& line){
  const std::vector<std::string>& vsl = line.parsedStrings();
  if(vsl.size() < 4){
    cout << "ERROR in FitParameter::setFromParsedLine"
	 << " need at least 3 parameters:\n"
	 << " iFix, initial mean, initial stepsize"
	 << endl;
    return false;
  }
  if(vsl[0] != _name){
    std::cout << "ERROR in FitParameter::setFromParsedLine"
	      << " wrong parameter name. My name is " << _name
	      << " the line's first element is " << vsl[0]
	      << endl;
    return false;
  }
  unsigned int start=0;
  if(vsl.size() >=7) ++start; // 1st parameter is index number
  if(vsl.size() > ++start) stringToAnything(vsl[start], _iFixInit);
  if(vsl.size() > ++start) stringToAnything(vsl[start], _meanInit);
  if(vsl.size() > ++start) stringToAnything(vsl[start], _stepInit);
  if(vsl.size() > ++start) stringToAnything(vsl[start], _minInit);
  if(vsl.size() > ++start) stringToAnything(vsl[start], _maxInit);

  return true;
}


const TMinuit* FitParameterli::getMinuit() const{
  return _myMinuit;
}
TMinuit* FitParameterli::getMinuit(){
  return _myMinuit;
}
int FitParameterli::iFixInit() const{ return _iFixInit;}
double FitParameterli::meanInit() const{ return _meanInit;}
double FitParameterli::stepInit() const{ return _stepInit;}
double FitParameterli::minInit() const{ return _minInit;}
double FitParameterli::maxInit() const{ return _maxInit;}

int FitParameterli::parNumber() const{
  if(_pN < 0 || _pN > getMinuit()->fMaxpar){
    cout << "ERROR IN FitParameterli "
	 << name()
	 << " index out of bounds" << endl;
    throw "index out of bounds in FitParameterli";
  }
  return _pN;
}

void FitParameterli::associate(TMinuit* tm, int parNum){
  _myMinuit=tm;
  _pN = parNum;
}

bool FitParameterli::MinuitOK() const{
  bool OK = (0!= getMinuit());
  if(!OK) return false;
  OK &= (_pN > 0);
  if(!OK) return false;
  OK &= (_pN < getMinuit()->fMaxpar);
  if(!OK) return false;
  OK &= (1 == getMinuit()->fEmpty);
  return OK;
}
double FitParameterli::mean() const{
  if(MinuitOK()){
    return getMinuit()->fU[parNumber()];
  }else{
    return meanInit();
  }
}
double FitParameterli::min() const{
  if(MinuitOK()){
    return getMinuit()->fAlim[parNumber()];
  }else{
    return minInit();
  }
}
double FitParameterli::max() const{
  if(MinuitOK()){
    return getMinuit()->fBlim[parNumber()];
  }else{
    return maxInit();
  }
}
double FitParameterli::errPos() const{
  if(MinuitOK()){
    return getMinuit()->fErp[parNumber()];
  }else{
    return stepInit();
  }
}
double FitParameterli::errNeg() const{
  if(MinuitOK()){
    return getMinuit()->fErn[parNumber()];
  }else{
    return stepInit();
  }
}
double FitParameterli::err() const{
  if(MinuitOK()){
    return getMinuit()->fWerr[parNumber()];
  }else{
    return stepInit();
  }
}

void FitParameterli::print(std::ostream& os) const{
  // ... in a format that can be read back in
  os << name()
     << "\t" << iFixInit()
     << "\t" << mean()
     << "\t" << err()
     << "\t" << min()
     << "\t" << max();
}
void FitParameterli::printMore(std::ostream& os) const{
  os << "## name"
     << "\t" << "fixed?"
     << "\t" << "mean"
     << "\t" << "error"
     << "\t" << "min"
     << "\t" << "max"
     << std::endl;
  print(os);
}

//
