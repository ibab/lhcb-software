// $Id: BTaggingChecker.h,v 1.1.1.1 2005-07-04 15:24:22 pkoppenb Exp $
#ifndef BTAGGINGCHECKER_H 
#define BTAGGINGCHECKER_H 1

// Include files
#include <string>
#include <vector>
#include <algorithm>
#include "GaudiKernel/AlgFactory.h"
#include "Event/FlavourTag.h"
#include "DaVinciMCTools/IDebugTool.h"
#include "Event/GenMCLink.h"

// from DaVinci
#include "Kernel/DVAlgorithm.h"
#include "DaVinciAssociators/Particle2MCLinksAsct.h"

/** @class BTaggingChecker BTaggingChecker.h
 *  
 *  Algorithm to read a location in TES and evaluate tag performances
 *  @author Marco Musy
 *  @date   2004-02-15
 */
class BTaggingChecker : public DVAlgorithm {
public:
  /// Standard constructor
  BTaggingChecker( const std::string& , ISvcLocator* );

  virtual ~BTaggingChecker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:
  /// Vector of locations of the tags to monitor
  std::string m_tags_location; 
  Particle2MCLinksAsct::IAsct* m_pAsctLinks; 
  IDebugTool* m_debug;

  int nsele,nrt[50],nwt[50];

};
#endif // BTAGGINGCHECKER_H
