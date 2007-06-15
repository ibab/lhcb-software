// $Id: BTaggingChecker.h,v 1.6 2007-06-15 16:46:20 jpalac Exp $
#ifndef BTAGGINGCHECKER_H 
#define BTAGGINGCHECKER_H 1

// Include files
#include "Event/FlavourTag.h"

#include "Event/HepMCEvent.h"
#include "Event/GenCollision.h"
#include "Event/GenHeader.h"
#include "Kernel/IDebugTool.h"
#include "MCInterfaces/IForcedBDecayTool.h"

// from DaVinci
#include "Kernel/DVAlgorithm.h"

// from local

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
  IDebugTool* m_debug;
  IForcedBDecayTool* m_forcedBtool;

  //IForcedBDecayTool* m_forcedBtool;
  const LHCb::MCParticle* forcedB(void);
  LHCb::MCParticle* associatedofHEP(HepMC::GenParticle* );

  int nsele,nrt[50],nwt[50];

};
#endif // BTAGGINGCHECKER_H
