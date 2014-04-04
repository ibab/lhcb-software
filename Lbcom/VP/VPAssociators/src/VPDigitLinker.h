#ifndef VPDIGITLINKER_H
#define VPDIGITLINKER_H 1

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class VPDigitLinker VPDigitLinker.h
 *  This algorithm creates association tables between the channel IDs of 
 *  the VP digits and the corresponding MC hits and particles.
 *  
 */

class VPDigitLinker : public GaudiAlgorithm {

public:
  /// Standard constructor
  VPDigitLinker(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~VPDigitLinker();

  /// Algorithm execution
  virtual StatusCode execute();

private:

  /// Location of input digits
  std::string m_digitLocation;
  /// Location of input MC digits
  std::string m_mcdigitLocation;
  /// Locations of output linker tables
  std::string m_hitLinkLocation; 
  std::string m_particleLinkLocation; 

};

#endif
