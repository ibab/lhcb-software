// $Id: FastFwdTool.h,v 1.7 2009-11-26 18:00:48 mschille Exp $
#ifndef FASTFWDTOOL_H
#define FASTFWDTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "PatFwdTrackCandidate.h"
#include "PatFwdPlaneCounter.h"
#include "PatFwdRegionCounter.h"
#include "PatKernel/PatForwardHit.h"
#include "Kernel/ILHCbMagnetSvc.h"


static const InterfaceID IID_FastFwdTool ( "FastFwdTool", 1, 0 );

/** @class FastFwdTool FastFwdTool.h
 *  This tool holds the parameters to extrapolate tracks
 *  in the field.
 *
 *  @author Olivier Callot
 *  @date   2005-04-01 Initial version
 *  @date   2007-08-20 Update for A-Team tracking framework
 */

class FastFwdTool : public GaudiTool {
public:

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_FastFwdTool; }

  /// Standard constructor
  FastFwdTool( const std::string& type,
               const std::string& name,
               const IInterface* parent);

  virtual ~FastFwdTool( ); ///< Destructor

  virtual StatusCode initialize();

  double xAtReferencePlane( PatFwdTrackCandidate& track, PatFwdHit* hit,
                            bool store=false );

  double fastXAtReference( PatFwdTrackCandidate& track, PatFwdHit* hit )    {
    double zMagnet    = ( m_zMagnetParams[0] +
                          m_zMagnetParams[2] * track.slX2() +
                          m_zMagnetParams[3] * hit->x() * hit->x() +
                          m_zMagnetParams[4] * track.slY2() );

    double xMagnet    = track.xStraight( zMagnet);
    double zHit       = hit->z();
    double xHit       = hit->x();
    double slopeAfter = ( xHit - xMagnet ) / ( zHit - zMagnet );
    double dSlope     = slopeAfter - track.slX();
    double dSl2       = dSlope * dSlope;
    zMagnet           = zMagnet + m_zMagnetParams[1] * dSl2;
    double dz         = 1.e-3 * ( zHit - m_zReference );
    double dxCoef     = dz * dz * ( m_xParams[0] + m_xParams[1] * dz );
    xHit              = xHit - dxCoef * dSlope ;
    xMagnet           = track.xStraight( zMagnet );
    slopeAfter        = ( xHit - xMagnet ) / ( zHit - zMagnet );
    return  xMagnet + ( m_zReference - zMagnet ) * slopeAfter;
  }


  double zReference() const { return m_zReference; }

  const std::vector<double>& zOutputs() const { return m_zOutputs; }

  bool fitXCandidate( PatFwdTrackCandidate& track,
                      double maxChi2, int minPlanes, bool isDebug = false );

  bool fitStereoCandidate( PatFwdTrackCandidate& track,
                           double maxChi2, int minPlanes, bool isDebug = false ) const;

  bool fitXProjection ( PatFwdTrackCandidate& track,
                        PatFwdHits::iterator itBeg,
                        PatFwdHits::iterator itEnd,
                        bool onlyXPlanes  ) const;

  void setRlDefault ( PatFwdTrackCandidate& track,
                      PatFwdHits::iterator itBeg,
                      PatFwdHits::iterator itEnd ) const;

  void updateHitsForTrack ( const PatFwdTrackCandidate& track,
                            PatFwdHits::iterator itBeg,
                            PatFwdHits::iterator itEnd ) const;

  double distanceForFit( const PatFwdTrackCandidate& track,
                         const PatFwdHit* hit) const {
    double dist =  distanceHitToTrack( track, hit );
    if ( hit->hit()->region() > 1)  return dist;
    return dist / track.cosAfter();
  }

  double distanceHitToTrack( const PatFwdTrackCandidate& track,
                             const PatFwdHit* hit) const {
    double dz   = hit->z() - m_zReference;
    double dist = hit->x() - track.x( dz );
    if (  hit->hit()->type() == Tf::RegionID::IT ) return dist;

    //OT : distance to a circle, drift time
    dist = dist * track.cosAfter();
    double dx = hit->driftDistance();
    if ( 0 <  hit->rlAmb() ) return dist + dx;
    if ( 0 >  hit->rlAmb() ) return dist - dx;

    // Take closest distance in the rest
    if ( fabs( dist - dx ) < fabs( dist + dx ) ) {
      return dist - dx;
    }
    return dist + dx;
  }

  double xForHit( const PatFwdTrackCandidate& track,
                  const PatFwdHit* hit) const {
    double dz   = hit->z() - m_zReference;
    return track.x( dz );
  }

  double chi2Hit( const PatFwdTrackCandidate& track,
                  const PatFwdHit* hit) const {
    double dist = distanceHitToTrack( track, hit );
    return dist * dist * hit->hit()->weight();
  }

  double distAtMagnetCenter( const PatFwdTrackCandidate& track ) const {
    double dz   = m_zMagnet - m_zReference;
    double dist = track.xStraight( m_zMagnet ) - track.xMagnet( dz );
    return dist;
  }

  double dzAtMagnetCenter( ) const {
    return m_zMagnet - m_zReference;
  }

  double chi2PerDoF( PatFwdTrackCandidate& track ) const;

  bool removeYIncompatible(  PatFwdTrackCandidate& track, double tol, int minPlanes ) const {
    bool hasChanged = false;
    for ( PatFwdHits::iterator itH = track.coordBegin(); track.coordEnd() != itH ; ++itH ) {
      PatFwdHit* hit = *itH;
      if ( !hit->isSelected() ) continue;
      if ( !hit->hit()->isYCompatible( track.y( hit->z() - m_zReference ), tol ) ) {
        hit->setSelected( false );
        hasChanged = true;
      }
    }
    if ( hasChanged ) return fitStereoCandidate( track, 1000000., minPlanes );

    PatFwdPlaneCounter planeCount( track.coordBegin(), track.coordEnd() );
    if ( minPlanes > planeCount.nbDifferent() ) return false;
    return true;
  }

  bool filterOT(   PatFwdTrackCandidate& track, int minCoord ) const {
    PatFwdRegionCounter regions( track.coordBegin(), track.coordEnd() );
    int nbIT =  regions.nbInRegion( 2 ) +  regions.nbInRegion( 3 ) +
      regions.nbInRegion( 4 ) +  regions.nbInRegion( 5 ) ;
    if ( 1 < nbIT ) return false;
    if ( regions.nbInRegion( 0 ) + regions.nbInRegion( 1 ) < minCoord ) return true;
    return false;
  }


  double changeInY(  PatFwdTrackCandidate& track ) const {
    double yOriginal = track.yStraight( m_zReference );
    if (!m_withoutBField)
      yOriginal += track.dSlope() * track.dSlope() * track.slY() * m_yParams[0];
    return yOriginal - track.y( 0. );
  }

  double qOverP( const PatFwdTrackCandidate& track ) const;

protected:

private:
  ILHCbMagnetSvc*     m_magFieldSvc;
  std::vector<double> m_zMagnetParams;
  std::vector<double> m_xParams;
  std::vector<double> m_yParams;
  std::vector<double> m_momentumParams;
  double m_zReference;
  std::vector<double> m_zOutputs;
  double m_xMagnetTol;
  double m_xMagnetTolSlope;
  double m_zMagnet;

  bool m_withoutBField;

};

#endif // FASTFWDTOOL_H
