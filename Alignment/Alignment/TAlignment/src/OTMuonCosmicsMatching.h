// $Id: OTMuonCosmicsMatching.h,v 1.2 2008-11-24 10:01:30 janos Exp $
#ifndef OTMUONCOSMICSMATCHING_H 
#define OTMUONCOSMICSMATCHING_H 1

// Include files
// from STD
#include <string>
#include <vector>

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h" 

// from TrackEvent
#include "Event/State.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackExtrapolator.h"

// from TrackTools
#include "TrackInterfaces/ITrackChi2Calculator.h"

namespace LHCb {
 class Track;
}

/** @class OTMuonCosmicsMatching OTMuonCosmicsMatching.h
 *  
 *
 *  @author Jan Amoraal
 *  @date   2008-10-28
 */

class OTMuonCosmicsMatching : public GaudiTupleAlg {

public:
  /// Standard constructor
  OTMuonCosmicsMatching( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~OTMuonCosmicsMatching( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  //virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  StatusCode matchChi2( const LHCb::Track& tTrack, const LHCb::Track& mTrack,
                        const double& atZ,
                        double& chi2 ) ;

private:

  ITrackExtrapolator*   m_extrapolator         ;
  std::string           m_nExtrapolator        ; 
  ITrackChi2Calculator* m_chi2Calculator       ;
  std::string           m_nChi2Calculator      ; 
  std::string           m_tTracksLocation      ;
  std::string           m_mTracksLocation      ;
  std::string           m_tracksOutputLocation ;
  double                m_matchAtZ             ;
  double                m_matchChi2Cut         ;
  bool                  m_allCombinations      ;

  /// Tuple stuff
  std::vector< LHCb::State > m_matchTStates    ;
  std::vector< LHCb::State > m_matchMuonStates ;

  //   std::vector< double > m_matchTx              ;
  //   std::vector< double > m_matchTxErr           ;
  //   std::vector< double > m_matchTtx             ;
  //   std::vector< double > m_matchTtxErr          ;
  //   std::vector< double > m_matchTy              ;
  //   std::vector< double > m_matchTyErr           ;
  //   std::vector< double > m_matchTty             ;
  //   std::vector< double > m_matchTtyErr          ;
  //   /// M match states
  //   std::vector< double > m_matchMx              ;
  //   std::vector< double > m_matchMxErr           ;
  //   std::vector< double > m_matchMtx             ;
  //   std::vector< double > m_matchMtxErr          ;
  //   std::vector< double > m_matchMy              ;
  //   std::vector< double > m_matchMyErr           ;
  //   std::vector< double > m_matchMty             ;
  //   std::vector< double > m_matchMtyErr          ;
  
};

#endif // OTMUONCOSMICSMATCHING_H
