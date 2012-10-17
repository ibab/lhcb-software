// $Id: $
#ifndef FASTVELOHIT_H
#define FASTVELOHIT_H 1

// Include files
#include "Kernel/LHCbID.h"
#include "Event/VeloLiteCluster.h"

//== Auxilliary class to store all parameters in the same memory area, to 
//== optimise memory access when decoding the Velo data

class FastVeloLineParams {
public:
  FastVeloLineParams( float a, float b, float c, float xs, float ys ) {
    m_a = a;
    m_b = b;
    m_c = c;
    m_xs = xs;
    m_ys = ys;
  };
  
  FastVeloLineParams( double a, double b, double c, double xs, double ys ) {
    m_a = float(a);
    m_b = float(b);
    m_c = float(c);
    m_xs = float(xs);
    m_ys = float(ys);
  };
  
  ~FastVeloLineParams() {};
  
  float a()  const { return m_a; }
  float b()  const { return m_b; }
  float c()  const { return m_c; }
  float xs() const { return m_xs; }
  float ys() const { return m_ys; }
private:  
  float m_a;
  float m_b;
  float m_c;
  float m_xs;
  float m_ys;
};

/** @class FastVeloHit FastVeloHit.h
 *  This defines the Velo hits (R, Phi) to be used in the fast pattern.
 *
 * The internal representation is FLOAT to decrease the amount of memory,
 * and keep it in cache. This speeds up the FastVelo tracking by 10% on certain 
 * processors. The interface is only double, to be LHCb standard.
 *
 *  @author Olivier Callot
 *  @date   2010-09-08
 */

class FastVeloHit {
public:
  /// Standard constructor
  FastVeloHit( ):
    m_zone( 0 ), m_z(0.), m_rLocal( 0. ), m_global( 0. ), m_xGlobal( 0. ),
    m_yGlobal( 0. ), m_weight( 0. ), m_phiWeight( 0. ), m_nbUsed( 0 ),
    m_sensor( 0 ), m_a( 0.), m_b( 0. ), m_c( 999. ), m_xStripCentre( 0. ),
    m_yStripCentre( 0. ), m_xCentre( 0. ), m_yCentre( 0. )
  { };

  virtual ~FastVeloHit( ) {}; ///< Destructor

  void setHit( const LHCb::VeloLiteCluster clus,
               const unsigned int zone,
               const double z,
               const double rLocal,
               const double weight ) {
    m_cluster   = clus;
    m_zone      = zone;
    m_z         = float(z);
    m_rLocal    = float(rLocal);
    m_global    = float(rLocal);
    m_weight    = float(weight);
    m_phiWeight = float(weight);
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
  double global()       const { return m_global; }
  double weight()       const { return m_weight; }
  double z()            const { return m_z;      }
  unsigned int sensor() const { return m_sensor; }
  unsigned int zone()   const { return m_zone; }
  unsigned int nbUsed() const { return m_nbUsed; }
  double xGlobal()      const { return m_xGlobal; }
  double yGlobal()      const { return m_yGlobal; }
  double rLocal()       const { return m_rLocal; }

  void   setGlobal( double r )  { m_global = float(r); }
  void   setUsed()              { m_nbUsed++; }
  void   clearUsed()            { m_nbUsed--; }
  void   resetUsed()            { m_nbUsed = 0; }
  void   setGlobalPosition( double x, double y ) { 
    m_xGlobal = float(x); 
    m_yGlobal = float(y);
  }
  void   setZ( double z )  { m_z = float(z); }
  void   setSensorCentre( double x, double y ) { m_xCentre = float(x); m_yCentre = float(y); }

  double a() const { return m_a; }
  double b() const { return m_b; }
  double c() const { return m_c; }

  void setLineParameters( FastVeloLineParams params ) {
    m_a  = params.a();
    m_b  = params.b();
    m_c  = params.c();
    m_xStripCentre = params.xs();
    m_yStripCentre = params.ys();
  }

  double distance( double x, double y ) const { return m_a * float(x) + m_b * float(y) + m_c; }

  double chi2( double x, double y ) const {
    double d = distance( x, y );
    return m_weight * d * d;
  }

  double xStripCentre () const { return m_xStripCentre; }
  double yStripCentre () const { return m_yStripCentre; }

  void setZone( unsigned int zone )  { m_zone = zone; }

  void setStartingPoint(  double x, double y ) {
    // x and y in global. Not necessarily on the strip, but define the direction.
    float dx = float(x) - m_xCentre;
    float dy = float(y) - m_yCentre;
    float norm = sqrt( dx*dx + dy*dy );
    m_a = dx / norm;
    m_b = dy / norm;
    float xOnStrip = m_xCentre + m_a * m_rLocal;
    float yOnStrip = m_yCentre + m_b * m_rLocal;
    m_c = - ( xOnStrip * m_a + yOnStrip * m_b );
  }  

  void setPhiWeight( double r ) { m_weight = m_phiWeight/float(r*r); }

  void setPhiWeight( double x, double y ) { 
    double dx = x - m_xCentre;
    double dy = y - m_yCentre;
    m_weight = m_phiWeight/float(dx*dx+dy*dy); 
  }

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
