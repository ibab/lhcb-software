// $Id$
#ifndef PATFWDTOOL_H
#define PATFWDTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "PatFwdTrackCandidate.h"
#include "PatFwdPlaneCounter.h"
#include "PatFwdRegionCounter.h"

#include "PatKernel/PatForwardHit.h"
#include "Kernel/ILHCbMagnetSvc.h"

// icc13 onwards should be fine actually, using the gcc extensions...
#ifndef __INTEL_COMPILER
#define PATFWDTOOL_VEC 1
#endif

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

  ~PatFwdTool( ) override = default;
  StatusCode initialize() override;

private:
  double  updateTrackAndComputeZMagnet( PatFwdTrackCandidate& track, const PatFwdHit* hit ) const;
  template <bool withoutBField, typename... Hits>
  void setXAtReferencePlane( const PatFwdTrackCandidate& track, double zMagnet, Hits... hits ) const;

public:

  double zReference() const { return m_zReference; }

  const std::vector<double>& zOutputs() const { return m_zOutputs; }

  bool fitXCandidate( PatFwdTrackCandidate& track, double maxChi2, int minPlanes ); // FIXME: for now, this updates m_zMagnet

  bool fitStereoCandidate( PatFwdTrackCandidate& track, double maxChi2, int minPlanes ) const;
private:
  template <bool withoutBField>
  bool fitXProjection_ ( PatFwdTrackCandidate& track,
                        PatFwdHits::const_iterator itBeg,
                        PatFwdHits::const_iterator itEnd,
                        bool onlyXPlanes  ) const;
  bool fitXProjection ( PatFwdTrackCandidate& track,
                        PatFwdHits::const_iterator itBeg,
                        PatFwdHits::const_iterator itEnd,
                        bool onlyXPlanes  ) const {
  return LIKELY(!m_withoutBField) ? fitXProjection_<false>( track, itBeg, itEnd, onlyXPlanes )
                                  : fitXProjection_<true> ( track, itBeg, itEnd, onlyXPlanes );
  }
  bool fitYProjection( PatFwdTrackCandidate& track, 
                       PatFwdHits::const_iterator itBeg,
                       PatFwdHits::const_iterator itEnd ) const ;

