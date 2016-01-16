#ifndef PRSEEDTRACK_H 
#define PRSEEDTRACK_H 1

// Include files
#include "Event/Track.h"
#include "PrKernel/PrHit.h"

/** @class PrSeedTrack PrSeedTrack.h
 *  This is the working class inside the T station pattern
 *
 *  @author Olivier Callot
 *  @date   2012-03-22             
 */

class PrSeedTrack {
 public: 
  /// Constructor with the z reference
  PrSeedTrack( unsigned int zone, float zRef ) 
    : m_zone( zone ), m_zRef( zRef ){
    init();
  };
  /// Constructor with the z reference
  PrSeedTrack( unsigned int zone, float zRef, PrHits& hits )
    : m_zone( zone ), m_zRef( zRef ), m_hits( hits ){
    init();
  }; 
  
  virtual ~PrSeedTrack( ) {}; ///< Destructor
    
  /// Handling of hits: acceess, insertion
  PrHits& hits()                   { return m_hits; }
  const PrHits& hits()        const{ return m_hits;  }
  void addHit( PrHit* hit )        { m_hits.push_back( hit );}
  void addHits( PrHits& hits )     {
    m_hits.reserve( m_hits.size() + hits.size() );
    m_hits.insert( m_hits.end(), hits.begin(), hits.end() );
  }
  unsigned int zone() const { return m_zone; }
  
  /// Parameters of the trajectory in the T stations
  void setParameters( float ax, float bx, float cx, float ay, float by ) {
    m_ax = ax;
    m_bx = bx;
    m_cx = cx;
    m_ay = ay;
    m_by = by;
  }
  float ax()                      const { return m_ax;}
  float bx()                      const { return m_bx;}
  float cx()                      const { return m_cx;}
  float ay()                      const { return m_ay;}
  float by()                      const { return m_by;}
  
  //**********Update the parameters of a track iteratively in the Fit
  void updateParameters( float dax, float dbx, float dcx,
                         float day=0., float dby= 0.  ) {
    m_ax += dax;
    m_bx += dbx;
    m_cx += dcx;
    m_ay += day;
    m_by += dby;
  }

  void setYParam( float ay=0., float by=0.){
    m_ay = ay;
    m_by = by;
  }
  
  void setnXnY( unsigned int nx, unsigned int ny){
    m_nx = nx;
    m_ny = ny;
  }
  unsigned int nx() const{return m_nx;}
  unsigned int ny() const{return m_ny;}
  
  //validity
  void setValid( bool v )             { m_valid = v; }
  bool  valid()                 const { return m_valid; }
  
  //dRatio
  void setdRatio( float dRatio )     { m_dRatio = dRatio;}
  float dRatio()                const{ return m_dRatio;}
  
  // chi2
  void setChi2(float chi2, int nDoF) { m_chi2 = chi2;m_nDoF = nDoF;m_chi2DoF = m_chi2/m_nDoF; }
  float chi2()                 const { return m_chi2; }
  float chi2PerDoF()           const { return m_chi2DoF;}
  int nDoF()                    const { return m_nDoF; }
  void setMaxChi2(float val1)        { m_maxChi2 = val1;}
  float MaxChi2()              const { return m_maxChi2;}
 

  //y(at a given z)
  float y( float z )           const { return (m_ay + m_by*z);}
  //Slope by
  float ySlope( )               const { return m_by; }
  
  //X at a given Z
  float x( float z )const{
    const float dz = z-m_zRef;
    return m_ax + dz * ( m_bx + dz* m_cx * (1.+m_dRatio*dz) );
  }
  
  //Slope X-Z plane track at a given z
  float xSlope( float z )const{ 
    const float dz = z-m_zRef;
    return m_bx + 2.*dz*m_cx + 3.*dz*dz*m_cx*m_dRatio; 
  }

  //y positon on Track
  float yOnTrack( PrHit* hit )   const{ return hit->yOnTrack( m_ay , m_by ); }
  
  //distance from Hit of the track
  float distance( PrHit* hit )   const{ 
    float yTra = yOnTrack(hit); 
    return hit->x( yTra ) - x( hit->z(yTra) );
  }
 
  //Chi2 contribution from a single Hit
  float chi2( PrHit* hit )       const{ 
    const float d = distance( hit );
    float w = hit->w();
    if(m_slopeCorr){
      float yTra = yOnTrack(hit);
      const float Corr = std::cos(xSlope( (float) hit->z(yTra) ));
      w = hit->w()*(Corr*Corr);
    }
    return d * d * w;
  }
  void setSlopeCorr( bool corr){
    m_slopeCorr = corr;
  }
  bool slopeCorr() const{
    return m_slopeCorr;
  }
  //DeltaY
  float deltaY( PrHit* hit )        const{
    if ( hit->isX() ) return 0.;
    return distance( hit ) / hit->dxDy();
  }
  
  struct GreaterBySize{//Before elements with lower n hits later and lower Chi2
    bool operator() (const PrSeedTrack& lhs, const PrSeedTrack& rhs) const { 
      if( lhs.hits().size() == rhs.hits().size() ){
        return lhs.chi2PerDoF() > rhs.chi2PerDoF();
      }
      return lhs.hits().size() < rhs.hits().size();
    }
  };
  
  struct LowerBySize{ //Before higher number of hits and better chi2
    bool operator() ( const PrSeedTrack& lhs, const PrSeedTrack& rhs) const{
      if(lhs.hits().size() == rhs.hits().size()){
        return lhs.chi2PerDoF() < rhs.chi2PerDoF();
      }
      return lhs.hits().size() > rhs.hits().size();
    }
  };
  
  int size() const{
    return m_hits.size();
  };
  
protected:
  
private:

  void init(){
    m_slopeCorr = false;
    m_valid     = true;
    m_hits.reserve( 32 );

    m_nx = 0;
    m_ny = 0;
    //track params
    m_ax = 0.;
    m_bx = 0.;
    m_cx = 0.;
    m_by = 0.;
    m_ay = 0.;
    //track chi2 settings
    m_chi2    = 0.;
    m_chi2DoF = 0.;
    m_nDoF    = -1;
    m_dRatio  = 0.;
    m_maxChi2 = -99999.;
  }
  //Global Private variables
private:
  bool          m_valid;
  bool          m_slopeCorr;
  unsigned int  m_nx;
  unsigned int  m_ny;
  unsigned int  m_zone;
  int           m_type;
  int           m_nDoF;
  float         m_chi2DoF;
  float         m_zRef;
  float         m_chi2;
  float         m_ax;
  float         m_bx;
  float         m_cx;
  float         m_ay;
  float         m_by;
  float         m_dRatio;
  float         m_maxChi2;

  PrHits        m_hits;
};

typedef std::vector<PrSeedTrack> PrSeedTracks;

#endif // PRSEEDTRACK2_H
