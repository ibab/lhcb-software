#ifndef FAST_AMPLITUDE_HH
#define FAST_AMPLITUDE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:57 GMT

#include <map>
#include <complex>

#include "Mint/IGetRealEvent.h"
#include "Mint/IReturnComplex.h"

#include "Mint/IDalitzEvent.h"
#include "Mint/Amplitude.h"

class FastAmplitude : public Amplitude
, virtual public MINT::IGetRealEvent<IDalitzEvent>
  , virtual public MINT::IReturnComplex
{
 protected:
  std::map<IDalitzEvent*, std::complex<double> > _resultMap;

  bool knownEvent(std::complex<double>& value);

 public:
  FastAmplitude( const DecayTree& decay
		 , IDalitzEventAccess* events
		 , char SPD_Wave='?'
		 , const std::string& opt=""
		 );
  
  FastAmplitude( const DecayTree& decay
		 , IDalitzEventList* events
		 , char SPD_Wave='?'
		 , const std::string& opt=""
		 );
  
  FastAmplitude( const AmpInitialiser& ampInit
		 , IDalitzEventAccess* events
		 );
  FastAmplitude( const AmpInitialiser& ampInit
		 , IDalitzEventList* events
		 );
  
  FastAmplitude( const FastAmplitude& other);
  FastAmplitude( const FastAmplitude& other, IDalitzEventAccess* newEvents);
  FastAmplitude( const FastAmplitude& other, IDalitzEventList* newEvents);

  FastAmplitude( const Amplitude& other);
  FastAmplitude( const Amplitude& other, IDalitzEventAccess* newEvents);
  FastAmplitude( const Amplitude& other, IDalitzEventList* newEvents);


  virtual std::complex<double> getVal();
  virtual std::complex<double> getVal(IDalitzEvent* evt);

};

#endif
//
