// $Id: BTaggingChecker.h,v 1.9 2009-02-25 09:45:18 musy Exp $
#ifndef BTAGGINGCHECKER_H 
#define BTAGGINGCHECKER_H 1

// Include files
#include "Event/FlavourTag.h"

#include "Event/HepMCEvent.h"
#include "Event/GenCollision.h"
#include "Event/GenHeader.h"
#include "Kernel/IPrintDecayTreeTool.h"
#include "MCInterfaces/IForcedBDecayTool.h"
#include "Kernel/IBackgroundCategory.h"

#include "Event/L0DUReport.h"
#include "Event/HltDecReports.h"

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
  IPrintDecayTreeTool* m_debug;
  IForcedBDecayTool* m_forcedBtool;
  IBackgroundCategory* m_bkg;

  //IForcedBDecayTool* m_forcedBtool;
  const LHCb::MCParticle* forcedB(void);
  LHCb::MCParticle* associatedofHEP(HepMC::GenParticle* );
  bool  m_requireHltTrigger;
  
  int nsele,nrt[50],nwt[50];

};
#endif // BTAGGINGCHECKER_H
