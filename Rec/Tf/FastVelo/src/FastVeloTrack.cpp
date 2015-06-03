// $Id: $
// Include files 

// local
#include "FastVeloTrack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FastVeloTrack
//
// 2010-09-09 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FastVeloTrack::FastVeloTrack(  ) :
  m_s0  (0.),
  m_sr  (0.),
  m_sz  (0.),
  m_srz (0.),
  m_sz2 (0.),
  m_zone(0),
  m_backward(false),
  m_r0    (-999.),
  m_tr    (-999.),
  m_r0Err2( 999.),
  m_trErr2( 999.),
  m_nbUsedRHits( 0 ),
  m_missedSensors( -1 ),
  m_valid( true ),
  m_x0( 0. ),
  m_tx( 0. ),
  m_y0( 0. ),
  m_ty( 0. ),
  m_qFactor( 0. ),
  m_sumGlobal( 0. ),
  m_sinDPhi( 0. ),
  m_sa2( 0. ),
  m_sa2z( 0. ),
  m_sa2z2( 0. ),
  m_sab( 0. ),
  m_sabz( 0. ),
  m_sabz2( 0. ),
  m_sb2( 0. ),
  m_sb2z( 0. ),
  m_sb2z2( 0. ),
  m_sac( 0. ),
  m_sacz( 0. ),
  m_sbc( 0. ),
  m_sbcz( 0. )
{
  m_rHits.reserve( 8 );
  m_phiHits.reserve( 8 );
}

//=============================================================================
// Destructor
//=============================================================================
FastVeloTrack::~FastVeloTrack() {
  m_rHits.clear();
  m_phiHits.clear();
}
//=========================================================================
//  Add a hit to the track. Update fit parameters
//=========================================================================
void FastVeloTrack::addRHit ( FastVeloHit* hit ) {
  m_rHits.push_back( hit );
  if ( 0 != hit->nbUsed() ) m_nbUsedRHits++;
  double z = hit->z();
  double w = hit->weight();
  double r = hit->global();

  m_s0  = m_s0  + w;
  m_sr  = m_sr  + w * r;
  m_sz  = m_sz  + w * z;
  m_srz = m_srz + w * r * z;
  m_sz2 = m_sz2 + w * z * z;

  if( m_rHits.size() > 2 ){
    double den = ( m_sz2 * m_s0 - m_sz * m_sz );
    if ( fabs(den) < 10e-10 ) den = 1.;
    m_tr     = ( m_srz * m_s0  - m_sr  * m_sz ) / den;
    m_r0     = ( m_sr  * m_sz2 - m_srz * m_sz ) / den;
    m_trErr2 = m_s0 / den;
    m_r0Err2 = m_sz2 / den;
  }
}
//=========================================================================
//  Add a hit to the track. Update fit parameters
//=========================================================================
void FastVeloTrack::removeRHit ( FastVeloHit* hit ) {
  m_rHits.erase( std::remove( m_rHits.begin(), m_rHits.end(), hit ), m_rHits.end() ); // remove the hit.
  if ( 0 != hit->nbUsed() ) m_nbUsedRHits--;
  double z = hit->z();
  double w = hit->weight();
  double r = hit->global();

  m_s0  = m_s0  - w;
  m_sr  = m_sr  - w * r;
  m_sz  = m_sz  - w * z;
  m_srz = m_srz - w * r * z;
  m_sz2 = m_sz2 - w * z * z;

  if( m_rHits.size() > 2 ){
    double den = ( m_sz2 * m_s0 - m_sz * m_sz );
    if ( fabs(den) < 10e-10 ) den = 1.;
    m_tr     = ( m_srz * m_s0  - m_sr  * m_sz ) / den;
    m_r0     = ( m_sr  * m_sz2 - m_srz * m_sz ) / den;
    m_trErr2 = m_s0 / den;
    m_r0Err2 = m_sz2 / den;
  }
}
//=========================================================================
//  Return the interpolated radius
//=========================================================================
double FastVeloTrack::rInterpolated( double z ) {
  FastVeloHits::const_iterator itF,itN, itNN;

  //== Get itF,itN the coordinates z is in between.
  //== Hits are sorted in z, increaseing of decreasing
  if ( m_rHits.front()->z() > m_rHits.back()->z() ) {
    itN = m_rHits.begin();
    itF = itN+1;
    for ( itNN = itF+1; m_rHits.end() != itNN; ++itNN ) {
      if ( (*itF)->z() < z ) break;
      itN = itF;
      itF = itNN;
    }
  } else {
    itN = m_rHits.begin();
    itF = itN+1;
    for ( itNN = itF+1; m_rHits.end() != itNN; ++itNN ) {
      if ( (*itF)->z() > z ) break;
      itN = itF;
      itF = itNN;
    }
  }

  double zRatio = (z - (*itN)->z() ) / ( (*itF)->z()- (*itN)->z() );
  return ( zRatio * (*itF)->global() + (1.-zRatio) * (*itN)->global() );
}

