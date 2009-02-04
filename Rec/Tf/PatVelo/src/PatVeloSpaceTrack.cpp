// $Id: PatVeloSpaceTrack.cpp,v 1.6 2009-02-04 11:42:35 dhcroft Exp $
// Include files 

// local
#include "PatVeloSpaceTrack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatVeloSpaceTrack
//
// 2006-07-26 : David Hutchcroft
//-----------------------------------------------------------------------------

namespace Tf {
  //=============================================================================
  // Standard constructor, initializes variables
  //=============================================================================
  PatVeloSpaceTrack::PatVeloSpaceTrack(  ) 
    : m_s0(0.)
    , m_sr(0.)
    , m_sz(0.)
    , m_srz(0.)
    , m_sz2(0.)
    , m_valid(true)
    , m_zone(0)
    , m_backward(false)
    , m_cov( )
    , m_maxRSensor(0)
    , m_minRSensor(65)
    , m_ancestor(0)
    , m_angleUtils(-Gaudi::Units::pi,Gaudi::Units::pi)
  {
    m_rCoord.reserve( 20 );
    m_phiCoord.reserve( 20 );
  }

  PatVeloSpaceTrack::PatVeloSpaceTrack( const PatVeloSpaceTrack & track ) 
    : m_s0(track.m_s0),
      m_sr(track.m_sr),
      m_sz(track.m_sz),
      m_srz(track.m_srz),
      m_srz2(track.m_srz2),
      m_sz2(track.m_sz2),
      m_valid(track.m_valid),
      m_zone(track.m_zone),
      m_xZone(track.m_xZone),
      m_negZone(track.m_negZone),
      m_backward(track.m_backward),
      m_pos0(track.m_pos0),
      m_slope(track.m_slope),
      m_firstZ(track.m_firstZ),
      m_meanZ(track.m_meanZ),
      m_posErr(track.m_posErr),
      m_slopeErr(track.m_slopeErr),
      m_chiSqDof(track.m_chiSqDof),
      m_rCoord(track.m_rCoord),
      m_phiCoord(track.m_phiCoord),
      m_point(track.m_point),
      m_slopeX(track.m_slopeX),
      m_slopeY(track.m_slopeY),
      m_cov(track.m_cov),
      m_maxRSensor(track.m_maxRSensor),
      m_minRSensor(track.m_minRSensor),
      m_ancestor(track.m_ancestor),
      m_angleUtils(track.m_angleUtils)
  {};


  /// add and R coordinate: update rz parameters
  void PatVeloSpaceTrack::addRCoord( PatVeloRHit* coord )  {  
    m_rCoord.push_back( coord );
    if( static_cast<unsigned int>(coord->sensor()->sensorNumber()) > m_maxRSensor )  {
      m_maxRSensor =  coord->sensor()->sensorNumber();
    }
    if( static_cast<unsigned int>(coord->sensor()->sensorNumber()) < m_minRSensor )  {
      m_minRSensor =  coord->sensor()->sensorNumber();
    }
    double z = coord->z();
    double w = coord->weight();
    double r = coord->coordHalfBox();

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
  void PatVeloSpaceTrack::setPhiInterpolated( PatVeloRHit* myCoord  ) {
    double z = myCoord->z();
    std::pair<const PatVeloPhiHit*,const PatVeloPhiHit*> coordP = 
      surroundZ(m_phiCoord,z);
    //== weight by the z ratio. 
    double zRatio = (z - coordP.first->z() ) / 
      ( coordP.second->z()- coordP.first->z() );
    double phi1 =  coordP.second->referencePhi();
    double phi2 =  coordP.first->referencePhi();

    double phi = m_angleUtils.mean(phi1,zRatio,phi2,(1.-zRatio));
    
    //Handle the 2.pi modulation.  
    //if ( phi1 < phi2 - Gaudi::Units::pi ) phi1 += 2*Gaudi::Units::pi;
    //if ( phi1 > phi2 + Gaudi::Units::pi ) phi1 -= 2*Gaudi::Units::pi;
    //double phi = ( zRatio * phi1 + (1.-zRatio) * phi2 );
    //if      ( phi >  Gaudi::Units::pi ) phi -= 2*Gaudi::Units::pi;
    //else if ( phi < -Gaudi::Units::pi ) phi += 2*Gaudi::Units::pi;

    myCoord->setRadiusAndPhi( myCoord->coordHalfBox(), phi );
  }

  //========================================================================
  // Fit the track, computes point, direction and covariance matrix.
  //========================================================================
  void PatVeloSpaceTrack::fitSpaceTrack ( double stepError, 
					  bool inwardFit, bool beamState ) {
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
    int nbMeas = 0;
    for ( std::vector<PatVeloPhiHit*>::reverse_iterator itC = m_phiCoord.rbegin() ; m_phiCoord.rend() != itC ; ++itC ) {
      cosPhi += (*itC)->cosPhi();
      sinPhi += (*itC)->sinPhi();
      ++nbMeas;
    }

    cosPhi /= nbMeas;
    sinPhi /= nbMeas;
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

      MSError += stepError;
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

      MSError += stepError;
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
	  zUse = GSL_MIN_DBL((*m_rCoord.rbegin())->z(),
			     (*m_phiCoord.rbegin())->z());
	}else{
	  zUse = GSL_MAX_DBL((*m_rCoord.rbegin())->z(),
			     (*m_phiCoord.rbegin())->z());
	}
      }
    }else{
      if( !m_backward ){
        // want last measurement point
        zUse = GSL_MAX_DBL((*m_rCoord.rbegin())->z(),
            (*m_phiCoord.rbegin())->z());
      }else{
        zUse = GSL_MIN_DBL((*m_rCoord.rbegin())->z(),
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

      MSError += stepError;
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

      MSError += stepError;
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
  }

}

  
