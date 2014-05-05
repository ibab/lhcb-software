// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:57 GMT
#include "Mint/FastAmplitude.h"

using namespace std;
using namespace MINT;

FastAmplitude::FastAmplitude( const DecayTree& decay
			      , char SPD_Wave
			      , const std::string& opt
			      )
  : Amplitude(decay, SPD_Wave, opt)
{}

FastAmplitude::FastAmplitude( const AmpInitialiser& ampInit
			      )
  : Amplitude(ampInit)
{}

FastAmplitude::FastAmplitude( const FastAmplitude& other)
  : IReturnRealForEvent<IDalitzEvent>()
  , IReturnComplexForEvent<IDalitzEvent>()
  , Amplitude(other)
  , _resultMap(other._resultMap)
{}

FastAmplitude::FastAmplitude( const Amplitude& other)
  : Amplitude(other)
{
  _resultMap.clear();
}

bool FastAmplitude::knownEvent(IDalitzEvent& evt, complex<double>& value){
  return evt.retrieveComplex(this, value);
}

std::complex<double> FastAmplitude::getVal(IDalitzEvent* evt){
  return getVal(*evt);
}

complex<double> FastAmplitude::getVal(IDalitzEvent& evt){
  bool dbThis=false;
  complex<double> result(-9999.0, 0);

  if(dbThis){
    cout << "FastAmplitude getting value:" << endl;
    cout << " eventPtr = " << &evt << endl;
    cout << " event = " << evt.p(1).E() << endl;
  }
  if(knownEvent(evt, result)){
    if(dbThis){
      cout << " this " << this  
	   << " remembering result: " << result 
	   << endl;
      complex<double> newResult = Amplitude::getVal(evt);
      cout << " compare to newly calculated: "
	   << newResult << endl;
      if(result != newResult) cout << " aaaaaaaaaaaaaaaaaa" << endl;
    }
    return result;
  }
  if(dbThis) cout << " result is not known - getting Amplitude " << endl;
  result = Amplitude::getVal(evt);
  evt.setComplex(this,result);
  if(dbThis)cout << "FastAmplitude returning " << result << endl;
  return result;
}


//