//=========================================================================
//  Count the number of shared hits. They are sorted by increaseing sensor number
//=========================================================================
bool FastVeloTrack::allHitsIncluded( FastVeloHits& hits ) {
  FastVeloHits::const_iterator itF1, itL1, itF2, itL2;
  itF1 = hits.begin();
  itF2 = m_rHits.begin();
  itL1 = hits.end();
  itL2 = m_rHits.end();
  unsigned int nCommon = 0;
  while ( itF1 != itL1 ) {
    if ( *itF1 == *itF2 ) {
      ++itF1;
      ++itF2;
      ++nCommon;
      if ( itF2 == itL2 ) break;
    } else if ( (*itF1)->sensor() < (*itF2)->sensor() ) {
      ++itF1;
    } else {
      return false;
    }
  }
  return nCommon == m_rHits.size();
}

//=========================================================================
//  Initialize a track from an existing one
//=========================================================================
void FastVeloTrack::setPhiClusters( FastVeloTrack& track,
                                    double cosPhi, double sinPhi,
                                    FastVeloHits::const_iterator it1, 
                                    FastVeloHits::const_iterator it2 ) {
  FastVeloHits::const_iterator itH;
  m_rHits.clear();
  for ( itH = track.rHits().begin(); track.rHits().end() != itH; ++itH ) {
    m_rHits.push_back( *itH );
  }
  m_zone = track.zone();
  m_missedSensors = track.nbMissedSensors();
  m_backward      = track.backward();  

  m_phiHits.clear();
  m_sumGlobal = 0.;
  m_qFactor = -1.;
  
  for ( itH = it1; it2 != itH; ++itH ) {
    m_sumGlobal += (*itH)->global();
    m_phiHits.push_back( *itH );
  }
  std::sort( m_phiHits.begin(), m_phiHits.end(), FastVeloHit::DecreasingByZ() );

  m_sinDPhi     = m_sumGlobal / m_phiHits.size();
  double cosDPhi = sqrt( 1. - m_sinDPhi * m_sinDPhi );   // small angle, cos is positive
  double xStart    = 1000. * ( cosPhi * cosDPhi   - sinPhi * m_sinDPhi );
  double yStart    = 1000. * ( cosPhi * m_sinDPhi + sinPhi * cosDPhi   );
  for ( itH = m_rHits.begin(); m_rHits.end() != itH; ++itH ) {
    (*itH)->setStartingPoint( xStart, yStart );
  }
  fitTrack();
}

//=========================================================================
//  Initialize a track from an existing one
//=========================================================================
void FastVeloTrack::setPhiClusters( FastVeloTrack& track,
                                    double x0, double tx, double y0, double ty,
                                    FastVeloHit* h1, FastVeloHit* h2, FastVeloHit* h3 ) {
  FastVeloHits::const_iterator itH;
  m_rHits.clear();
  for ( itH = track.rHits().begin(); track.rHits().end() != itH; ++itH ) {
    (*itH)->setStartingPoint( x0 + tx * (*itH)->z(), y0 + ty * (*itH)->z() );
    m_rHits.push_back( *itH );
  }
  m_zone = track.zone();
  m_missedSensors = track.nbMissedSensors();
  m_backward      = track.backward();

  m_phiHits.clear();
  m_phiHits.push_back( h1 );
  m_phiHits.push_back( h2 );
  m_phiHits.push_back( h3 );
  std::sort( m_phiHits.begin(), m_phiHits.end(), FastVeloHit::DecreasingByZ() );

  fitTrack();
}

