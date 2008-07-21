// $Id: HltMatchTVeloTracks.h,v 1.5 2008-07-21 18:02:45 albrecht Exp $
#ifndef HLTMUONALLEY_MATCHTVELOTRACKS_H 
#define HLTMUONALLEY_MATCHTVELOTRACKS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/ITrackMatch.h"            // Interface
#include "TrackInterfaces/ITrackMomentumEstimate.h"
#include "Event/StateParameters.h"
#include "HltBase/IMatchTVeloTracks.h"            // Interface
#include "HltBase/IBiFunctionTool.h"            // Interface




/** @class HltMatchTVeloTracks HltMatchTVeloTracks.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2007-03-07
 */
class HltMatchTVeloTracks : public GaudiTool, 
                         virtual public IMatchTVeloTracks,
                         virtual public ITrackMatch,
                         virtual public ITrackBiFunctionTool
{
public: 
  /// Standard constructor
  HltMatchTVeloTracks( const std::string& type, 
                    const std::string& name,
                    const IInterface* parent);

  virtual ~HltMatchTVeloTracks( ); ///< Destructor

  StatusCode initialize();


  StatusCode match2dVelo(const LHCb::Track& veloTrack,
                         const LHCb::Track& Ttrack, 
                         double &x_dist);

  StatusCode match3dVelo(const LHCb::Track& veloTrack,
                         const LHCb::Track& Ttrack,
                         LHCb::Track& matchedTrack, 
                         double &x_dist, double &y_dist);
  
  double function(const LHCb::Track& velo, const LHCb::Track& ttrack) {
    double quality = 1e6;
    match2dVelo(velo,ttrack,quality).ignore();
    return quality;
  }
  
  StatusCode match(const LHCb::Track& velotrack,
                   const LHCb::Track& ttrack,
                   LHCb::Track& velottrack,
                   double& quality, double& quality2) {
    return match3dVelo(velotrack,ttrack,velottrack,quality,quality2);
  }
  
  double momentum(const LHCb::Track& velotrack,const LHCb::Track& Ttrack);
  
protected:

private:

  double m_2dx_cut;
  double m_sec_tol;
  double z_magnet_center;
  double m_x_cut;
  double m_y_cut;
  double m_space_cut;
  double m_ptkickConstant;

  ITrackMomentumEstimate* m_fastPTool;
  
  LHCb::State* m_myState;
};
#endif // MATCHTVELOTRACKS_H
