// $Id: $
#ifndef DEFTTESTALG_H 
#define DEFTTESTALG_H 1

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// FTDet
class DeFTDetector;

/** @class DeFTTestAlg DeFTTestAlg.h "FTDet/DeFTTestAlg.h"
 *   
 *  A GaudiAlgorithm with solely test purposes, which calls (tests)
 *  the methods defined in the FT det. element classes.
 *
 *  @author Plamen Hopchev
 *  @date   2012-04-25
 */

class DeFTTestAlg : public GaudiAlgorithm {

public: 

  /// Standard constructor
  DeFTTestAlg( const std::string& name, ISvcLocator* pSvcLocator );

  ~DeFTTestAlg( ); ///< Destructor

  StatusCode initialize();    ///< Algorithm initialization
  StatusCode execute   ();    ///< Algorithm execution
  StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  DeFTDetector* m_deFT; ///< A pointer to the top-level FT det. element
  std::string m_mcHitsLocation; ///< TES location of the FT MC hits
  
};
#endif // DEFTTESTALG_H
