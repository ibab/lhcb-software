// $Id: PatAddTTCoord.h,v 1.2 2009-05-12 11:02:03 smenzeme Exp $
#ifndef PATADDTTCOORD_H
#define PATADDTTCOORD_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Event/Track.h"
#include "Event/State.h"
#include "Event/STCluster.h"
#include "TrackInterfaces/IAddTTClusterTool.h"            // Interface
#include "Kernel/ILHCbMagnetSvc.h"


#include "TfKernel/TTStationHitManager.h"
#include "PatKernel/PatTTHit.h"

  /** @class PatAddTTCoord PatAddTTCoord.h
   *
   *  Adds TT clusters to tracks for Pat algorithms.
   *  
   *  @author Olivier Callot
   *  @date   2006-06-29 Initial version
   *  @date   2007-08-20 Update for A-Team framework
   */

  class PatAddTTCoord : public GaudiTool, virtual public IAddTTClusterTool {
  public:
    /// Standard constructor
    PatAddTTCoord( const std::string& type,
                   const std::string& name,
                   const IInterface* parent);

    virtual ~PatAddTTCoord( ); ///< Destructor

    /// Add TT clusters to matched tracks
    virtual StatusCode addTTClusters( LHCb::Track& track );

    /// Add TT clusters to matched tracks
    virtual StatusCode addTTClusters( LHCb::Track& track,
                                      std::vector<LHCb::STCluster*>& ttClusters,
                                      std::vector<double>& ttChi2s );

    /// Get the distance to the strip
    virtual double distanceToStrip( const LHCb::Track& track,
                                    const LHCb::STCluster& ttCluster );

    virtual StatusCode initialize();

  private:

    Tf::TTStationHitManager<PatTTHit> * m_ttHitManager;

    double m_ttParam;
    double m_ttTol;
    double m_ttTolSlope;
    double m_zTTField;
    double m_zTTProj;
    double m_ttMaxChi2; 
    ILHCbMagnetSvc*     m_magFieldSvc; 

  };

#endif // PATADDTTCOORD_H
