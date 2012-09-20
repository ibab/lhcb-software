// $Id: PatCheckerUpgrade.h,v 1.3 2008-06-01 17:28:53 mjohn Exp $
#ifndef PATCHECKERUPGRADE_H 
#define PATCHECKERUPGRADE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/ISequencerTimerTool.h"

// local
#include "PatCounterUpgrade.h"
#include "PatKernel/IPatTTCounter.h"

/** @class PatCheckerUpgrade PatCheckerUpgrade.h
 *  Check the quality of the pattern, by comparing to MC information
 *
 *  @author Olivier Callot
 *  @date   2005-03-29
 * Modified by Wenbin Qian for VP
 *  @adapted to A-Team framework 2007-08-20 SHM
 */
  
  class PatCheckerUpgrade : public GaudiAlgorithm {
  public: 
    /// Standard constructor
    PatCheckerUpgrade( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~PatCheckerUpgrade( ); ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization
    
  protected:
    
    
  private:
    bool m_level1;
    bool m_checkMissedVP;
    bool m_checkMissedForward;
    bool m_checkMissedSeed;
    bool m_checkMissedDownstream;
    bool m_checkMatchNotForward;
    
    PatCounterUpgrade* m_vP;
    PatCounterUpgrade* m_veloTT;
    PatCounterUpgrade* m_forward; 
    PatCounterUpgrade* m_tsa;
    PatCounterUpgrade* m_tTrack;
    PatCounterUpgrade* m_match;
    PatCounterUpgrade* m_downst;
    PatCounterUpgrade* m_kSNew;
    PatCounterUpgrade* m_best;
    
    IPatTTCounter* m_ttForward;
    IPatTTCounter* m_ttMatch;
    IPatTTCounter* m_ttDownst;
    
    //== Vector of the counters
    std::vector<PatCounterUpgrade*> m_allCounters;
    
    std::vector<IPatTTCounter*> m_allTTCounters;

    bool m_measureTime;
    ISequencerTimerTool* m_timer;
    int m_initTime;
    int m_partTime;

    std::string vPName;
    std::string m_hltVPLocation;
    std::string m_hltVeloTTLocation;
    std::string m_hltForwardLocation;
  }; 

#endif // PATCHECKERUPGRADE_H