//=========================================================================
//  Initialize a track from 4 clusters
//=========================================================================
void FastVeloTrack::setPhiClusters( FastVeloHit* r1, int zone, 
                                    FastVeloHit* h1, FastVeloHit* h2, FastVeloHit* h3 ) {
  FastVeloHits::const_iterator itH;
  m_rHits.clear();
  m_rHits.push_back( r1 );
  m_zone = zone;
  m_missedSensors = 0;
  m_backward      = 0;
  // update phi hits with a consistent weight (until fitted must be approx)
  h1->setPhiWeight(r1->rLocal());
  h2->setPhiWeight(r1->rLocal());
  h3->setPhiWeight(r1->rLocal());
  m_phiHits.clear();
  m_phiHits.push_back( h1 );
  m_phiHits.push_back( h2 );
  m_phiHits.push_back( h3 );
  fitTrack();
}

//=========================================================================
//  Initialize a track from 4 clusters
//=========================================================================
void FastVeloTrack::setPhiClusters( FastVeloHit* r1, FastVeloHit* r2, int zone, 
                                    FastVeloHit* h1, FastVeloHit* h2 ) {
  FastVeloHits::const_iterator itH;
  m_rHits.clear();
  m_rHits.push_back( r1 );
  m_rHits.push_back( r2 );
  m_zone = zone;
  m_phiHits.clear();
  m_phiHits.push_back( h1 );
  m_phiHits.push_back( h2 );
  fitTrack();
}

//=========================================================================
//  Fit the track, compute m_x0, m_tx, m_y0, m_ty.
//=========================================================================
void FastVeloTrack::fitTrack ( ) {
  m_sa2   = 0.;
  m_sa2z  = 0.;
  m_sa2z2 = 0.;
  m_sab   = 0.;
  m_sabz  = 0.;
  m_sabz2 = 0.;
  m_sb2   = 0.;
  m_sb2z  = 0.;
  m_sb2z2 = 0.;
  m_sac   = 0.;
  m_sacz  = 0.;
  m_sbc   = 0.;
  m_sbcz  = 0.;
  m_qFactor = -1.;
  
  FastVeloHits::iterator itH;
  for ( itH = m_rHits.begin(); m_rHits.end() != itH; ++itH ) {
    addToFit( *itH );
  }
  for ( itH = m_phiHits.begin(); m_phiHits.end() != itH; ++itH ) {
    addToFit( *itH );
  }
  solve();
}

//=========================================================================
//  Solve the system of 4 equation to obtain the track parameters
//=========================================================================
void FastVeloTrack::solve() {
  // Now solve the system...
  double a1 = m_sa2z  * m_sa2z  - m_sa2   * m_sa2z2;
  double a2 = m_sa2z  * m_sab   - m_sa2   * m_sabz ;
  double a3 = m_sa2z  * m_sabz  - m_sa2   * m_sabz2;
  double ac = m_sa2z  * m_sac   - m_sa2   * m_sacz ;
  
  double b1 = m_sab   * m_sa2z  - m_sa2   * m_sabz ;
  double b2 = m_sab   * m_sab   - m_sa2   * m_sb2  ;
  double b3 = m_sab   * m_sabz  - m_sa2   * m_sb2z ;
  double bc = m_sab   * m_sac   - m_sa2   * m_sbc  ;
  
  double c1 = m_sabz  * m_sa2z  - m_sa2   * m_sabz2;
  double c2 = m_sabz  * m_sab   - m_sa2   * m_sb2z ;
  double c3 = m_sabz  * m_sabz  - m_sa2   * m_sb2z2;
  double cc = m_sabz  * m_sac   - m_sa2   * m_sbcz ;
  
  //== now 3 equations...

  double d1 = b1 * a2 - a1 * b2;
  double d2 = b1 * a3 - a1 * b3;
  double dc = b1 * ac - a1 * bc;
  
  double e1 = c1 * a2 - a1 * c2;
  double e2 = c1 * a3 - a1 * c3;
  double ec = c1 * ac - a1 * cc;

  double den = (e1 * d2 - d1 * e2);
  if ( 0 != den ) den = 1./den;
  
  m_ty =  ( d1 * ec - e1 * dc ) * den;
  m_y0 =  ( e2 * dc - d2 * ec ) * den;
  m_tx = - ( ac  + a2   * m_y0 + a3  * m_ty ) / a1;
  m_x0 = - ( m_sac + m_sa2z * m_tx + m_sab * m_y0 + m_sabz * m_ty ) / m_sa2;
}

