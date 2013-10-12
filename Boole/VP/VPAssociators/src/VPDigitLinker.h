#ifndef VPDIGITLINKER_H
#define VPDIGITLINKER_H 1

// STL
#include <map>
// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// LHCb
// Event/MCEvent
#include "Event/MCHit.h"
#include "Event/MCVPDigit.h"
// Event/DigiEvent
#include "Event/VPDigit.h"

/** @class VPDigitLinker VPDigitLinker.h
 *  
 */

class VPDigitLinker : public GaudiAlgorithm {

public:
  /// Standard constructor
  VPDigitLinker(const std::string& name,ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~VPDigitLinker();
  virtual StatusCode execute();        ///< Algorithm execution

private:

  std::string m_digitLocation;
  std::string m_mcdigitLocation;
  std::string m_hitLinkLocation; 
  std::string m_particleLinkLocation; 

  bool associateToTruth(const LHCb::VPDigit* digit,
                        const LHCb::MCVPDigits* mcdigits,
                        std::map<const LHCb::MCHit*, double>& hitMap);
  void mergeDeltaRays(const std::map<const LHCb::MCHit*, double>& inputMap,
                      std::map<const LHCb::MCHit*, double>& hitMap);

};

#endif
