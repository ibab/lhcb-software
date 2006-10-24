// $Id: DoubleTagCorrelation.h
#ifndef DOUBLETAGCORRELATION_H 
#define DOUBLETAGCORRELATION_H 1

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

 protected:
  StatusCode setDecayToFind( const int evtCode );

 private:
  /// Vector of locations of the tags to monitor
  std::string m_tagslocation; 
  std::string m_ostagslocation;

  bool            m_fromData;        ///< flag read event code from data
  int             m_evtCode;         ///< event code to test
  bool            m_setDecay;        ///< Flag is decay has been set
  IEvtTypeSvc*    m_evtTypeSvc;      ///< Pointer to service
  IMCDecayFinder* m_mcFinder;        ///< Pointer to tool
  IDebugTool* m_debug;

  int** matrix;

};
#endif // DOUBLETAGCORRELATION_H