public:
  void setRlDefault ( PatFwdTrackCandidate& track,
                      PatFwdHits::const_iterator itBeg,
                      PatFwdHits::const_iterator itEnd ) const;

  template <typename Iterator1,typename Iterator2>
  void setXAtReferencePlane( const PatFwdTrackCandidate& track, Iterator1&& begin, Iterator2&& end) const {
      if (LIKELY(!m_withoutBField)) {
          auto z_Magnet = zMagnet( track );
#ifdef  PATFWDTOOL_VEC
          using iter_t = typename std::decay<Iterator1>::type;
          iter_t i = std::forward<Iterator1>(begin);
          for ( ; std::distance( i, end ) % 2 !=0 ; ++i ) {
                setXAtReferencePlane<false>( track, z_Magnet, i[0]);
          }
          for ( ; i!=end ; i+=2 ) {
                setXAtReferencePlane<false>( track, z_Magnet, i[0], i[1] );
          }
#else
          std::for_each( std::forward<Iterator1>(begin), 
                         std::forward<Iterator2>(end), 
                         [&,z_Magnet](PatFwdHit* hit) { setXAtReferencePlane<false>(track,z_Magnet,hit); } );
#endif
      } else {
          std::for_each( std::forward<Iterator1>(begin), 
                         std::forward<Iterator2>(end), 
                         [&](PatFwdHit* hit) { setXAtReferencePlane<true>(track,0.0,hit ) ; } );
      }
  }

  template <typename T> T driftResidual( int amb, T dist, T dx ) const {
    // Take closest distance if amb == 0
    auto smallest = [](T a, T b) { return std::abs(a)<std::abs(b) ? a : b ; };
    return amb == 0 ? smallest( dist+dx, dist-dx ) : ( dist + amb * dx );
  }

  double distanceForXFit( const PatFwdTrackCandidate& track, const PatFwdHit* hit, double ica) const {
    auto dist = hit->x() - track.x( hit->z() - zReference() );
    return hit->isOT() ? driftResidual( hit->rlAmb(), dist, ica*hit->driftDistance() ) : dist;
  }

  double distanceForYFit( const PatFwdTrackCandidate& track, const PatFwdHit* hit, double ica) const {
    auto dist =  distanceHitToTrack( track, hit );
    if ( hit->isOT() ) dist *= ica ;
    return - dist / hit->hit()->dxDy();
  }

  double chi2Hit( const PatFwdTrackCandidate& track, const PatFwdHit* hit) const {
    auto dist = distanceHitToTrack( track, hit );
    return dist * dist * hit->hit()->weight();
  }

  double distanceHitToTrack( const PatFwdTrackCandidate& track, const PatFwdHit* hit) const {
    auto dist = hit->x() - track.x( hit->z() - zReference() );
    return hit->isOT() ? driftResidual( hit->rlAmb(), dist*track.cosAfter(), hit->driftDistance() ) : dist ;
  }

  double chi2Magnet( const PatFwdTrackCandidate& track) const {
      //== Error component due to the contraint of the magnet centre
      double dist      = distAtMagnetCenter( track );
      double errCenter = m_xMagnetTol + track.dSlope() * track.dSlope() * m_xMagnetTolSlope;
      return  dist * dist / errCenter;
  }

  double distAtMagnetCenter( const PatFwdTrackCandidate& track ) const {
    auto dz = m_zMagnet - zReference();
    return track.xStraight( m_zMagnet ) - track.xMagnet( dz );
  }

  double chi2PerDoF( PatFwdTrackCandidate& track ) const;

  bool removeYIncompatible(PatFwdTrackCandidate& track, double tol, int minPlanes ) const {
    bool hasChanged = false;
    for ( PatFwdHit* hit : track.coords() ) {
      if ( !hit->isSelected() ) continue;
      if ( !hit->hit()->isYCompatible( track.y( hit->z() - zReference() ), tol ) ) {
        hit->setSelected( false );
        hasChanged = true;
      }
    }
    if ( hasChanged ) return fitStereoCandidate( track, 1000000., minPlanes );

    PatFwdPlaneCounter planeCount( track.coordBegin(), track.coordEnd() );
    return planeCount.nbDifferent() >= minPlanes;
  }

  double changeInY(const  PatFwdTrackCandidate& track ) const {
    double yOriginal = track.yStraight( zReference() );
    if (LIKELY(!m_withoutBField))
      yOriginal += track.dSlope() * track.dSlope() * track.slY() * m_yParams[0];
    return yOriginal - track.y( 0. );
  }

  double qOverP( const PatFwdTrackCandidate& track ) const;
//=========================================================================
//  Returns center of magnet for velo track
//=========================================================================
  double zMagnet( const PatFwdTrackCandidate& track ) const
{
  //== correction behind magnet neglected
  return  ( m_zMagnetParams[0] +
            m_zMagnetParams[2] * track.slX2() +
            m_zMagnetParams[4] * track.slY2() );
}
  double magscalefactor() const { return m_magFieldSvc->signedRelativeCurrent();} ;

private:
  ILHCbMagnetSvc*     m_magFieldSvc; 
  std::vector<double> m_zMagnetParams;
  std::vector<double> m_xParams;
  std::vector<double> m_yParams;
  std::vector<double> m_momentumParams;
  double m_zReference; // TODO: move this into PatFwdTrackCandidate, make all methods 'const'
  std::vector<double> m_zOutputs;
  double m_xMagnetTol;
  double m_xMagnetTolSlope;
  double m_zMagnet;

  bool m_withoutBField;
  bool m_ambiguitiesFromPitchResiduals;
};

#endif // PATFWDTOOL_H
