// $Id: PatFwdTool.h,v 1.1.1.1 2007-10-09 18:23:10 smenzeme Exp $
#ifndef FWDGEOMETRYTOOL_H
#define FWDGEOMETRYTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "PatFwdTrackCandidate.h"
#include "PatFwdPlaneCounter.h"
#include "PatFwdRegionCounter.h"
#include "PatKernel/PatForwardHit.h"

static const InterfaceID IID_PatFwdTool ( "PatFwdTool", 1, 0 );

  /** @class PatFwdTool PatFwdTool.h
   *  This tool holds the parameters to extrapolate tracks
   *  in the field.
   *
   *  @author Olivier Callot
   *  @date   2005-04-01 Initial version
   *  @date   2007-08-20 Update for A-Team tracking framework
   */

  class PatFwdTool : public GaudiTool {
  public:

    // Return the interface ID
    static const InterfaceID& interfaceID() { return IID_PatFwdTool; }

    /// Standard constructor
    PatFwdTool( const std::string& type,
                const std::string& name,
                const IInterface* parent);

    virtual ~PatFwdTool( ); ///< Destructor

    virtual StatusCode initialize();


    double xAtReferencePlane( PatFwdTrackCandidate& track, PatFwdHit* hit, bool store=false );


    double zReference() const { return m_zReference; }

    double zAfter()     const { return m_zAfter; }

    bool fitXCandidate( PatFwdTrackCandidate& track,
                        double maxChi2, int minPlanes );

    bool fitStereoCandidate( PatFwdTrackCandidate& track,
                             double maxChi2, int minPlanes );

    void fitXProjection ( PatFwdTrackCandidate& track,
                          PatFwdHits::iterator itBeg,
                          PatFwdHits::iterator itEnd,
                          bool onlyXPlanes  );

    void setRlDefault ( PatFwdTrackCandidate& track,
                        PatFwdHits::iterator itBeg,
                        PatFwdHits::iterator itEnd );

    void updateHitsForTrack ( PatFwdTrackCandidate& track,
                              PatFwdHits::iterator itBeg,
                              PatFwdHits::iterator itEnd );

    double distanceForFit( PatFwdTrackCandidate& track,  PatFwdHit* hit) {
      double dist =  distanceHitToTrack( track, hit );
      if ( hit->hit()->region() > 1)  return dist;
      return dist / track.cosAfter();
    }

    double distanceHitToTrack( PatFwdTrackCandidate& track,  PatFwdHit* hit) {
      double dz   = hit->z() - m_zReference;
      double dist = hit->x() - track.x( dz );
      if ( hit->hit()->region() > 1 ) return dist;

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

    double chi2Hit( PatFwdTrackCandidate& track,  PatFwdHit* hit) {
      double dist = distanceHitToTrack( track, hit );
      return dist * dist * hit->hit()->weight();
    }

    double distAtMagnetCenter( PatFwdTrackCandidate& track ) {
      double dz   = m_zMagnet - m_zReference;
      double dist = track.xStraight( m_zMagnet ) - track.xMagnet( dz );
      return dist;
    }

    double chi2PerDoF(  PatFwdTrackCandidate& track );

    bool removeYIncompatible(  PatFwdTrackCandidate& track, double tol, int minPlanes ) {
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

      PatFwdPlaneCounter<PatForwardHit> planeCount( track.coordBegin(), track.coordEnd() );
      if ( minPlanes > planeCount.nbDifferent() ) return false;
      return true;
    }

    bool filterOT(   PatFwdTrackCandidate& track, int minCoord ) {
      PatFwdRegionCounter regions( track.coordBegin(), track.coordEnd() );
      int nbIT =  regions.nbInRegion( 2 ) +  regions.nbInRegion( 3 ) +
        regions.nbInRegion( 4 ) +  regions.nbInRegion( 5 ) ;
      if ( 1 < nbIT ) return false;
      if ( regions.nbInRegion( 0 ) + regions.nbInRegion( 1 ) < minCoord ) return true;
      return false;
    }


    double changeInY(  PatFwdTrackCandidate& track) {
      double yOriginal = track.yStraight( m_zReference );
      if (!m_withoutBField)
        yOriginal += track.dSlope() * track.dSlope() * track.slY() * m_yParams[0];
      return yOriginal - track.y( 0. );
    }

    double qOverP( PatFwdTrackCandidate& track );

  protected:

  private:
    std::vector<double> m_zMagnetParams;
    std::vector<double> m_xParams;
    std::vector<double> m_yParams;
    std::vector<double> m_momentumParams;
    double m_zReference;
    double m_zAfter;
    double m_xMagnetTol;
    double m_xMagnetTolSlope;
    double m_zMagnet;

    bool m_withoutBField;

  };
#endif // FWDGEOMETRYTOOL_H
