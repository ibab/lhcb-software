// $Id: BTaggingMonitor.h,v 1.7 2005-07-04 08:20:05 pkoppenb Exp $
#ifndef BTAGGINGMONITOR_H 
#define BTAGGINGMONITOR_H 1

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

/** @class BTaggingMonitor BTaggingMonitor.h
 *  
 *  Algorithm to read a location in TES and evaluate tag performances
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
  std::string m_tags_location; 
  Particle2MCLinksAsct::IAsct* m_pAsctLinks; 
  IDebugTool* m_debug;

  int nsele,nrt[50],nwt[50];

};
#endif // BTAGGINGMONITOR_H
