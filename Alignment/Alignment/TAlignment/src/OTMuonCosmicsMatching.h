// $Id: OTMuonCosmicsMatching.h,v 1.3 2009-02-06 13:17:35 janos Exp $
#ifndef OTMUONCOSMICSMATCHING_H 
#define OTMUONCOSMICSMATCHING_H 1

// Include files
// from STD
#include <string>
#include <vector>

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

// from Muon
#include "MuonDet/DeMuonDetector.h"

// from TrackEvent
#include "Event/State.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackExtrapolator.h"

// from TrackTools
#include "TrackInterfaces/ITrackChi2Calculator.h"

namespace MatchingHelpers {
 class MatchedTrack;
}

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


protected:


private:
  StatusCode matchChi2( LHCb::State& tTrack, LHCb::State& mTrack,
                        const double& atZ,
                        double& chi2 ) ;

  void filter( const std::vector< MatchingHelpers::MatchedTrack >& matchedTracks, 
               LHCb::Tracks& output ) const;
  
  void merge( const LHCb::Track& tTrack, const LHCb::Track& muonTrack, LHCb::Track& track ) const;

  void fillNTuple( const std::vector< MatchingHelpers::MatchedTrack >& matchedTracks ) const;
    
private:

  ITrackExtrapolator*   m_extrapolator         ;
  std::string           m_nExtrapolator        ; 
  ITrackChi2Calculator* m_chi2Calculator       ;
  std::string           m_nChi2Calculator      ; 
  std::string           m_tTracksLocation      ;
  std::string           m_mTracksLocation      ;
  std::string           m_tracksOutputLocation ;
  double                m_matchAtZ             ;
  bool                  m_matchAtFirstMuonHit  ;
  double                m_matchChi2Cut         ;
  bool                  m_allCombinations      ;
  bool                  m_addMuonIDs           ;
  DeMuonDetector*       m_muonDet              ;
};

#endif // OTMUONCOSMICSMATCHING_H
