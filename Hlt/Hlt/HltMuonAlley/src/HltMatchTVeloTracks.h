// $Id: HltMatchTVeloTracks.h,v 1.1 2008-01-22 10:00:48 hernando Exp $
#ifndef HLTMUONALLEY_MATCHTVELOTRACKS_H 
#define HLTMUONALLEY_MATCHTVELOTRACKS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "HltBase/IMatchTVeloTracks.h"            // Interface
#include "HltBase/ITrackMatch.h"            // Interface
#include "HltBase/IBiFunctionTool.h"            // Interface
#include "Event/StateParameters.h"
#include "TrackInterfaces/IFastMomentumEstimate.h"




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
  StatusCode finalize();


  StatusCode match2dVelo(const LHCb::Track& veloTrack,
                         const LHCb::Track& Ttrack, 
                         double &x_dist);

  StatusCode match3dVelo(const LHCb::Track& veloTrack,
                         const LHCb::Track& Ttrack,
                         LHCb::Track& matchedTrack, 
                         double &x_dist, double &y_dist);
  
  double function(const LHCb::Track& velo, const LHCb::Track& ttrack) {
    double quality = 1e6;
    StatusCode sc = match2dVelo(velo,ttrack,quality);
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

  float m_2dx_cut;
  float m_sec_tol;
  float z_magnet_center;
  float m_x_cut;
  float m_y_cut;
  float m_space_cut;
  float m_ptkickConstant;

  IFastMomentumEstimate* m_fastPTool;

  LHCb::State* m_myState;
};
#endif // MATCHTVELOTRACKS_H