//=========================================================================
//  Fit, remove highest Chi2 in phi until OK or less than minExpected
//=========================================================================
bool FastVeloTrack::removeWorstMultiple( double maxChi2, unsigned int minExpected, bool withSin ) {
  double highest = 1000.;
  FastVeloHits::iterator itH, worst;
  int nbDiffSensors = 0;
  while ( m_phiHits.size() >= minExpected ) {
    highest = -1.;
    worst = m_phiHits.end();
    unsigned int prevSensor = 999;
    for ( itH = m_phiHits.begin(); m_phiHits.end() != itH ; ++itH ) {
      unsigned int nextSensor = 999;
      unsigned int mySensor   = (*itH)->sensor();
      double chi2 = 0.;
      if ( itH != m_phiHits.end()-1 ) nextSensor = (*(itH+1))->sensor();
      if ( mySensor == prevSensor || mySensor == nextSensor    ) {
        if ( withSin ) {
          chi2 = fabs( m_sinDPhi - (*itH)->global() );
        } else {
          chi2 = ( *itH )->chi2( m_x0 + (*itH)->z() * m_tx, m_y0 + (*itH)->z() * m_ty);
        }
        if ( highest < chi2 ) {
          highest = chi2;
          worst = itH;
        }
      }
      if ( mySensor != prevSensor ) nbDiffSensors++;
      prevSensor = mySensor;
    }
    if ( 3 > nbDiffSensors ) return false;
    if ( 0 > highest ) break;
    m_sumGlobal -= (*worst)->global();
    subtractToFit( *worst );
    m_phiHits.erase( worst );
    m_sinDPhi = m_sumGlobal / m_phiHits.size();
    if ( withSin ) solve();
  }
  if  (!withSin ) solve();

  //== Now filter the rest, i.e. sensors with a single hit

  while ( m_phiHits.size() >= minExpected ) {
    highest = -1.;
    worst = m_phiHits.end();
    for ( itH = m_phiHits.begin(); m_phiHits.end() != itH ; ++itH ) {
      double chi2 = ( *itH )->chi2( m_x0 + (*itH)->z() * m_tx, m_y0 + (*itH)->z() * m_ty);
      if ( highest < chi2 ) {
        highest = chi2;
        worst = itH;
      }
    }
    if ( highest < maxChi2 ) break;
    if ( m_phiHits.size() < minExpected ) break;
    subtractToFit( *worst );
    m_phiHits.erase( worst );
    solve();
  }
  return highest < maxChi2;
}
//=========================================================================
//  Fit, remove highest Chi2 in R and phi until OK
//=========================================================================
bool FastVeloTrack::removeWorstRAndPhi( double maxChi2, unsigned int minExpected ) {
  double highest = -1.;
  FastVeloHits::iterator itH, worst;
  bool worstIsPhi = true;
  updateRParameters();
  while ( m_phiHits.size() + m_rHits.size() >= minExpected ) {
    highest = -1.;
    worst = m_phiHits.end();
    for ( itH = m_phiHits.begin(); m_phiHits.end() != itH ; ++itH ) {
      double chi2 = ( *itH )->chi2( m_x0 + (*itH)->z() * m_tx, m_y0 + (*itH)->z() * m_ty);
      if ( highest < chi2 ) {
        highest = chi2;
        worst = itH;
        worstIsPhi = true;
      }
    }
    for ( itH = m_rHits.begin(); m_rHits.end() != itH ; ++itH ) {
      double chi2 = ( *itH )->chi2( m_x0 + (*itH)->z() * m_tx, m_y0 + (*itH)->z() * m_ty);
      if ( highest < chi2 ) {
        highest = chi2;
        worst = itH;
        worstIsPhi = false;
      }
    }
    if ( highest < maxChi2 ) break;  // includes no bad hit
    subtractToFit( *worst );
    if ( worstIsPhi ) {
      m_phiHits.erase( worst );
      if ( m_phiHits.size() < 3 ) break;
    } else {
      m_rHits.erase( worst );
      if ( m_rHits.size() < 3 ) break;
    }
    updateRParameters();
  }
  return highest < maxChi2;
}

