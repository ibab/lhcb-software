// $Id: PatVeloSpaceTrack.cpp,v 1.15 2010-02-18 14:12:06 dhcroft Exp $
// Include files

// local
#include "PatVeloSpaceTrack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatVeloSpaceTrack
//
// 2006-07-26 : David Hutchcroft
//-----------------------------------------------------------------------------

namespace Tf {
  //========================================================================
  // Standard constructor, initializes variables
  //========================================================================
  PatVeloSpaceTrack::PatVeloSpaceTrack( const PatVeloTrackTool * const trackTool )
    : m_s0(0.),
      m_sr(0.),
      m_sz(0.),
      m_srz(0.),
      m_sz2(0.),
      m_valid(true),
      m_zone(0),
      m_backward(false),
      m_pos0(0.),
      m_slope(0.),
      m_meanZ(0.),
      m_posErr(0.),
      m_slopeErr(0.),
      m_chiSqDof(0.),
      m_point(0.,0.,0.),
      m_slopeX(0.),
      m_slopeY(0.),
      m_cov(Gaudi::TrackSymMatrix()),
      m_maxRSensor(0),
      m_minRSensor(65),
      m_nVeloExpected(-1.),
      m_ancestor(0),
      m_angleUtils(-Gaudi::Units::pi,Gaudi::Units::pi),
      m_trackTool(trackTool),
      m_side(PatVeloHitSide::Unknown)
  {
    m_rCoord.clear();
    m_rCoord.reserve( 20 );
    m_phiCoord.clear();
    m_phiCoord.reserve( 20 );
  }

  /// add and R coordinate: update rz parameters
  void PatVeloSpaceTrack::addRCoord( PatVeloRHit* coord )  {
    m_rCoord.push_back( coord );
    if( static_cast<unsigned int>(coord->sensor()->sensorNumber()) > m_maxRSensor )  {
      m_maxRSensor =  coord->sensor()->sensorNumber();
    }
    if( static_cast<unsigned int>(coord->sensor()->sensorNumber()) < m_minRSensor )  {
      m_minRSensor =  coord->sensor()->sensorNumber();
    }
    // if the side was not otherwise set use first R cluster to set
    if( m_side == PatVeloHitSide::Unknown ) m_side = coord->side();

    double z = coord->z();
    double w = coord->weight();
    double r = coord->coordHalfBox();
    if( coord->side() != m_side ){
      unsigned int zone = coord->zone();
      if( coord->side() == PatVeloHitSide::Right ) zone+=4 ;
      // if track is fitted in other HB frame
      r += m_trackTool->rOffsetOtherHB( coord->sensorNumber(),
                                        m_trackTool->phiGlobalRZone(zone));
    }
    m_s0  = m_s0  + w;
    m_sr  = m_sr  + w * r;
    m_sz  = m_sz  + w * z;
    m_srz = m_srz + w * r * z;
    m_sz2 = m_sz2 + w * z * z;
  }

  /// fit the RZ part of the track
  void PatVeloSpaceTrack::fitRZ() {
    double den = ( m_sz2 * m_s0 - m_sz * m_sz );
    if ( fabs(den) > 1e-10 ) {
      m_slope = ( m_srz * m_s0  - m_sr  * m_sz ) / den;
      m_pos0  = ( m_sr  * m_sz2 - m_srz * m_sz ) / den;
      m_meanZ    = m_sz / m_s0;
      m_posErr   = 1./ m_s0;
      m_slopeErr = m_s0 / den;
    }
  }

  /// find interpolated R at z
  double PatVeloSpaceTrack::rInterpolated( double z ) {
    std::pair<const PatVeloRHit*,const PatVeloRHit*> coordP = surroundZ(m_rCoord,z);
    double zRatio = (z - coordP.first->z() ) / ( coordP.second->z()- coordP.first->z() );
    return ( zRatio * coordP.second->coordHalfBox() + (1.-zRatio) * coordP.first->coordHalfBox() );
  }

