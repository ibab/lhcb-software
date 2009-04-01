// $Id: NewSpaceTrack.h,v 1.1 2009-04-01 08:11:45 ocallot Exp $
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
class NewSpaceTrack {
public: 
  /// Standard constructor
  NewSpaceTrack( std::vector<Tf::PatVeloPhiHit*>::const_iterator it1,
                 std::vector<Tf::PatVeloPhiHit*>::const_iterator it2 ); 

  virtual ~NewSpaceTrack( ); ///< Destructor

  void addCluster( Tf::PatVeloPhiHit* hit );

  void removeCluster( Tf::PatVeloPhiHit* hit );

  void fitTrack( );

  std::vector<Tf::PatVeloPhiHit*>::const_iterator begin() { return m_hits.begin(); }
  std::vector<Tf::PatVeloPhiHit*>::const_iterator end()   { return m_hits.end(); }
  std::vector<Tf::PatVeloPhiHit*>::iterator beginHits() { return m_hits.begin(); }
  std::vector<Tf::PatVeloPhiHit*>::iterator endHits()   { return m_hits.end(); }
  unsigned int nbHits()                                   { return m_hits.size(); }

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

  double qFactor() {
    if ( 100 < m_qFactor ) return m_qFactor;
    m_qFactor = 0.;
    for ( std::vector<Tf::PatVeloPhiHit*>::const_iterator itH = m_hits.begin(); 
          m_hits.end() != itH ; ++itH ) {
      m_qFactor += dist2( *itH );
    }
    m_qFactor = m_qFactor / ( m_hits.size() - 2 );
    m_qFactor = m_qFactor + .5 * ( m_hits.size() - m_nbUnused );
    return m_qFactor;
  }

  void setValid( bool valid )   { m_valid = valid; }
  bool isValid() const { return m_valid; }
  
protected:

private:
  bool   m_valid;
  std::vector<Tf::PatVeloPhiHit*> m_hits;
  double m_s0;
  double m_sx;
  double m_sy;
  double m_sz;
  double m_sxz;
  double m_syz;
  double m_sz2;
  
  double m_x0;
  double m_tx;
  double m_y0;
  double m_ty;
  double m_z0;

  double m_sPhi;
  double m_averagePhi;
  
  int    m_nbUnused;
  double m_qFactor;
};
#endif // NEWSPACETRACK_H