//=========================================================================
//  Add another cluster
//=========================================================================
bool FastVeloTrack::addPhiCluster( FastVeloHit* hit, double maxChi2 ) {
  //== Avoid re-adding the same hit;
  if ( std::find( m_phiHits.begin(), m_phiHits.end(), hit ) != m_phiHits.end() ) return false;

  double chi2 = hit->chi2( m_x0 + hit->z() * m_tx, m_y0 + hit->z() * m_ty);
  if ( maxChi2 > chi2 ) {
    m_phiHits.push_back( hit );
    addToFit( hit );
    solve();
    return true;
  }
  return false;
}
//=========================================================================
// Add the best cluster... 
//=========================================================================
bool FastVeloTrack::addBestPhiCluster( FastVeloHits& hitList, double maxChi2ToAdd ) {
  bool added = false;
  double bestChi2 = 1.e9;
  FastVeloHit* best;
  FastVeloHits::const_iterator itH;
  for ( itH = hitList.begin() ; hitList.end() != itH ; ++itH ) {
    double ch = chi2( *itH );
    if ( ch < bestChi2 ) {
      bestChi2 = ch;
      best = *itH;
    }
  }
  if ( bestChi2 < maxChi2ToAdd ) {
    m_phiHits.push_back( best );
    addToFit( best );
    solve();
    added = true;
  } 
  return added;
}
//=========================================================================
// Add the best cluster in the other sensor of the station
//=========================================================================
bool FastVeloTrack::addBestClusterOtherSensor( FastVeloHits& hitList, double maxChi2ToAdd ) {
  bool added = false;
  unsigned int  foundSensor = 1000;
  FastVeloHits::const_iterator itH;
  for ( itH = hitList.begin() ; hitList.end() != itH ; ++itH ) {
    if ( std::find( m_phiHits.begin(), m_phiHits.end(), *itH ) != m_phiHits.end() ) {
      foundSensor = (*itH)->sensor();
      break;
    }
  }

  double bestChi2 = 1.e9;
  FastVeloHit* best;
  for ( itH = hitList.begin() ; hitList.end() != itH ; ++itH ) {
    if ( (*itH)->sensor() != foundSensor ) {
      double ch = chi2( *itH );
      if ( ch < bestChi2 ) {
        bestChi2 = ch;
        best = *itH;
      }
    }
  }
  if ( bestChi2 < maxChi2ToAdd ) {
    m_phiHits.push_back( best );
    addToFit( best );
    solve();
    added = true;
  } 
  return added;
}

