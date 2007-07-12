// $Id: MatchTVeloTracks.h,v 1.2 2007-07-12 16:56:55 asatta Exp $
#ifndef MATCHTVELOTRACKS_H 
#define MATCHTVELOTRACKS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "HltBase/IMatchTVeloTracks.h"            // Interface


/** @class MatchTVeloTracks MatchTVeloTracks.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2007-03-07
 */
class MatchTVeloTracks : public GaudiTool, virtual public IMatchTVeloTracks {
public: 
  /// Standard constructor
  MatchTVeloTracks( const std::string& type, 
                    const std::string& name,
                    const IInterface* parent);

  virtual ~MatchTVeloTracks( ); ///< Destructor

  StatusCode initialize();
  StatusCode finalize();


  StatusCode match2dVelo(LHCb::Track& veloTrack,LHCb::Track& Ttrack, float &x_dist);
  


  StatusCode match3dVelo(LHCb::Track& veloTrack,LHCb::Track& Ttrack,
                                 LHCb::Track& matchedTrack, float &x_dist, float &y_dist);
  float calcP(LHCb::Track& velotrack,LHCb::Track& Ttrack);
  
protected:

private:

  float m_2dx_cut;
  float m_sec_tol;
  float z_magnet_center;
  float m_x_cut;
  float m_y_cut;
  float m_space_cut;
  float m_ptkickConstant;

  LHCb::State* m_myState;
};
#endif // MATCHTVELOTRACKS_H
