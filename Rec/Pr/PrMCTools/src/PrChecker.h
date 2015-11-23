// $Id: $

#ifndef PRCHECKER_H
#define PRCHECKER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"


// local
//#include "PrCounter.h"
#include "PrKernel/IPrCounter.h"

class IHistoTool ;


/** @class PrChecker PrChecker.h
 *  Check the quality of the pattern recognition, by comparing to MC information
 *  Produces efficiency, ghost rate and clone rate numbers.
 *
 *  The output looks like:
 *
 *  prChecker.TrackType      INFO **** TrackType                        "nRecod tracks" tracks including                                                  "nGhosts" ghosts ["ghost rate" %], Event average   "ghost rate" % ****
 *  prChecker.TrackType      INFO                             type :    "nRecod & rectible tracks" from     "nRectible tracks" ["track efficiency" %]     "nClones" clones [ "cloneRate" %], purity: abc %, hitEff:  xzy %
 *
 *  with:
 *  - nRecod tracks: number of reconstructed tracks
 *  - nGhosts: number of reconstructed ghost tracks (tracks not matched to MC truth)
 *  - ghoste rate = nGhosts/nRecod (event average: average ghost rate per event, insensitive to busy or empty events)
 *  - nRecod & rectible tracks: number of reconstructed tracks that are considered reconstructible i.e. can be matched to MC truth, and left enough clusters in the detector to be considered reconstructible.
 *  - nRectible tracks: charged MCParticles that left enough clusters in the detector to be considered reconstructible.
 *  - efficiency: nRecod & rectible tracks / nRectible tracks
 *  - nClones: Number of tracks that share link to MCParticle
 *  - cloneRate: nClones / (nRecod & rectible tracks + nClones)
 *  - purity: Fraction of hits on the track belonging to the matched MCParticle
 *  - hitEff: Fraction of hits on the track of all hits belonging to the matched MCParticle
 * 
 *  Parameters:
 *   - Eta25Cut: Only consider particles with 2 < eta < 5? (default: false)
 *   - TriggerNumbers: Give numbers for p > 3GeV, pT > 500 MeV? (default: false)
 *   - UseElectrons: Take electrons into account in numbers? (default: false)
 * 
 *  @author Olivier Callot, Thomas Nikodem
 *  @date   2014-02-13
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
  std::string m_bestTracks;
  
  IPrCounter* m_velo;
  IPrCounter* m_forward;
  IPrCounter* m_match;
  IPrCounter* m_upTrack;
  IPrCounter* m_tTrack;
  IPrCounter* m_downTrack;
  IPrCounter* m_best;
  IPrCounter* m_bestLong;
  IPrCounter* m_bestDownstream;

  //IPrCounter* m_utForward;
  //IPrCounter* m_utMatch;
  //IPrCounter* m_utbestLong;
  //IPrCounter* m_utDownst;

  int  m_writeVeloHistos;      
  int  m_writeForwardHistos;   
  int  m_writeMatchHistos;     
  int  m_writeDownHistos;      
  int  m_writeUpHistos;        
  int  m_writeTTrackHistos;    
  int  m_writeBestHistos;
  int  m_writeBestLongHistos;
  int  m_writeBestDownstreamHistos;
  int  m_writeUTHistos;

  bool m_eta25cut;             
  bool m_triggerNumbers;
  bool m_useElectrons;
  
  double m_ghostProbCut;

  //== Vector of the counters
  std::vector<IPrCounter*> m_allCounters;
  //std::vector<IPrUTCounter*> m_allUTCounters;

  const IHistoTool* m_histoTool;

};

#endif // PRCHECKER_H
