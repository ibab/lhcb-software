// $Id: DoubleTagCorrelation.h
#ifndef DOUBLETAGCORRELATION_H 
#define DOUBLETAGCORRELATION_H 1

// Include files
#include "Event/FlavourTag.h"

#include "Event/HepMCEvent.h"
#include "Event/GenCollision.h"
#include "Event/GenHeader.h"
#include "Kernel/IEvtTypeSvc.h"
#include "MCInterfaces/IMCDecayFinder.h"
#include "MCInterfaces/IPrintMCDecayTreeTool.h"
#include "MCInterfaces/IForcedBDecayTool.h"

// from DaVinci
#include "Kernel/DVAlgorithm.h"

// from local

/** @class DoubleTagCorrelation DoubleTagCorrelation.h
 *  
 *  Algorithm to evaluate OS/SS tag correlation
 *  @author Chris Barnes
 *  @date   2006-01-28
 */

class DoubleTagCorrelation : public DVAlgorithm {

 public:
  /// Standard constructor
  DoubleTagCorrelation( const std::string& , ISvcLocator* );

  virtual ~DoubleTagCorrelation( ); ///< Destructor

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

  int** matrix;

};
#endif // DOUBLETAGCORRELATION_H
