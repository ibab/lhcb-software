// $Id: $
#ifndef PRCHECKER_H
#define PRCHECKER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"


// local
#include "PrCounter.h"


class IHistoTool ;


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



class PrChecker : public GaudiHistoAlg {
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
  std::string m_matchTracks;
  std::string m_seedTracks;
  std::string m_downTracks;
  std::string m_upTracks;
  
  PrCounter* m_velo;
  PrCounter* m_forward;
  PrCounter* m_match;
  PrCounter* m_upTrack;
  PrCounter* m_tTrack;
  PrCounter* m_downTrack;
  PrCounter* m_best;
  PrCounter* m_bestLong;
  PrCounter* m_bestDownstream;


  int  m_writeVeloHistos;      
  int  m_writeForwardHistos;   
  int  m_writeMatchHistos;     
  int  m_writeDownHistos;      
  int  m_writeUpHistos;        
  int  m_writeTTrackHistos;    
  int  m_writeBestHistos;      

  bool m_eta25cut;             

  //== Vector of the counters
  std::vector<PrCounter*> m_allCounters;

  const IHistoTool* m_histoTool;
};

#endif // PRCHECKER_H
