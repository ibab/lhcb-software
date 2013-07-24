#ifndef PRSEEDQUADRUPLET_H 
#define PRSEEDQUADRUPLET_H 1

// Include files
#include "PrKernel/PrHit.h"

/** @class PrSeedQuadruplet PrSeedQuadruplet.h
 *  Handles a point made form a T station
 *
 *  @author Olivier Callot
 *  @date   2012-06-19
 */
class PrSeedQuadruplet {
public: 
  /// Standard constructor
  PrSeedQuadruplet( ) {
    m_used = false;
    m_x0 = 0.;
    m_z0 = 0.;
    m_y0 = 0.;
    m_tx = 0.;
    m_ty = 0.;
  }; 
  PrSeedQuadruplet( PrHit* x1, PrHit* u, PrHit* v, PrHit* x2 ) {
    m_used = false;
    m_hits.push_back( x1 );
    m_hits.push_back( u );
    m_hits.push_back( v );
    m_hits.push_back( x2 );

    float s0    = 0.;
    float sz    = 0.;
    float szs   = 0.;
    float sz2   = 0.;
    float sz2s  = 0.;
    float sz2s2 = 0.;
    
    float sx    = 0.;
    float sxz   = 0.;
    float sxzs  = 0.;
    
    for ( PrHits::const_iterator itH = m_hits.begin(); m_hits.end() != itH; ++itH ) {
      float x = (*itH)->x();
      float w = (*itH)->w();
      float z = (*itH)->z( 0. );
      float s = (*itH)->dxDy();
      s0    += w;
      sz    += w * z;
      szs   += w * z * s;
      sz2   += w * z * z;
      sz2s  += w * z * z * s;
      sz2s2 += w * z * z * s * s;
      sx    += w * x;
      sxz   += w * x * z;
      sxzs  += w * x * z * s;
    }
    double a1 = sz * sz  - s0 * sz2;
    double a2 = sz * szs - s0 * sz2s;
    double ax = sz * sx  - s0 * sxz;
    double b1 = szs * sz2  - sz * sz2s;
    double b2 = szs * sz2s - sz * sz2s2;
    double bx = szs * sxz  - sz * sxzs;
    
    double den = a2 * b1 - a1 * b2;
    m_ty = ( bx * a1 - ax * b1 ) / den;
    m_tx = ( bx * a2 - ax * b2 ) / den;
    m_x0 = ( sx - sz * m_tx - szs *m_ty ) / s0;
    m_z0 = sz / s0;
    m_y0 = m_ty * m_z0;
    m_x0 = m_x0 + m_tx * m_z0;
  }; 

  virtual ~PrSeedQuadruplet( ){}; ///< Destructor

  float x0() const { return m_x0; }
  float y0() const { return m_y0; }
  float z0() const { return m_z0; }
  float tx() const { return m_tx; }
  float ty() const { return m_ty; }

  PrHits& hits()   { return m_hits; }
  const PrHits& hits() const  { return m_hits; }

  bool isUsed()    { return m_used; }
  void setUsed( bool flag ) { m_used = flag; }

  struct LowerByTy {
    bool operator() (const PrSeedQuadruplet lhs, const PrSeedQuadruplet rhs ) const { return lhs.ty() < rhs.ty(); }
  };

protected:

private:
  PrHits m_hits;
  float  m_x0;
  float  m_y0;
  float  m_z0;
  float  m_tx;
  float  m_ty;
  bool   m_used;
};

typedef std::vector<PrSeedQuadruplet> PrSeedQuadruplets;

#endif // PRSEEDQUADRUPLET_H
