// Include files 

// local
#include "NewSpaceTrack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : NewSpaceTrack
//
// 2009-03-24 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
NewSpaceTrack::NewSpaceTrack( std::vector<Tf::PatVeloPhiHit*>::const_iterator it1,
                              std::vector<Tf::PatVeloPhiHit*>::const_iterator it2 ) {
  m_s0  = 0.;
  m_sx  = 0.;
  m_sy  = 0.;
  m_sz  = 0.;
  m_sxz = 0.;
  m_syz = 0.;
  m_sz2 = 0.;
  m_sPhi = 0.;
  m_averagePhi = 0.;

  m_x0 = 0.;
  m_tx = 0.;
  m_y0 = 0.;
  m_ty = 0.;

  m_nbUnused = 0;
  m_qFactor  = -999.;
  m_valid    = true;
  
  for ( std::vector<Tf::PatVeloPhiHit*>::const_iterator itH = it1; it2 >= itH ; ++itH ) {
    addCluster( *itH );
  }
  std::sort( m_hits.begin(), m_hits.end(), Tf::PatVeloPhiHit::DecreasingByZ() );
  fitTrack();
}
//=============================================================================
// Destructor
//=============================================================================
NewSpaceTrack::~NewSpaceTrack() {}

//=========================================================================
//  Add a cluster to the track, fit the new parameters
//=========================================================================
void NewSpaceTrack::addCluster ( Tf::PatVeloPhiHit* hit) {
  m_qFactor  = -999.;
  m_hits.push_back( hit );
  if ( !hit->hit()->isUsed() ) ++m_nbUnused;
  m_sPhi = m_sPhi + hit->referencePhi();

  double z  = 0.001 * hit->z();
  double r  = hit->referenceR();
  double x  = r * hit->cosPhi();
  double y  = r * hit->sinPhi();
  double w  = hit->weight() / r / r;
  
  m_s0  = m_s0  + w;
  m_sx  = m_sx  + w * x;
  m_sy  = m_sy  + w * y;
  m_sz  = m_sz  + w * z;
  m_sxz = m_sxz + w * x * z;
  m_syz = m_syz + w * y * z;
  m_sz2 = m_sz2 + w * z * z;
}

//=========================================================================
//  Remove the worst hits, first by distance to average phi when multiple in sensor, 
//  then by chi2.
//=========================================================================
bool NewSpaceTrack::removeWorstMultiple( double maxChi2, unsigned int minExpected, bool debug ) {
  double highest = 1000.;
  std::vector<Tf::PatVeloPhiHit*>::iterator itH;
  while ( m_hits.size() >= minExpected ) {
    highest = -1.;
    Tf::PatVeloPhiHit* worst = 0;
    unsigned int prevSensor = 999;
    for ( itH = m_hits.begin(); m_hits.end() != itH ; ++itH ) {
      unsigned int nextSensor = 999;
      unsigned int mySensor   = (*itH)->sensor()->sensorNumber();
      if ( itH != m_hits.end()-1 ) nextSensor = (*(itH+1))->sensor()->sensorNumber();
      if ( mySensor == prevSensor || mySensor == nextSensor    ) {
        double chi2 = dist2( *itH );
        if ( highest < chi2 ) {
          highest = chi2;
          worst = *itH;
        }
      }
      prevSensor = mySensor;
    }
    if ( 0 > highest ) break;
    if ( 0 != worst ) {
      removeCluster( worst );
      if ( debug  )
        std::cout << "Remove cluster Sensor " << worst->sensorNumber() 
                  << " strip " << worst->hit()->strip() 
                  << " highest Chi2 " << highest << std::endl;
    }
  }

  //== Now filter the rest, i.e. sensors with a single hit

  highest = 1000.;
  while ( m_hits.size() >= minExpected ) {
    highest = -1.;
    Tf::PatVeloPhiHit* worst = 0;
    for ( itH = m_hits.begin(); m_hits.end() != itH ; ++itH ) {
      double chi2 = dist2( *itH );
      if ( debug ) std::cout << "  Cluster Sensor " << (*itH)->sensorNumber() 
                             << " strip " << (*itH)->hit()->strip() 
                             << " chi2 " << chi2 << std::endl;
      if ( highest < chi2 ) {
        highest = chi2;
        worst = *itH;
      }
    }
    if ( highest < maxChi2 ) break;
    removeCluster( worst );
    if ( debug ) std::cout << "Remove cluster Sensor " << worst->sensorNumber() 
                           << " strip " << worst->hit()->strip() 
                           << " highest chi2 " << highest << std::endl;
  }
  return highest < maxChi2;
}
//=========================================================================
//  Remove a cluster to the track and fit the new parameters
//=========================================================================
void NewSpaceTrack::removeCluster ( Tf::PatVeloPhiHit* hit) {
  m_qFactor  = -999.;

  m_hits.erase( std::find( m_hits.begin(), m_hits.end(), hit ) );
  if ( !hit->hit()->isUsed() ) --m_nbUnused;
  m_sPhi = m_sPhi - hit->referencePhi();

  double z  = 0.001 * hit->z();
  double r  = hit->referenceR();
  double x  = r * hit->cosPhi();
  double y  = r * hit->sinPhi();
  double w  = hit->weight() / r / r;
  
  m_s0  = m_s0  - w;
  m_sx  = m_sx  - w * x;
  m_sy  = m_sy  - w * y;
  m_sz  = m_sz  - w * z;
  m_sxz = m_sxz - w * x * z;
  m_syz = m_syz - w * y * z;
  m_sz2 = m_sz2 - w * z * z;

  fitTrack();
}
//=========================================================================
//  Compute the track's parameters
//=========================================================================
void NewSpaceTrack::fitTrack ( ) {
  double denom = m_sz2 * m_s0 - m_sz * m_sz;
  if ( fabs(denom) < 1.e-10 ) denom = 1;
  m_x0 = ( m_sx * m_sz2 - m_sxz * m_sz ) / denom;
  m_y0 = ( m_sy * m_sz2 - m_syz * m_sz ) / denom;
  m_tx = 0.001 * ( m_sxz * m_s0 - m_sx * m_sz ) / denom;
  m_ty = 0.001 * ( m_syz * m_s0 - m_sy * m_sz ) / denom;  
  m_averagePhi = m_sPhi / m_hits.size();
}
//=============================================================================
