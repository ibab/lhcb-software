// $Id: BTaggingInclusive.h
#ifndef DEBUG_BTAGGINGINCLUSIVE_H 
#define DEBUG_BTAGGINGINCLUSIVE_H 1

// Include files
#include "GaudiKernel/AlgFactory.h"
#include "Event/FlavourTag.h"
#include "Kernel/IDebugTool.h"

#include "Event/HepMCEvent.h"
#include "Event/GenCollision.h"
#include "Event/GenHeader.h"
#include "Kernel/IEvtTypeSvc.h"
#include "Kernel/IMCDecayFinder.h"
#include "Kernel/IDebugTool.h"

// from DaVinci
#include "Kernel/DVAlgorithm.h"

/** @class BTaggingInclusive BTaggingInclusive.h debug/BTaggingInclusive.h
 *  
 *
 *  @author Onuora Awunor
 *  @date   2006-05-11
 */
class BTaggingInclusive: public DVAlgorithm {
public: 
  /// Standard constructor
  BTaggingInclusive( const std::string& , ISvcLocator* ); 

  virtual ~BTaggingInclusive( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

 protected:
  StatusCode setDecayToFind( const int evtCode );

 private:
  /// Vector of locations of the tags to monitor
  std::string m_tagslocation; 
  std::string m_ostagslocation;

  IDebugTool* m_debug;

  bool            m_fromData;        ///< flag read event code from data
  int             m_evtCode;         ///< event code to test
  bool            m_setDecay;        ///< Flag is decay has been set
  IEvtTypeSvc*    m_evtTypeSvc;      ///< Pointer to service
  IMCDecayFinder* m_mcFinder;        ///< Pointer to tool

  int nsele,nrt[50],nwt[50];
 
};
#endif // DEBUG_BTAGGINGINCLUSIVE_H
