#ifndef NEWSPACETRACK_H 
#define NEWSPACETRACK_H 1

// Include files
#include "PatVeloHit.h"

/** @class NewSpaceTrack NewSpaceTrack.h
 *  This is a simple class to handle a cluster of PHI measurements
 *
 *  @author Olivier Callot
 *  @date   2009-03-24
 */
class NewSpaceTrack final {
public: 
  /// Standard constructor
  NewSpaceTrack( std::vector<Tf::PatVeloPhiHit*>::const_iterator it1,
                 std::vector<Tf::PatVeloPhiHit*>::const_iterator it2 ); 

  void addCluster( Tf::PatVeloPhiHit* hit );

  void removeCluster( Tf::PatVeloPhiHit* hit );

  void fitTrack( );

  std::vector<Tf::PatVeloPhiHit*>::const_iterator begin() const { return m_hits.begin(); }
  std::vector<Tf::PatVeloPhiHit*>::const_iterator end()   const { return m_hits.end(); }
  std::vector<Tf::PatVeloPhiHit*>::iterator beginHits() { return m_hits.begin(); }
  std::vector<Tf::PatVeloPhiHit*>::iterator endHits()   { return m_hits.end(); }
  unsigned int nbHits() const                           { return m_hits.size(); }

  bool removeWorstMultiple( double maxChi2, unsigned int minExpected, bool debug );

  double dPhi( const Tf::PatVeloPhiHit* hit ) const {
    return hit->referencePhi() - m_averagePhi;
  }
  
  double dist2( const Tf::PatVeloPhiHit* hit ) const {
    double z  = hit->z();
    double r  = hit->referenceR();
    double x  = r * hit->cosPhi();
    double y  = r * hit->sinPhi();
    double w  = hit->weight() / r / r;
    double dx = m_x0 + z * m_tx - x;
    double dy = m_y0 + z * m_ty - y;
    return w * (dx*dx+dy*dy);
  };
  
  int nbUnused() const { return m_nbUnused; }

  double qFactor() const {
    if ( -100 < m_qFactor ) return m_qFactor;
    m_qFactor = std::accumulate( m_hits.begin(), m_hits.end(), 0.,
                                 [&](double qF, const Tf::PatVeloPhiHit* h)
                                 { return qF + this->dist2(h); } );
    m_qFactor = m_qFactor / ( m_hits.size() - 2 );
    m_qFactor = m_qFactor + ( m_hits.size() - m_nbUnused );
    return m_qFactor;
  }

  void setValid( bool valid )   { m_valid = valid; }
  bool isValid() const { return m_valid; }

  double averagePhi() const { return m_averagePhi; }

private:
  bool   m_valid;
  std::vector<Tf::PatVeloPhiHit*> m_hits;
  double m_s0 = 0.;
  double m_sx = 0.;
  double m_sy = 0.;
  double m_sz = 0.;
  double m_sxz = 0.;
  double m_syz = 0.;
  double m_sz2 = 0.;
  
  double m_x0 = 0.;
  double m_tx = 0.;
  double m_y0 = 0.;
  double m_ty = 0.;

  double m_sPhi = 0.;
  double m_averagePhi = 0.;
  
  int    m_nbUnused = 0;
  mutable double m_qFactor = -999.;
};
#endif // NEWSPACETRACK_H
