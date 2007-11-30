// $Id: PatRZTrack.cpp,v 1.5 2007-11-30 09:09:19 dhcroft Exp $
// Include files 

// local
#include "PatRZTrack.h"

namespace Tf {

  //--------------------------------------------------------------------------
  // Implementation file for class : PatRZTrack
  //
  // 2006-09-20 : David Hutchcroft: from PatRZTrack.h
  //--------------------------------------------------------------------------

  //==========================================================================
  // Standard constructor, initializes variables
  //==========================================================================

  PatRZTrack::PatRZTrack( ) :
    m_s0(0.),
    m_sr(0.),
    m_sz(0.),
    m_srz(0.),
    m_sr2(0.),
    m_sz2(0.),
    m_valid(true),
    m_zone(0),  
    m_backward(false),
    m_missedStations(false),
    m_minSensor( 100 ),
    m_maxSensor( 0 ),
    m_pos0(-999.),
    m_slope(-999.)  
  {
    m_coord.reserve( 20 );
  }

  void PatRZTrack::addRCoord( VeloRHit* coord )  { 
    if ( coord->sensor()->sensorNumber() > m_maxSensor ) m_maxSensor = coord->sensor()->sensorNumber();
    if ( coord->sensor()->sensorNumber() < m_minSensor ) m_minSensor = coord->sensor()->sensorNumber();
    m_coord.push_back( coord ); 
    double z = coord->z();
    double w = coord->weight(); 
    double r = coord->coordHalfBox();

    m_s0  = m_s0  + w;
    m_sr  = m_sr  + w * r;
    m_sz  = m_sz  + w * z;
    m_srz = m_srz + w * r * z;
    m_sr2 = m_sr2 + w * r * r;
    m_sz2 = m_sz2 + w * z * z;

    if( m_coord.size() > 2 ){
      double den = ( m_sz2 * m_s0 - m_sz * m_sz );
      if ( fabs(den) < 10e-10 ) den = 1.;
      m_slope = ( m_srz * m_s0  - m_sr  * m_sz ) / den;
      m_pos0  = ( m_sr  * m_sz2 - m_srz * m_sz ) / den;
      m_slopeErr = m_s0 / den;
    }
  }

  double PatRZTrack::chi2() const {
    double chi2=0;
    for(VeloRHits::const_iterator i= m_coord.begin() ; i < m_coord.end(); ++i){
      chi2 += gsl_pow_2((*i)->coordHalfBox() - (*i)->z()*m_slope - m_pos0)*
	(*i)->weight();
    }
    return chi2;
  }

  void PatRZTrack::tagUsedCoords()  {
    for ( VeloRHits::iterator itC = m_coord.begin();
        m_coord.end() != itC; ++itC ){
      (*itC)->setStatus(HitBase::UsedByVeloRZ);
    }
  }

  double PatRZTrack::rInterpolated( double z ) {
    VeloRHits::const_iterator itF,itN, itNN;

    //== Get itF,itN the coordinates z is in between.
    if ( m_backward ) {
      itN = m_coord.begin();
      itF = itN+1;
      for ( itNN = itF+1; m_coord.end() != itNN; ++itNN ) {
        if ( (*itF)->z() > z ) break;
        itN = itF;
        itF = itNN;
      }
    } else {
      itN = m_coord.begin();
      itF = itN+1;
      for ( itNN = itF+1; m_coord.end() != itNN; ++itNN ) {
        if ( (*itF)->z() < z ) break;
        itN = itF;
        itF = itNN;
      }
    }

    double zRatio = (z - (*itN)->z() ) / ( (*itF)->z()- (*itN)->z() );
    return ( zRatio * (*itF)->coordHalfBox() + (1.-zRatio) * (*itN)->coordHalfBox() );
  }
}
