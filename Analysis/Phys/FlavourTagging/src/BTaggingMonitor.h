// $Id: BTaggingMonitor.h,v 1.6 2005-03-01 09:57:05 musy Exp $
#ifndef BTAGGINGMONITOR_H 
#define BTAGGINGMONITOR_H 1

// Include files
// from STL
#include <string>
#include <vector>
#include <algorithm>
// from Gaudi
#include "GaudiKernel/AlgFactory.h"

#include "Event/EventHeader.h"
#include "Event/FlavourTag.h"
#include "DaVinciMCTools/IDebugTool.h"
#include "Event/GenMCLink.h"
#include "Event/TrgDecision.h"

// from DaVinci
#include "Kernel/DVAlgorithm.h"
#include "DaVinciAssociators/Particle2MCLinksAsct.h"

/** @class BTaggingMonitor BTaggingMonitor.h
 *  
 *
 *  @author Marco Musy
 *  @date   2004-02-15
 */
class BTaggingMonitor : public DVAlgorithm {
public:
  /// Standard constructor
  BTaggingMonitor( const std::string& , ISvcLocator* );

  virtual ~BTaggingMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:
  /// Vector of locations of the tags to monitor
  std::vector<std::string> m_tags_locations; 
  Particle2MCLinksAsct::IAsct* m_pAsctLinks; 
  IDebugTool* m_debug;

  int nsele,nrt[50],nwt[50];

};
#endif // BTAGGINGMONITOR_H
