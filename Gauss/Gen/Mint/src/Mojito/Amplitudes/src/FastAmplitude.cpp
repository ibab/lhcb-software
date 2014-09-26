// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:57 GMT
#include "Mint/FastAmplitude.h"

using namespace std;
using namespace MINT;

FastAmplitude::FastAmplitude( const DecayTree& decay
			      , IDalitzEventAccess* events
			      , char SPD_Wave
			      , const std::string& opt
			      )
  : Amplitude(decay, events, SPD_Wave, opt)
  , _rememberNumber(-9999)
{}

FastAmplitude::FastAmplitude( const DecayTree& decay
			      , IDalitzEventList* events
			      , char SPD_Wave
			      , const std::string& opt
			      )
  : Amplitude(decay, events, SPD_Wave, opt)
  , _rememberNumber(-9999)
{}

FastAmplitude::FastAmplitude( const AmpInitialiser& ampInit
			      , IDalitzEventAccess* events
			      )
  : Amplitude(ampInit, events)
  , _rememberNumber(-9999)
{}
FastAmplitude::FastAmplitude( const AmpInitialiser& ampInit
			      , IDalitzEventList* events
			      )
  : Amplitude(ampInit, events)
  , _rememberNumber(-9999)
{}

FastAmplitude::FastAmplitude( const FastAmplitude& other)
  : IBasicEventAccess<IDalitzEvent>()
  , IEventAccess<IDalitzEvent>()
  , IDalitzEventAccess()
  , IReturnReal()
  , IGetRealEvent<IDalitzEvent>()
  , IReturnComplex()
  , Amplitude(other)
  , _resultMap(other._resultMap)
  , _rememberNumber(-9999)
{}

FastAmplitude::FastAmplitude( const FastAmplitude& other
			      , IDalitzEventAccess* newEvents)
  : Amplitude(other, newEvents)
  , _rememberNumber(-9999)
{
  _resultMap.clear();
}

FastAmplitude::FastAmplitude( const FastAmplitude& other
			      , IDalitzEventList* newEvents)
  : Amplitude(other, newEvents)
  , _rememberNumber(-9999)
{
  _resultMap.clear();
}

FastAmplitude::FastAmplitude( const Amplitude& other)
  : Amplitude(other)
  , _rememberNumber(-9999)
{
  _resultMap.clear();
}
FastAmplitude::FastAmplitude( const Amplitude& other
			      , IDalitzEventAccess* newEvents)
  : Amplitude(other, newEvents)
{
  _resultMap.clear();
}
FastAmplitude::FastAmplitude( const Amplitude& other
			      , IDalitzEventList* newEvents)
  : Amplitude(other, newEvents)
  , _rememberNumber(-9999)
{
  _resultMap.clear();
}

long int FastAmplitude::rememberNumber() const{
  if(_rememberNumber < 0){
    _rememberNumber = DalitzEvent::assignUniqueRememberNumber();
  }
  return _rememberNumber;
}
bool FastAmplitude::knownEvent(complex<double>& value){
  // old cashing - much (!) slower, use only for tests etc
  // return getEvent()->retrieveComplex(this, value);
  // new cashing:
  return getEvent()->retrieveComplex(rememberNumber(), value);
}

std::complex<double> FastAmplitude::getVal(IDalitzEvent* evt){
  //  bool dbthis=false;
  this->setEvent(evt);
  std::complex<double> result = this->getVal();
  this->resetEventRecord();
  return result;
}

complex<double> FastAmplitude::getVal(){
  /* debug
  //  cout << "returning Amplitude for " << theDecay().oneLiner() << endl;
  complex<double> returnVal =  Amplitude::getVal(); // dbg
  //cout << " ... with value " << returnVal << endl;
  return returnVal;
  */
  bool dbThis=false;
  complex<double> result(-9999.0, 0);

  if(0 == getEvent()){
    if(dbThis) cout << " No event in FastAmplitude::getVal()!!!" << endl;
    return 0;
  }

  if(dbThis){
    cout << "FastAmplitude getting value:" << endl;
    cout << " eventPtr = " << getEvent() << endl;
    cout << " event = " << getEvent()->p(1).E() << endl;
  }
  if(knownEvent(result)){
    if(dbThis)cout << " this " << this  << " number " << rememberNumber()
		   << " remembering result: " << result 
		   << endl;
    return result;
  }
  if(dbThis) cout << " result is not known - getting Amplitude " << endl;
  result = Amplitude::getVal();
  getEvent()->setComplex(rememberNumber(),result);
  if(dbThis)cout << "FastAmplitude returning " << result << endl;
  return result;
}

//
