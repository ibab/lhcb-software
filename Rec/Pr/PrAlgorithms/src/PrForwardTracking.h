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
 *
 *  \brief Main algorithm for the Forward tracking of the upgrade
 *  \brief The main work is done in PrForwardTool
 *
 *   Parameters:
 * - InputName: Name of the input container
 * - OutputName: Name of the output container
 * - HitManagerName: Name of the hit manager
 * - ForwardToolName: Name of the tool that does the actual forward tracking
 * - DeltaQuality: Quality criterion to reject duplicates
 * - DebugToolName: Name of the debug tool
 * - WantedKey: Key of the MCParticle one wants to look at 
 * - VeloKey: Key of the Velo track one wants to look at
 * - TimingMeasurement: Print table with timing measurements
 *
 *
 *  @author Olivier Callot
 *  @date   2012-03-20
 *  @author Michel De Cian
 *  @date   2014-03-12 Changes with make code more standard and faster
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
  
  /** Check if track can be forwarded
   *  @brief Check if track can be forwarded
   *  @param track Velo track that might be forwarded
   *  @return bool 
   */
  bool acceptTrack(const LHCb::Track* track ){
    return !(track->checkFlag( LHCb::Track::Invalid) ) && !(track->checkFlag( LHCb::Track::Backward) );
  }
  
  
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
