#ifndef FAST_AMPLITUDE_HH
#define FAST_AMPLITUDE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:57 GMT

#include <map>
#include <complex>

#include "Mint/IReturnRealForEvent.h"
#include "Mint/IReturnComplexForEvent.h"

#include "Mint/IDalitzEvent.h"
#include "Mint/Amplitude.h"

class FastAmplitude : public Amplitude
, virtual public MINT::IReturnRealForEvent<IDalitzEvent>
  , virtual public MINT::IReturnComplexForEvent<IDalitzEvent>
{
 protected:
  std::map<IDalitzEvent*, std::complex<double> > _resultMap;

  bool knownEvent(IDalitzEvent& evt, std::complex<double>& value);

 public:
  FastAmplitude( const DecayTree& decay
		 , char SPD_Wave='?'
		 , const std::string& opt=""
		 );
  
  FastAmplitude( const AmpInitialiser& ampInit
		 );
  
  FastAmplitude( const FastAmplitude& other);

  FastAmplitude( const Amplitude& other);

  virtual std::complex<double> getVal(IDalitzEvent& evt);
  virtual std::complex<double> getVal(IDalitzEvent* evt); // for backwards compatibilitiy

};

#endif
//
