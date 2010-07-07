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

    double m_sa;     ///< sum of weights for x
    double m_sb;     ///< sum of weights for y

    double m_s02;    ///< sum of square weight of clusters

    double m_sax;     ///< sum of ( weights for x * x )
    double m_sby;     ///< sum of ( weights for y * y )

    double m_saz;     ///< sum of ( weights for x * z )
    double m_sbz;     ///< sum of ( weights for y * z )

    double m_saxz;    ///< sum of ( weights for x * x * z )
    double m_sbyz;    ///< sum of ( weights for y * y * z )

    double m_sax2;    ///< sum of ( weights for x * x**2 )
    double m_sby2;    ///< sum of ( weights for y * y**2 )

    double m_saz2;    ///< sum of ( weights for x * z**2 )
    double m_sbz2;    ///< sum of ( weights for x * z**2 )

    double m_slopeX;  ///< slope in x
    double m_slopeY;  ///< slope in y
    double m_interceptX;   ///< x at z = 0
    double m_interceptY;   ///< y at z = 0
    double m_meanZ_a;  ///< average of z measurements using x weights
    double m_meanZ_b;  ///< average of z measurements using y weights
    double m_errorSlopX; ///< error on X slope
    double m_errorSlopY; ///< error on Y slope

    /// detector element
    DeVeloPix* m_veloPix;

    /// parameter to take MS into account
    double m_stepError;

    LHCb::VeloPixLiteCluster::VeloPixLiteClusters* m_clusters;
    IVeloPixClusterPosition* m_positiontool;

     };
}
#endif // TF_PATVELOPIXFITLHCBIDS_H