  /// if this coord is on the track set the effective phi and r of the coord
  void PatVeloSpaceTrack::setPhiInterpolated( PatVeloRHit* myCoord ) {
    // if the side was not otherwise set use first R cluster to set
    if( m_side == PatVeloHitSide::Unknown ) m_side = myCoord->side();
    double z = myCoord->z();
    std::pair<const PatVeloPhiHit*,const PatVeloPhiHit*> coordP =
      surroundZ(m_phiCoord,z);
    //== weight by the z ratio.
    double phi1 =  coordP.second->referencePhi();
    double phi2 =  coordP.first->referencePhi();
    double zRatio = (z - coordP.first->z() ) /
      ( coordP.second->z()- coordP.first->z() );
    double phi = m_angleUtils.mean(phi1,zRatio,phi2,(1.-zRatio));
    if( m_side == myCoord->side() ){
      myCoord->setRadiusAndPhi( myCoord->coordHalfBox() , phi );
    }else{
      myCoord->setRadiusAndPhi( myCoord->coordHalfBox() +
                                m_trackTool->rOffsetOtherHB(myCoord->sensorNumber(),phi), phi);
    }
  }

  //========================================================================
  // Fit the track, computes point, direction and covariance matrix.
  //========================================================================
  void PatVeloSpaceTrack::fitSpaceTrack ( double stepError,
					  bool inwardFit, bool beamState,
					  unsigned int fullErrorPoints) {
    // Note: defaults to inwardFit = true

    // if fitting toward z = 0 (inward fit) and forward track
    // OR fitting away from z =0 and backward track
    if ( ( inwardFit && ! m_backward ) || (! inwardFit && m_backward ) ) {
      std::sort( m_rCoord.begin(), m_rCoord.end(),
          PatVeloRHit::DecreasingByZ() );
      std::sort( m_phiCoord.begin(), m_phiCoord.end(),
          PatVeloPhiHit::DecreasingByZ() );
    }else{
      // reverse the sort order
      std::sort( m_rCoord.rbegin(), m_rCoord.rend(),
          PatVeloRHit::DecreasingByZ() );
      std::sort( m_phiCoord.rbegin(), m_phiCoord.rend(),
          PatVeloPhiHit::DecreasingByZ() );
    }

    //== First compute the 'plane' of the track by the average phi.
    //== Average the sine/cosine to avoid 2Pi problems...
    double cosPhi = 0.;
    double sinPhi = 0.;
    unsigned int nbMeas = 0;
    for ( std::vector<PatVeloPhiHit*>::reverse_iterator itC = m_phiCoord.rbegin() ; m_phiCoord.rend() != itC ; ++itC ) {
      cosPhi += (*itC)->cosPhi();
      sinPhi += (*itC)->sinPhi();
      ++nbMeas;
    }

    if( 1 < nbMeas ) {
      cosPhi /= nbMeas;
      sinPhi /= nbMeas;
    }
    double norm = sqrt( cosPhi*cosPhi + sinPhi * sinPhi );
    cosPhi /= norm;
    sinPhi /= norm;

    //== Now process the measurements, in this tilted frame

    // u is roughly radial in RZ plane of track
    // and v is 90deg to both u and z axis
    // forward is toward z=0, backward is away from z=0
    PatVeloSpaceTrackLocal::FrameParam u,v;

    double MSError = 0.;

    //== Phi coordinate: Use it mainly for V, i.e. angular measure.

    for ( std::vector<PatVeloPhiHit*>::reverse_iterator itC = m_phiCoord.rbegin() ; m_phiCoord.rend() != itC ; ++itC ) {
      double z     = (*itC)->z();
      double r     = (*itC)->referenceR();
      double x     = r * (*itC)->cosPhi();
      double y     = r * (*itC)->sinPhi();

      double lu     =  x * cosPhi + y * sinPhi;
      double lv     = -x * sinPhi + y * cosPhi;

      double dCos =   (*itC)->cosPhi() * cosPhi +  (*itC)->sinPhi() * sinPhi;
      double dSin = - (*itC)->cosPhi() * sinPhi +  (*itC)->sinPhi() * cosPhi;

      double variance = r * r * (*itC)->hit()->variance();

      double w = dSin * dSin / (variance + MSError);
      u.increment(w,lu,z);
      w = dCos * dCos / (variance + MSError);
      v.increment(w,lv,z);

      if( fullErrorPoints <=
	  static_cast<unsigned int>(itC - m_phiCoord.rbegin()) ){
	MSError += stepError;
      }
    }

    //== R coordinate: Use it mainly for U, radial measure

    MSError = 0.;

    for ( std::vector<PatVeloRHit*>::reverse_iterator itC = m_rCoord.rbegin() ; m_rCoord.rend() != itC ; ++itC ) {
      setPhiInterpolated( *itC );
      double z     = (*itC)->z();
      double r     = (*itC)->coordHalfBox();
      double x     = r * (*itC)->cosPhi();
      double y     = r * (*itC)->sinPhi();

      double lu     =  x * cosPhi + y * sinPhi;
      double lv     = -x * sinPhi + y * cosPhi;

      double dCos =   (*itC)->cosPhi() * cosPhi +  (*itC)->sinPhi() * sinPhi;
      double dSin = - (*itC)->cosPhi() * sinPhi +  (*itC)->sinPhi() * cosPhi;

      double variance = (*itC)->hit()->variance();

      double w = dCos * dCos / (variance + MSError);
      u.increment(w,lu,z);
      w = dSin * dSin / (variance + MSError);
      v.increment(w,lv,z);

      if( fullErrorPoints <=
	  static_cast<unsigned int>(itC - m_rCoord.rbegin()) ) {
	MSError += stepError;
      }
    }

    // fit the two frames
    u.fitLinear();
    v.fitLinear();

    double zUse; // z to evaluate track at

    if( inwardFit ) {
      if( beamState ){
	// want point of closest approach to z axis
	zUse = - ( u.x0 * u.dxdz + v.x0 * v.dxdz ) /
	  (u.dxdz * u.dxdz + v.dxdz * v.dxdz );
      }else{
	if( !m_backward ){
	  // want the first measured point
	  zUse = std::min((*m_rCoord.rbegin())->z(),
			     (*m_phiCoord.rbegin())->z());
	}else{
	  zUse = std::max((*m_rCoord.rbegin())->z(),
			     (*m_phiCoord.rbegin())->z());
	}
      }
    }else{
      if( !m_backward ){
        // want last measurement point
        zUse = std::max((*m_rCoord.rbegin())->z(),
            (*m_phiCoord.rbegin())->z());
      }else{
        zUse = std::min((*m_rCoord.rbegin())->z(),
            (*m_phiCoord.rbegin())->z());
      }
    }
    double uUse =  u.x0 + zUse * u.dxdz; // u at zUse
    double vUse =  v.x0 + zUse * v.dxdz; // v at zUse

    m_point.SetXYZ( uUse * cosPhi - vUse * sinPhi,
        uUse * sinPhi + vUse * cosPhi,
        zUse );
    m_slopeX = u.dxdz * cosPhi - v.dxdz * sinPhi;
    m_slopeY = u.dxdz * sinPhi + v.dxdz * cosPhi;

    // precalculate some constants
    double cosPhi2 = cosPhi*cosPhi;
    double sinPhi2 = sinPhi*sinPhi;
    double sinPcosP = sinPhi*cosPhi;

    //== Covariance at z = zUse
    double errU2 = u.errorX2(zUse);
    double errV2 = v.errorX2(zUse);
    m_cov(0,0) = errU2 * cosPhi2 + errV2 * sinPhi2;
    m_cov(1,0) = (errU2 + errV2) * sinPcosP;
    m_cov(1,1) = errU2 * sinPhi2 + errV2 * cosPhi2;

    double errSU =  u.errorXZ(zUse);
    double errTV =  v.errorXZ(zUse);
    m_cov(2,0) = errSU * cosPhi2 + errTV * sinPhi2;
    m_cov(2,1) = (errSU + errTV) * sinPcosP;
    m_cov(3,0) = (errSU + errTV) * sinPcosP;
    m_cov(3,1) = errSU * sinPhi2 + errTV * cosPhi2;

    double errS2 = u.errorsDxDz2();
    double errT2 = v.errorsDxDz2();
    m_cov(2,2) = errS2 * cosPhi2 + errT2 * sinPhi2;
    m_cov(3,2) = (errS2 + errT2) * sinPcosP ;
    m_cov(3,3) = errS2 * sinPhi2 + errT2 * cosPhi2;

    //=================================================
    // and now the ChiSquared of the fit
    //=================================================
    double chi2 = 0.;
    MSError = 0.;

    //== Phi coordinate:

    for ( std::vector<PatVeloPhiHit*>::reverse_iterator itC = m_phiCoord.rbegin() ; m_phiCoord.rend() != itC ; ++itC ) {
      double dz    = (*itC)->z() - m_point.z();
      double r     = (*itC)->referenceR();
      double dx    = r * (*itC)->cosPhi() - m_point.x() - m_slopeX * dz;
      double dy    = r * (*itC)->sinPhi() - m_point.y() - m_slopeY * dz;

      double du     =  dx * cosPhi + dy * sinPhi;
      double dv     = -dx * sinPhi + dy * cosPhi;

      double dCos =   (*itC)->cosPhi() * cosPhi +  (*itC)->sinPhi() * sinPhi;
      double dSin = - (*itC)->cosPhi() * sinPhi +  (*itC)->sinPhi() * cosPhi;

      double variance = r * r * (*itC)->hit()->variance();
      double w = 1./ ( variance + MSError);

      chi2 += w * dSin * dSin * du * du ;
      chi2 += w * dCos * dCos * dv * dv ;

      if( fullErrorPoints <=
	  static_cast<unsigned int>(itC - m_phiCoord.rbegin()) ) {
	MSError += stepError;
      }
    }

    //== R coordinate:

    MSError = 0.;

    for ( std::vector<PatVeloRHit*>::reverse_iterator itC = m_rCoord.rbegin() ; m_rCoord.rend() != itC ; ++itC ) {
      double dz    = (*itC)->z() - m_point.z();
      double r     = (*itC)->coordHalfBox();
      double dx    = r * (*itC)->cosPhi() - m_point.x() - m_slopeX * dz;
      double dy    = r * (*itC)->sinPhi() - m_point.y() - m_slopeY * dz;

      double du     =  dx * cosPhi + dy * sinPhi;
      double dv     = -dx * sinPhi + dy * cosPhi;

      double dCos =   (*itC)->cosPhi() * cosPhi +  (*itC)->sinPhi() * sinPhi;
      double dSin = - (*itC)->cosPhi() * sinPhi +  (*itC)->sinPhi() * cosPhi;

      double variance = (*itC)->hit()->variance();
      double w = 1./ ( variance + MSError);

      chi2 += w * dCos * dCos * du * du ;
      chi2 += w * dSin * dSin * dv * dv ;

      if( fullErrorPoints <=
	  static_cast<unsigned int>(itC - m_rCoord.rbegin()) ) {
	MSError += stepError;
      }
    }

    // number of degrees of freedom for the fit
    int nFree = m_phiCoord.size() + m_rCoord.size() - 4 ;

    m_chiSqDof = chi2/nFree;
  }

  void PatVeloSpaceTrack::tagClustersAsUsed( HitBase::EStatus status){

    for ( std::vector<PatVeloRHit*>::iterator iR = m_rCoord.begin();
	  iR != m_rCoord.end() ; ++iR ){
      (*iR)->hit()->setStatus(status,true);
    }

    for ( std::vector<PatVeloPhiHit*>::iterator iPhi = m_phiCoord.begin();
	  iPhi != m_phiCoord.end() ; ++iPhi ){
      (*iPhi)->hit()->setStatus(status,true);
    }

    for ( std::vector<PatVeloRHit*>::iterator iR = m_rCoordNoFit.begin();
	  iR != m_rCoordNoFit.end() ; ++iR ){
      (*iR)->hit()->setStatus(status,true);
    }

    for ( std::vector<PatVeloPhiHit*>::iterator iPhi = m_phiCoordNoFit.begin();
	  iPhi != m_phiCoordNoFit.end() ; ++iPhi ){
      (*iPhi)->hit()->setStatus(status,true);
    }
  }

}


