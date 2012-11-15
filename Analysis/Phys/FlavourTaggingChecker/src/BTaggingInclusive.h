// $Id: BTaggingInclusive.h
#ifndef DEBUG_BTAGGINGINCLUSIVE_H 
#define DEBUG_BTAGGINGINCLUSIVE_H 1

// Include files
#include "GaudiKernel/AlgFactory.h"
#include "Event/FlavourTag.h"
#include "MCInterfaces/IPrintMCDecayTreeTool.h"

#include "Event/HepMCEvent.h"
#include "Event/GenCollision.h"
#include "Event/GenHeader.h"
#include "Kernel/IEvtTypeSvc.h"
#include "MCInterfaces/IMCDecayFinder.h"
#include "MCInterfaces/IPrintMCDecayTreeTool.h"
#include "MCInterfaces/IForcedBDecayTool.h"

// from DaVinci
#include "Kernel/DaVinciAlgorithm.h"

// from local

/** @class BTaggingInclusive BTaggingInclusive.h debug/BTaggingInclusive.h
 *  
 *
 *  @author Onuora Awunor
 *  @date   2006-05-11
 */
class BTaggingInclusive: public DaVinciAlgorithm {
public: 
  /// Standard constructor
  BTaggingInclusive( const std::string& , ISvcLocator* ); 

  virtual ~BTaggingInclusive( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization


 private:
  /// Vector of locations of the tags to monitor
  std::string m_tagslocation; 
  std::string m_ostagslocation;

  IPrintMCDecayTreeTool* m_debug;

  IForcedBDecayTool* m_forcedBtool;
  LHCb::MCParticle* associatedofHEP(HepMC::GenParticle* ) ;

  int nsele,nrt[50],nwt[50];
 
};
#endif // DEBUG_BTAGGINGINCLUSIVE_H
