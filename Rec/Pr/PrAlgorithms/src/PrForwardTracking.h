#ifndef PRFORWARDTRACKING_H 
#define PRFORWARDTRACKING_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "GaudiAlg/ISequencerTimerTool.h"

#include "PrKernel/IPrDebugTool.h"
#include "PrKernel/PrHitManager.h"

#include "IPrForwardTool.h"

/** @class PrForwardTracking PrForwardTracking.h
 *  Main algorithm fo rthe Forward tracking of the upgrade
 *
 *  @author Olivier Callot
 *  @date   2012-03-20
 */
class PrForwardTracking : public GaudiAlgorithm {
public: 
  /// Standard constructor
  PrForwardTracking( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrForwardTracking( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string    m_inputName;
  std::string    m_outputName;
  std::string    m_hitManagerName;
  std::string    m_forwardToolName;
  PrHitManager*  m_hitManager;
  IPrForwardTool* m_forwardTool;
  float          m_deltaQuality;

  //== Debugging controls
  std::string    m_debugToolName;
  int            m_wantedKey;
  int            m_veloKey;
  IPrDebugTool*  m_debugTool;
  bool           m_doTiming;
  ISequencerTimerTool* m_timerTool;
  int            m_timeTotal;
  int            m_timePrepare;
  int            m_timeExtend;
  int            m_timeFinal;
};
#endif // PRFORWARDTRACKING_H