//=========================================================================
//  Add the best R cluster(closest R)  of a sensor. Select zone also...
//=========================================================================
bool FastVeloTrack::addBestRCluster ( FastVeloSensor* sensor, double maxChi2 ) {
  double x = xAtZ( sensor->z() ) - sensor->xCentre();
  double y = yAtZ( sensor->z() ) - sensor->yCentre();
  if (  sensor->isRight() && x >  1. ) return false;
  if ( !sensor->isRight() && x < -1. ) return false;
  
  int  zone = 0;
  if ( fabs(x) > fabs(y) ) zone = 1;
  if ( x * y > 0 ) zone = 3 - zone;
  double rPred = sqrt( x*x + y*y);
  FastVeloHit* best = NULL;
  for ( FastVeloHits::const_iterator itH = sensor->hits(zone).begin();
        sensor->hits(zone).end() != itH; ++itH ) {
    double dist = fabs( rPred - (*itH)->rLocal() );
    double chi2 = (*itH)->weight() * dist * dist;
    if ( chi2 < maxChi2 ) {
      maxChi2 = chi2;
      best    = *itH;
    }
  }
  if ( NULL == best ) return false;                  
  m_rHits.push_back( best );
  best->setStartingPoint( m_x0 + best->z() * m_tx, m_y0 + best->z() * m_ty );
  addToFit( best );
  solve();
  updateRParameters();
  return true;
}

//=========================================================================
//  Return the covariance matrix of the last fit at the specified z
//=========================================================================
Gaudi::TrackSymMatrix FastVeloTrack::covariance( double z ) {
  Gaudi::TrackSymMatrix cov;

  cov(0,0) = m_sa2;
  cov(1,0) = m_sab;
  cov(1,1) = m_sb2;

  cov(2,0) = m_sa2z - z * m_sa2;
  cov(2,1) = m_sabz - z * m_sab;
  cov(3,0) = cov(2,1);
  cov(3,1) = m_sb2z - z * m_sb2;

  cov(2,2) = m_sa2z2 - 2 * z * m_sa2z + z * z * m_sa2;
  cov(3,2) = m_sabz2 - 2 * z * m_sabz + z * z * m_sab;
  cov(3,3) = m_sb2z2 - 2 * z * m_sb2z + z * z * m_sb2;

  cov(4,4) = 1.;

  bool ifail = cov.InvertChol( );
  if (!ifail ) std::cout << "FastVeloTrack:covariance: Choleski inverstion failed." << std::endl;
  
  return cov;
}
//=============================================================================

//===============================================================================
// Fit the track with a Kalman filter, allowing for some scattering at
// every hit. Function arguments:
//  state: state at the last filtered hit.
//  direction=+1 : filter in positive z direction (not normally what you want)
//  direction=-1 : filter in negative z direction
//  noise2PerLayer: scattering contribution (squared) to tx and ty
// The return value is the chi2 of the fit
// ===============================================================================
#include "FastVeloHitManager.h"

namespace {
  class ZLine
  {
  private:
    double m_z;
    Gaudi::Vector4 m_pars ;
  public:
    ZLine( const LHCb::State& s ) 
      : m_z(s.z()), m_pars(s.stateVector().Sub<Gaudi::Vector4>(0)) {}
    ZLine( double z, double x, double y, double tx, double ty) :
      m_z(z) { 
      m_pars(0) = x ;
      m_pars(1) = y ;
      m_pars(2) = tx ;
      m_pars(3) = ty ;
    }
	   
    double z() const { return m_z ; }
    const Gaudi::Vector4& parameters() const { return m_pars ; }
    double x( double z ) const { return m_pars[0] + (z - m_z)*m_pars[2] ; }
    double y( double z ) const { return m_pars[1] + (z - m_z)*m_pars[3] ; }
    double tx() const { return m_pars[2] ; }
    double ty() const { return m_pars[3] ; }
    Gaudi::XYZPoint position( double z ) const { return Gaudi::XYZPoint( x(z),y(z),z) ; }
  } ;
}

