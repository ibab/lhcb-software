// $Id: $
#ifndef FASTVELOHIT_H
#define FASTVELOHIT_H 1

// Include files
#include "Kernel/LHCbID.h"
#include "Event/VeloLiteCluster.h"

/** @class FastVeloHit FastVeloHit.h
 *  This defines the Velo hits (R, Phi) to be used in the fast pattern.
 *
 *  @author Olivier Callot
 *  @date   2010-09-08
 */

class FastVeloLineParams {
public:
  FastVeloLineParams( float a, float b, float c, float xs, float ys ) {
    m_a = a;
    m_b = b;
    m_c = c;
    m_xs = xs;
    m_ys = ys;
  };
  
  ~FastVeloLineParams() {};
  
  float a() const { return m_a; }
  float b() const { return m_b; }
  float c() const { return m_c; }
  float  xs() const { return m_xs; }
  float  ys() const { return m_ys; }
private:  
  float m_a;
  float m_b;
  float m_c;
  float  m_xs;
  float  m_ys;
};

class FastVeloHit {
public:
  /// Standard constructor
  FastVeloHit( ):
    m_zone( 0 ), m_z(0.), m_rLocal( 0. ), m_weight( 0. ), m_nbUsed( 0 ) {
    m_sensor = 0;
    m_a      = 0.;
    m_b      = 0.;
    m_c      = 999.;
  };

  virtual ~FastVeloHit( ) {}; ///< Destructor

  void setHit( const LHCb::VeloLiteCluster clus,
               const unsigned int zone,
               const float z,
               const float rLocal,
               const float weight ) {
    m_cluster   = clus;
    m_zone      = zone;
    m_z         = z;
    m_rLocal    = rLocal;
    m_global    = rLocal;
    m_weight    = weight;
    m_phiWeight = weight;
    m_nbUsed    = 0;
    m_sensor    = clus.channelID().sensor();
    m_a         = 0.;
    m_b         = 0.;
    m_c         = 1.;
    m_xStripCentre = 0.;
    m_yStripCentre = 0.;
    m_xGlobal    = 0.;
    m_yGlobal    = 0.;
    m_xCentre    = 0.;
    m_yCentre    = 0.;
  }

  LHCb::LHCbID lhcbID() const { return m_cluster.channelID();  }
  LHCb::VeloLiteCluster cluster() const { return m_cluster;  }
  float global()        const { return m_global; }
  float weight()        const { return m_weight; }
  float z()             const { return m_z;      }
  unsigned int sensor() const { return m_sensor; }
  unsigned int zone()   const { return m_zone; }
  unsigned int nbUsed() const { return m_nbUsed; }
  float xGlobal()       const { return m_xGlobal; }
  float yGlobal()       const { return m_yGlobal; }

  void   setGlobal( float r )  { m_global = r; }
  void   setUsed()              { m_nbUsed++; }
  void   clearUsed()            { m_nbUsed--; }
  void   setGlobalPosition( float x, float y ) { 
    m_xGlobal = x; 
    m_yGlobal = y;
  }
  void   setZ( float z )  { m_z = z; }
  void   setSensorCentre( float x, float y ) { m_xCentre = x; m_yCentre = y; }

  float a() const { return m_a; }
  float b() const { return m_b; }
  float c() const { return m_c; }

  void setLineParameters( FastVeloLineParams params ) {
    m_a  = params.a();
    m_b  = params.b();
    m_c  = params.c();
    m_xStripCentre = params.xs();
    m_yStripCentre = params.ys();
  }

  float distance( float x, float y ) const { return m_a * x + m_b * y + m_c; }

  float chi2( float x, float y ) const {
    float d = distance( x, y );
    return m_weight * d * d;
  }

  float xStripCentre () const { return m_xStripCentre; }
  float yStripCentre () const { return m_yStripCentre; }

  void setZone( unsigned int zone )  { m_zone = zone; }

  void setStartingPoint(  float x, float y ) {
    // x and y in global. Not necessarily on the strip, but define the direction.
    float dx = x - m_xCentre;
    float dy = y - m_yCentre;
    float norm = sqrt( dx*dx + dy*dy );
    m_a = dx / norm;
    m_b = dy / norm;
    float xOnStrip = m_xCentre + m_a * m_rLocal;
    float yOnStrip = m_yCentre + m_b * m_rLocal;
    m_c = - ( xOnStrip * m_a + yOnStrip * m_b );
  }  

  void setPhiWeight( float r ) { m_weight = m_phiWeight/(r*r); }

  struct DecreasingByZ  {
    bool operator() (const FastVeloHit* lhs, const FastVeloHit* rhs) const { return lhs->z() > rhs->z(); }
  };

  struct IncreasingByZ  {
    bool operator() (const FastVeloHit* lhs, const FastVeloHit* rhs) const { return lhs->z() < rhs->z(); }
  };

  struct LowerByGlobal  {
    bool operator() (const FastVeloHit* lhs, const FastVeloHit* rhs) const { return lhs->global() < rhs->global(); }
  };

protected:

private:
  LHCb::VeloLiteCluster m_cluster;
  unsigned int  m_zone;
  float        m_z;
  float        m_rLocal;
  float        m_global;
  float        m_xGlobal;
  float        m_yGlobal;
  float        m_weight;
  float        m_phiWeight;
  unsigned int  m_nbUsed;
  unsigned int  m_sensor;
  float        m_a;
  float        m_b;
  float        m_c;
  float        m_xStripCentre;
  float        m_yStripCentre;
  float        m_xCentre;
  float        m_yCentre;
};

typedef std::vector<FastVeloHit*> FastVeloHits;

#endif // FASTVELOHIT_H
