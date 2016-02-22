// $Id: PatForward.h,v 1.2 2007-11-19 15:06:38 aperiean Exp $
#ifndef PATFORWARD_H
#define PATFORWARD_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/ISequencerTimerTool.h"
#include "TrackInterfaces/IPatForwardTool.h"
#include "TfKernel/TStationHitManager.h"
#include "PatKernel/PatForwardHit.h"


class IOTHitCreator;

  /** @class PatForward PatForward.h
   *  Forward pattern recognition. Connect a Velo track to the T stations.
   *
   *  @author Olivier Callot
   *  @date   2005-04-01 Initial version
   *  @date   2007-08-20 Update for A-Team framework 
   */

class PatForward : public GaudiAlgorithm {
  public:
    /// Standard constructor
    PatForward( const std::string& name, ISvcLocator* pSvcLocator );
    
    ~PatForward( ) override = default; ///< Destructor
    
    StatusCode initialize() override;    ///< Algorithm initialization
    StatusCode execute   () override;    ///< Algorithm execution
    
  private:
    
    int overlaps(const LHCb::Track* lhs, const LHCb::Track* rhs ) const;
    
    int              m_fwdTime;

    std::string      m_inputTracksName;
    std::string      m_outputTracksName;
    std::string      m_forwardToolName;
    
    int m_deltaNumberInTT;
    int m_deltaNumberInT;

    unsigned int m_maxNVelo;
    bool m_doClean;
    bool m_doTiming;

    unsigned int m_maxNumberOTHits;      
    unsigned int m_maxNumberITHits; 
    
    const Tf::IOTHitCreator* m_otHitCreator;

        
    IPatForwardTool*     m_forwardTool;
    ISequencerTimerTool* m_timerTool;
    bool             m_writeNNVariables; // switch on or off NN var. writing
  };
#endif // PATFORWARD_H
