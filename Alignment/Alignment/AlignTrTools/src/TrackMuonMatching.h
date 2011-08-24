// $Id: TrackMuonMatching.h,v 1.1 2010-06-04 13:02:57 svecchi Exp $
#ifndef TRACKMUONMATCHING_H 
#define TRACKMUONMATCHING_H 1

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

#include "MuonDet/DeMuonDetector.h"

namespace LHCb {
 class Track;
}

/** @class TrackMuonMatching TrackMuonMatching.h
 *  
 *
 *  @author Stefania Vecchi
 *  @date   2010-06-04
 */

class TrackMuonMatching : public GaudiTupleAlg {

public:
  /// Standard constructor
  TrackMuonMatching( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrackMuonMatching( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  //virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  StatusCode matchChi2( LHCb::State& tTrack, LHCb::State& mTrack,
                        const double& atZ,
                        double& chi2 ) ;
  StatusCode OTmuonExtrap( LHCb::State* tState, const double& atZ);
  
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
  bool                  m_matchAtOT            ;
  double                m_matchChi2Cut         ;
  bool                  m_allCombinations      ;
  DeMuonDetector*       m_muonDet              ; 
  bool                  m_ntuple               ;

  static const  int     nSTATIONS=5;  
  double                m_zMS[5];
  
};

#endif // TRACKMUONMATCHING_H