void FastVeloTrack::fitKalman(LHCb::State& state,
			      double& chi2,
			      int& ndof,
			      const int direction,
                              const float noise2PerLayer) const
{

  // make a single list of hits. r and phi hits are already sorted.
  std::vector<FastVeloHit*> hits(m_rHits.size()+m_phiHits.size()) ;
  if( m_rHits.front()->z() < m_rHits.back()->z()  ) 
    std::merge(m_phiHits.begin(),m_phiHits.end(),m_rHits.begin(),m_rHits.end(), hits.begin(), FastVeloHit::IncreasingByZ()) ;
  else 
    std::merge(m_phiHits.begin(),m_phiHits.end(),m_rHits.begin(),m_rHits.end(), hits.begin(), FastVeloHit::DecreasingByZ()) ;
  // if the direction is -1, then reverse the list
  if( direction < 0 ) 
    std::reverse( std::begin(hits), std::end(hits) ) ;
  
  // initialize with the state that comes in (to be improved ...)
  //const ZLine ref(state) ;
  const ZLine ref(0,m_x0,m_y0,m_tx,m_ty) ;
  Gaudi::Vector4 pars = ref.parameters() ;
  Gaudi::SymMatrix4x4 cov ;
  double z = ref.z() ;
  // move to the z-position of the first hit
  const double firstz = hits.front()->z() ;
  pars(0) += (firstz - z) * pars(2) ;
  pars(1) += (firstz - z) * pars(3) ;
  z = firstz ;
  // initialize the covariance matrix
  cov(0,0) = cov(1,1) = 1  ; //mm^2
  cov(2,2) = cov(3,3) = 0.01  ; //rad^2

  // compute the norm of the track direction, since we'll need is a couple of times
  //const double trkdirnorm2 = ( 1 + ref.tx()*ref.tx() + ref.ty()*ref.ty() ) ;
  
  // reset the chi2
  chi2 = 0 ;
  ndof = hits.size() - 4 ;

  // start filtering
  for( const auto& ihit : hits ) {
    // for now, just get the trajectory. this is too slow, but we'll cache rest later

    //const FastVeloSensor* fastsensor = hitmanager.sensor( ihit->sensor()) ;
    //std::auto_ptr<LHCb::Trajectory> traj = fastsensor->sensor()->trajectory( ihit->lhcbID().veloID(),0)  ;
    
    // compute the inner product of the norm to the sensor with the
    // track direction. we need this to compute the residual in the
    // plane, rather than as the doca.
    //
    // n = norm.Dot( trackdir) / ( norm.Mag() * trackdir.Mag() )
    //const double n = ( -fastsensor->dzDx() * ref.tx() -fastsensor->dzDy() * ref.ty() + 1.0 ) / 
    // std::sqrt( (1 + fastsensor->dzDx() * fastsensor->dzDx() + fastsensor->dzDy() * fastsensor->dzDy() * trkdirnorm2 ) ;
    
    // extrapolate the reference to a point close to the trajectory and get parameters of trajectory
    
    //const Gaudi::XYZPoint refpos = ref.position( ihit->z() ) ;
    //const double mu = traj->muEstimate( refpos ) ;
    //const Gaudi::XYZPoint hitpos = traj->position( mu ) ;
    //const Gaudi::XYZVector hitdir = traj->direction( mu )  ;
    
    // now compute the reference residual and the derivatives
    
    // std::cout << "hit " 
    // 	      << ihit->lhcbID().isVeloR() << " "
    // 	      << ihit->z() << " "
    // 	      << hitpos << " " << hitdir << " " << n << std::endl ;
    
    // first by my complicated version, not optimizing anything:
    //const Gaudi::XYZVector deltaX = refpos - hitpos ;
    //const Gaudi::XYZVector phat = hitdir.Cross( Gaudi::XYZVector(ref.tx(),ref.ty(),1.0) ).Unit() ;
    //const double doca = deltaX.Dot(phat) ;
    // let's now try to do the same using Olivier's caches
    
    //const double refresidual = ihit->a() * deltaX.x() + ihit->b()* deltaX.y()  ;
    const double refresidual = ihit->a() * ref.x(ihit->z()) + ihit->b()* ref.y(ihit->z()) + ihit->c() ;
    
    // std::cout << "residual: " << ihit->lhcbID().isVeloR() << " " << doca << " " << refresidual << " "
    // << ihit->distance( ref.x(ihit->z()), ref.y(ihit->z()) ) << std::endl ;

    // so, it turns out that these residuals are so identical, that we'd be stupid not to reuse Olivier's cache!
    
    // just to make sure I am not stupid, let's compute the derivatives as well:
    

    // then, let's fit!
    
    // first propagate the current state to the z position of this hit    
    const double dz  = ihit->z() - z ;
    const double dz2 = dz*dz ;
    pars(0) += dz * pars(2) ;
    pars(1) += dz * pars(3) ;
    z+= dz ;
    // Gaudi::Matrix4x4 F ;
    // F(0,0)=F(1,1)=F(2,2)=F(3,3) = 1 ;
    // F(0,2)=F(1,3)=dz ;
    // Gaudi::SymMatrix4x4 newcov = ROOT::Math::Similarity(F,cov) ;
    cov(0,0) += dz2*cov(2,2) + 2*dz*cov(2,0) ;
    cov(1,0) += dz2*cov(3,2) + dz*(cov(3,0)+cov(2,1)) ;
    cov(2,0) += dz*cov(2,2) ;
    cov(3,0) += dz*cov(3,2) ;
    cov(1,1) += dz2*cov(3,3) + 2*dz*cov(3,1) ;
    cov(2,1) += dz*cov(3,2) ;
    cov(3,1) += dz*cov(3,3) ;

    // filter. note the sign! Htx and Hty are zero, so we will note compute them.
    double Hx = ihit->a() ;
    double Hy = ihit->b() ;
    double R = 1/ihit->weight() + Hx*Hx*cov(0,0) + 2*Hx*Hy*cov(1,0) + Hy*Hy*cov(1,1) ;
    // residual relative to reference
    double r = refresidual + Hx * ( pars[0] - ref.x(z) ) + Hy * ( pars[1] - ref.y(z) ) ;
    chi2 += r*r/R ;

    // K = C * H^T * R^-1
    // update the state vector and cov matrix
    double CHT[4] ;
    CHT[0] = ( cov(0,0) * Hx + cov(0,1) * Hy ) ;
    CHT[1] = ( cov(1,0) * Hx + cov(1,1) * Hy ) ;
    CHT[2] = ( cov(2,0) * Hx + cov(2,1) * Hy ) ;
    CHT[3] = ( cov(3,0) * Hx + cov(3,1) * Hy ) ;
    for(int i=0; i<4; ++i) {
      pars(i) -= CHT[i] * r / R ;
    }

    for(int i=0; i<4; ++i) {
      for(int j=0; j<=i; ++j)
	cov(i,j) += -CHT[i]*CHT[j]/R ;
    }
    
    //
    // std::cout << "R, cov matrix after filtering hit: "
    // 	      << R << " "
    // 	      << std::sqrt(R) << " "
    // 	      << std::sqrt(cov(0,0)) << " " << std::sqrt(cov(1,1)) << " "
    // 	      << std::sqrt(1/ihit->weight()) << std::endl ;

    // finally, add the noise
    cov(2,2) += noise2PerLayer * (1 + ref.tx()*ref.tx()) ;
    cov(3,3) += noise2PerLayer * (1 + ref.ty()*ref.ty()) ;
    cov(3,2) += noise2PerLayer * ref.tx()*ref.ty() ;

    // std::cout << "reference at current z, new result: "
    // 	      << "(" << ref.x(z) << "," << ref.y(z) << "," << ref.tx() << " " << ref.ty() << ")  "
    // 	      << "(" << pars(0) << "," << pars(1) << " "<< pars(2) << " " << pars(3) << " ) "
    // 	      << chi2 << std::endl ;
  }

  // // std::cout << "FINAL reference at current z, new result: "
  // // 	    << z << " "
  // // 	    << "(" << ref.x(z) << "," << ref.y(z) << "," << ref.tx() << " " << ref.ty() << ")  "
  // // 	    << "(" << pars(0) << "," << pars(1) << " "<< pars(2) << " " << pars(3) << " ) "
  // // 	    << chi2 << std::endl ;
  
  // copy everything to the input state
  state.setZ( z ) ;
  for(int i=0; i<4; ++i) {
    state.stateVector()(i) = pars(i) ;
    for(int j=0; j<=i; ++j) {
      state.covariance()(i,j) = cov(i,j) ;
    }
  }
}
