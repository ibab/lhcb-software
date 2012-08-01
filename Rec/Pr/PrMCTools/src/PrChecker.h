// $Id: $
#ifndef PRCHECKER_H
#define PRCHECKER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// local
#include "PrCounter.h"

/** @class PrChecker PrChecker.h
 *  Check the quality of the pattern, by comparing to MC information
 *
 *  @author Olivier Callot
 *  @date   2005-03-29
 */
class EffCounter {
public:
  EffCounter() {
    nEvt    = 0;
    total   = 0;
    ghost   = 0;
    nLong   = 0;
    okLong  = 0;
    nLong5 = 0;
    okLong5 = 0;
  };
  ~EffCounter() {};
  int nEvt;
  int total;
  int ghost;
  int nLong;
  int okLong;
  int nLong5;
  int okLong5;
};



class PrChecker : public GaudiAlgorithm {
public:
  /// Standard constructor
  PrChecker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrChecker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:


private:
  std::string m_veloTracks;
  std::string m_forwardTracks;
  std::string m_seedTracks;
  
  PrCounter* m_velo;
  PrCounter* m_forward;
  PrCounter* m_tTrack;
  PrCounter* m_best;

  //== Vector of the counters
  std::vector<PrCounter*> m_allCounters;

  std::vector<EffCounter> m_counters;
};

#endif // PRCHECKER_H
