#ifndef HLT1TRACKMONITOR_H 
#define HLT1TRACKMONITOR_H 1

#include "GaudiKernel/HistoDef.h"
#include "GaudiKernel/ObjectContainerBase.h"
#include "HltBase/HltBaseAlg.h"

#include <unordered_map>

namespace AIDA {
  class IHistorgram2D;
}

/** @class Hlt1TrackMonitor Hlt1TrackMonitor.h
 *
 *  functionality:
 *        monitor simple track quantities
 *        make histograms with Hlt diagnostics info
 *
 *  @author Sascha Stahl
 *  @date   2015-05-21
 */
class Hlt1TrackMonitor : public HltBaseAlg {
public:

  /// Standard constructor
  Hlt1TrackMonitor(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~Hlt1TrackMonitor(); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:
  
  void monitorTracks(LHCb::Track::Range tracks);
  void monitorFittedTracks(LHCb::Track::Range tracks);
  void monitorKinematics(LHCb::Track::Range tracksVelo,LHCb::Track::Range tracksForward);
  
  template < typename T > T* fetch(const std::string& location)
  {
    T* t = this->exist<T>( location ) ?  this->get<T>( location ) : 0;
    if ( t == 0 && this->msgLevel( MSG::WARNING ) ) {
      Warning( " Could not retrieve " + location, StatusCode::SUCCESS, 10 );
    }
    return t;
  }

  void monitor();

  std::string m_VeloTrackLocation;
  std::string m_VeloTTTrackLocation;
  std::string m_ForwardTrackLocation;
  std::string m_FittedTrackLocation;

  AIDA::IHistogram1D* m_VeloTrackMult;
  AIDA::IHistogram1D* m_VeloTTTrackMult;
  AIDA::IHistogram1D* m_ForwardTrackMult;
  AIDA::IHistogram1D* m_nTTHits;
  AIDA::IHistogram1D* m_nITHits;
  AIDA::IHistogram1D* m_nOTHits;
  AIDA::IHistogram1D* m_nVeloHits;
  AIDA::IHistogram1D* m_trackChi2DoF;
  AIDA::IHistogram1D* m_hitResidual;
  AIDA::IHistogram1D* m_hitResidualPull;
  AIDA::IHistogram1D* m_VeloPhi;
  AIDA::IHistogram1D* m_VeloEta;
  AIDA::IHistogram1D* m_forwardPhi;
  AIDA::IHistogram1D* m_forwardEta;
  AIDA::IHistogram1D* m_forwardPt;
  

  struct EnumClassHash{
    template <typename T>
    std::size_t operator()(T t) const{
      return static_cast<std::size_t>(t);
    }
  };

  std::unordered_map<LHCb::Measurement::Type,AIDA::IHistogram1D*,EnumClassHash> m_hitResidualPerDet;
  std::unordered_map<LHCb::Measurement::Type,AIDA::IHistogram1D*,EnumClassHash> m_hitResidualPullPerDet;

};
#endif // HLT1TRACKMONITOR_H

