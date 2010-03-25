// $Id: PatVeloPixFitLHCbIDs.h,v 1.4(??) 2010/03/10 16:25:59(??) lcarson Exp(??) $
#ifndef TF_PATVELOPIXFITLHCBIDS_H 
#define TF_PATVELOPIXFITLHCBIDS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "TrackInterfaces/ITrackFitter.h"
#include "TrackInterfaces/IVeloPixClusterPosition.h"
#include "VeloPixDet/DeVeloPix.h"
#include "Event/VeloPixLiteCluster.h"

namespace LHCb {
  class Track;
}

// Forward declarations
class DeVeloPix;
class VeloPixLiteCluster;

namespace Tf {
   
  // updated interface ID to 2.0 as part of the Tf update
  static const InterfaceID IID_PatVeloPixFitLHCbIDs ("Tf::PatVeloPixFitLHCbIDs",1,0);

  /** @class PatVeloPixFitLHCbIDs PatVeloPixFitLHCbIDs.h
   *  Tool to take a list of LHCbIDs from VeloPix and fit a track as per PatVelo
   *
   *  @author Laurence Carson and Victor Coco
   *  @date   2010-03-10
   */
  class PatVeloPixFitLHCbIDs : public GaudiTool, 
			    virtual public ITrackFitter {
  public: 

    // Return the interface ID
    static const InterfaceID& interfaceID() { return IID_PatVeloPixFitLHCbIDs; }

    /// Standard constructor
    PatVeloPixFitLHCbIDs( const std::string& type, 
		      const std::string& name,
		      const IInterface* parent);

    virtual ~PatVeloPixFitLHCbIDs( ); ///< Destructor

    StatusCode initialize(); ///< initialize

    /// fit the track using the LHCbIDs on it with the PatVelo internal fit
    virtual StatusCode fit ( LHCb::Track & track,
                             LHCb::ParticleID pid = LHCb::ParticleID(211) );

  private:

    double m_s0;     ///< sum of weight of clusters
    double m_s02;    ///< sum of square weight of clusters
    double m_sx;     ///< sum of ( weight of clusters * x )
    double m_sy;     ///< sum of ( weight of clusters * y )
    double m_sz;     ///< sum of ( weight of clusters * z )
    double m_sxz;    ///< sum of ( weight of clusters * x * z )
    double m_syz;    ///< sum of ( weight of clusters * y * z )
    double m_sx2;    ///< sum of ( weight of clusters * x**2 )
    double m_sy2;    ///< sum of ( weight of clusters * y**2 )
    double m_sz2;    ///< sum of ( weight of clusters * z**2 )

    double m_slopeX;  ///< slope in x
    double m_slopeY;  ///< slope in y
    double m_interceptX;   ///< x at z = 0
    double m_interceptY;   ///< y at z = 0
    double m_meanZ;  ///< average of z measurements
    double m_errorSlopX; ///< error on X slope
    double m_errorSlopY; ///< error on Y slope

    /// detector element
    DeVeloPix* m_veloPix;

    LHCb::VeloPixLiteCluster::VeloPixLiteClusters* m_clusters;
    IVeloPixClusterPosition* m_positiontool;

     };
}
#endif // TF_PATVELOPIXFITLHCBIDS_H
