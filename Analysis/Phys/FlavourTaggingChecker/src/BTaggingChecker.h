// $Id: BTaggingChecker.h,v 1.2 2006-10-24 10:21:06 jpalac Exp $
#ifndef BTAGGINGCHECKER_H 
#define BTAGGINGCHECKER_H 1

// Include files
#include "Event/FlavourTag.h"

#include "Event/HepMCEvent.h"
#include "Event/GenCollision.h"
#include "Event/GenHeader.h"
#include "Kernel/IEvtTypeSvc.h"
#include "Kernel/IMCDecayFinder.h"
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
  StatusCode setDecayToFind( const int evtCode );

 private:
  /// Vector of locations of the tags to monitor
  std::string m_tags_location; 
  IDebugTool* m_debug;

  bool            m_fromData;        ///< flag read event code from data
  int             m_evtCode;         ///< event code to test
  bool            m_setDecay;        ///< Flag is decay has been set
  IEvtTypeSvc*    m_evtTypeSvc;      ///< Pointer to service
  IMCDecayFinder* m_mcFinder;        ///< Pointer to tool

  int nsele,nrt[50],nwt[50];

};
#endif // BTAGGINGCHECKER_H
