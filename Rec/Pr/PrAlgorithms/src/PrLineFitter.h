#ifndef PRLINEFITTER_H 
#define PRLINEFITTER_H 1

// Include files
#include "PrKernel/PrHit.h"

/** @class PrLineFitter PrLineFitter.h
 *  Simple class to fit a line with coordinates
 *
 *  @author Olivier Callot
 *  @date   2012-08-03
 */
class PrLineFitter {
public: 
  /// Standard constructor
  PrLineFitter( float z ) {
    m_z0  = z; 
    m_s0  = 0.;
    m_sz  = 0.;
    m_sz2 = 0.;
    m_sc  = 0.;
    m_scz = 0.;
  };
  
  virtual ~PrLineFitter( ){}; ///< Destructor

  void set( PrHit* h1=NULL, PrHit* h2=NULL ) {
    m_hits.clear();
    m_s0  = 0.;
    m_sz  = 0.;
    m_sz2 = 0.;
    m_sc  = 0.;
    m_scz = 0.;
    if ( NULL != h1 ) {
      addHitInternal( h1 );
      if ( NULL != h2 ) {
        addHitInternal( h2 );
        solve();
      }
    }
  }

  void addHit( PrHit* hit ) {
    addHitInternal( hit );
    if ( m_hits.size() > 1 ) solve();
  }

  float distance( PrHit* hit ) { return hit->coord() - (m_c0 + (hit->z()-m_z0) * m_tc); }
  
  
  float chi2( PrHit* hit ) {
    float d = distance( hit );
    return d * d * hit->w();
  }

  float coordAtRef() const { return m_c0; }
  float slope()      const { return m_tc; }

  PrHits& hits() { return m_hits; }

  void setHitsUnused() {
    for ( PrHits::iterator itH = m_hits.begin(); m_hits.end() != itH; ++itH ) {
      (*itH)->setUsed( false );
    }
  }


protected:

  void addHitInternal( PrHit* hit ) {
    m_hits.push_back( hit );
    float c = hit->coord();
    float w = hit->w();
    float z = hit->z() - m_z0;
    m_s0   += w;
    m_sz   += w * z;
    m_sz2  += w * z * z;
    m_sc   += w * c;
    m_scz  += w * c * z;
  }
  
  void solve() {
    float den = (m_sz*m_sz-m_s0*m_sz2);
    m_c0  = (m_scz * m_sz - m_sc * m_sz2) / den;
    m_tc  = (m_sc *  m_sz - m_s0 * m_scz) / den;
  }

private:
  float  m_z0;
  PrHits m_hits;
  float  m_c0;
  float  m_tc;

  float m_s0;
  float m_sz;
  float m_sz2;
  float m_sc;
  float m_scz;
  
};
#endif // PRLINEFITTER_H
