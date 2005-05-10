// $Id: TestHltScore.h,v 1.1 2005-05-10 16:37:00 pkoppenb Exp $
#ifndef TESTHLTSCORE_H 
#define TESTHLTSCORE_H 1

// Include files
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class TestHltScore TestHltScore.h
 *  
 *  Reads and tests HltScore class
 *
 *  @author Patrick KOPPENBURG
 *  @date   2005-05-10
 */
class HltAlgorithmScore ;
class HltScore ;
class IAlgorithm2ID ;

class TestHltScore : public GaudiAlgorithm {
public: 
  /// Standard constructor
  TestHltScore( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TestHltScore( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  StatusCode printAlgorithm(const HltAlgorithmScore* );
  const HltScore* getScore(void);
  StatusCode printScore(const HltScore*);
  StatusCode checkScore(const HltScore*); 
  StatusCode fillStats(const HltScore*); 
  StatusCode printStatistics();

private:
  IAlgorithm2ID* m_algorithm2ID ; ///< AlgorithmID

  // statistics
  int m_selEvents ; // number of selected events
  int m_rejEvents ; // number of rejected events
  int m_notEvents ; // number of not processed events
  int m_selPV ; // number of PVs in selected events
  int m_rejPV ; // number of PVs in rejected events
  int m_selTracks ; // number of tracks in selected events
  int m_rejTracks ; // number of tracks in rejected events
  int m_selPhotons ; // number of photons in selected events
  int m_rejPhotons ; // number of photons in rejected events
  int m_selPi0s ; // number of Pi0s in selected events
  int m_rejPi0s ; // number of Pi0s in rejected events
  int m_selB    ; // number of B in selected events
  int m_selDstar ; // number of D* in rejected events

};
#endif // TESTHLTSCORE_H
