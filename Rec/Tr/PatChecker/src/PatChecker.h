#ifndef PATCHECKER_H 
#define PATCHECKER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/ISequencerTimerTool.h"

// local
#include "PatCounter.h"
#include "PatKernel/IPatTTCounter.h"

/** @class PatChecker PatChecker.h
 *  Check the quality of the pattern, by comparing to MC information
 *
 *  @author Olivier Callot
 *  @date   2005-03-29
 *  @adapted to A-Team framework 2007-08-20 SHM
 */
  
  class PatChecker : public GaudiAlgorithm {
  public: 
    /// Standard constructor
    PatChecker( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~PatChecker( ); ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization
    
  protected:
    
    
  private:
    bool m_checkMissedVeloSpace;
    bool m_checkMissedForward;
    bool m_checkMissedSeed;
    bool m_checkMissedDownstream;
    bool m_checkMatchNotForward;
    
    PatCounter* m_veloRz;
    PatCounter* m_veloSpace;
    PatCounter* m_veloTT;
    PatCounter* m_forward; 
    PatCounter* m_tsa;
    PatCounter* m_tTrack;
    PatCounter* m_match;
    PatCounter* m_downst;
    PatCounter* m_kSNew;
    PatCounter* m_best;
    
    IPatTTCounter* m_ttForward;
    IPatTTCounter* m_ttMatch;
    IPatTTCounter* m_ttDownst;
    
    //== Vector of the counters
    std::vector<PatCounter*> m_allCounters;
    
    std::vector<IPatTTCounter*> m_allTTCounters;

    bool m_measureTime;
    ISequencerTimerTool* m_timer;
    int m_initTime;
    int m_partTime;

    std::string veloSpaceName;    
    std::string m_hltRZVeloLocation;
    std::string m_hltVeloLocation;
    std::string m_hltVeloTTLocation;
    std::string m_hltForwardLocation;
  }; 

#endif // PATCHECKER_H
