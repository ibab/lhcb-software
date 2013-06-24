#ifndef BCMDIGI_BCMDIGITIZATION_H
#define BCMDIGI_BCMDIGITIZATION_H 1

#include <string>

// From Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/MCHit.h"
#include "Event/BcmDigit.h"

// local

// Event model

// Forward declaration
//class DeBcm;

class BcmDigitization : public GaudiAlgorithm {
public:
  // Constructor: A constructor of this form must be provided.
	BcmDigitization(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();
  
private:
  //  std::string m_bcmUpDetLocation;
  //  std::string m_bcmDownDetLocation;
  std::string m_bcmHitsLocation;
  std::string m_bcmDigitLocation;
  std::string m_bcmMCDigitLocation;
  //  DeBcm* m_bcmUpDet;
  //  DeBcm* m_bcmDownDet;
  bool m_bcmMCDigits;
};

#endif
