#ifndef MAKE_PREINTEGRATED_AMPLITUDES_HH
#define MAKE_PREINTEGRATED_AMPLITUDES_HH

#include "Mint/DalitzEventPattern.h"
#include <string>
#include "TRandom.h"

class MakePreIntegratedAmplitudes{
  DalitzEventPattern _pat;
  std::string _opt;
  double _prec;
  std::string _fname;
  std::string _eventsFname;
  bool _generateEventsIfNeeded;
  TRandom* _rnd;
  const std::string& makeFname(const std::string& prefix);
  const std::string& makeEventsFname();
  bool includeCPCon() const;
 public:
  MakePreIntegratedAmplitudes(const DalitzEventPattern& pat
			      , const std::string& opt
			      , double prec
			      , const std::string& integEventsFname=""
			      , const std::string& fnamePrefix=""
			      , const std::string& generateEventsIfNeeded=
			      "DoGenerate"
			      , TRandom* rnd=gRandom
			      );

  const std::string& fname()const {return _fname;}
  const std::string& eventsFname()const {return _eventsFname;}
  std::string defaultEventsFname() const;
  bool doit();
};
#endif
//
