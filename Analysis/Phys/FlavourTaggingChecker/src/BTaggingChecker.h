// $Id: BTaggingChecker.h,v 1.3 2006-11-03 20:51:41 musy Exp $
#ifndef BTAGGINGCHECKER_H 
#define BTAGGINGCHECKER_H 1

// Include files
#include "Event/FlavourTag.h"

#include "Event/HepMCEvent.h"
#include "Event/GenCollision.h"
#include "Event/GenHeader.h"
#include "Kernel/IDebugTool.h"

// from DaVinci
#include "Kernel/DVAlgorithm.h"

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

 protected:
  LHCb::MCParticle* findBForcedToDecay( void );
  LHCb::MCParticle* associatedofHEP( HepMC::GenParticle* );

 private:
  /// Vector of locations of the tags to monitor
  std::string m_tags_location; 
  IDebugTool* m_debug;

  int nsele,nrt[50],nwt[50];

};
#endif // BTAGGINGCHECKER_H
