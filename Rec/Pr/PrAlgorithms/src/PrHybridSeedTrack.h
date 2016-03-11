#ifndef PRHYBRIDSEEDTRACK_H 
#define PRHYBRIDSEEDTRACK_H 1

// Include files
#include "Event/Track.h"
#include "PrKernel/PrHit.h"

/** @class PrHybridSeedTrack PrHybridSeedTrack.h
 *  This is the working class inside the T station pattern
 *
 *  @author Renato Quagliani
 *  @date   2015-05-13
 */
//Comment it if you don't want to do truth matching
#include "LHCbMath/BloomFilter.h"

class PrHybridSeedTrack {
public: 
  
  typedef BloomFilter<LHCb::LHCbID, 6, 98, 16384>  XHitFingerPrint;
  const XHitFingerPrint& bloomfilter() const noexcept{
    return m_ids;
  }
private:
  XHitFingerPrint m_ids;
public:
  void updateIDs() noexcept{
    m_ids.clear();
    for( auto&&hit:m_hits){
      m_ids.insert(hit->id());
    }
  }
public:
  /// Constructor with the z reference
  PrHybridSeedTrack( unsigned int zone, float zRef ) 
    : m_zone( zone ), m_zRef( zRef ){
    init();
  };
  
  PrHybridSeedTrack( unsigned int zone, float zRef, PrHits& hits )
    : m_zone( zone ), m_zRef( zRef ), m_hits( hits ){
    init();
  }; 
  
  //virtual ~PrHybridSeedTrack( ) {}; ///< Destructor
  ~PrHybridSeedTrack( ){};
  
  //Handling the hits on the track
  PrHits& hits()            { return m_hits; }
  const PrHits& hits()  const{ return m_hits;  }
  
  //Add hit
  void addHit( PrHit* hit )        { m_hits.push_back( hit );}
  void addHits( PrHits& hits ) {
    m_hits.reserve( m_hits.size() + hits.size() );
    m_hits.insert( m_hits.end(), hits.begin(), hits.end() );
  }
  //Zone (0,1) Up Down
  unsigned int zone() const { return m_zone; }
  //====================================
  //SETTERS & Getters  Genearl One
  //====================================
  //***********Track Parameters to compute distances
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
  
  void setXT1( float value){
    m_xT1 = value;
  }
  float xT1( ) const{
    return m_xT1;
  }
  void setXT3( float value){
    m_xT3 = value;
  }
  float xT3( )const{
    return m_xT3;
  }
  void setXT2(float value){
    m_xT2 = value;
  }
  float xT2()const{
    return m_xT2;
  }
  
  void setnXnY( unsigned int nx, unsigned int ny){
    m_nx = nx;
    m_ny = ny;
  }
  unsigned int nx() const{return m_nx;}
  unsigned int ny() const{return m_ny;}
  

  //validity
  void setValid( bool v )               { m_valid = v; }
  bool   valid()                 const { return m_valid; }
  //dRatio
  void setdRatio( float dRatio )       { m_dRatio = dRatio;}
  float dRatio()                 const{return m_dRatio;}
  
  // chi2
  void setChi2(float chi2, int nDoF) { m_chi2 = chi2;m_nDoF = nDoF;m_chi2DoF = m_chi2/m_nDoF; }
  float chi2()                   const { return m_chi2; }
  float chi2PerDoF()         const { return m_chi2DoF;}//m_chi2 / m_nDoF;}
  int nDoF()                        const { return m_nDoF; }

  //BackProjection
  void setX0(float X0)                  { m_X0=X0;}
  float X0()                      const { return m_X0;}

  float y( float z )          const { return (m_ay + m_by*z);}
  //Slope by
  float ySlope( )               const { return m_by; }
  
  //X at a given Z
  float x( float z )const{
    const float dz = z-m_zRef;
    return m_ax + dz * ( m_bx + dz* m_cx * (1.+m_dRatio*dz) );
  }
  void setType( int type){
    m_type = type;
  }
  int type() const{
    return m_type;
  }
  //Slope X-Z plane track at a given z
  float xSlope( float z )const{ 
    const float dz = z-m_zRef;
    return m_bx + 2.*dz*m_cx + 3.*dz*dz*m_cx*m_dRatio; //is it need?
  }
  void setCase(unsigned int Case){
    m_case=Case ;
  }
  unsigned int Case() const{
    return m_case;
  }
  //y positon on Track
  float yOnTrack( PrHit* hit ) const{ return hit->yOnTrack( m_ay , m_by ); }
  //distance from Hit of the track
  float distance( PrHit* hit ) const{ 
    float yTra = yOnTrack(hit); //is it needed for x - layers?
    return hit->x( yTra ) - x( hit->z(yTra) );
  }
  //Chi2 contribution from a single Hit
  float chi2( PrHit* hit )           const{ 
    const float d = distance( hit );
    float w = hit->w();
    return d * d * w;
  }
  //DeltaY
  float deltaY( PrHit* hit )        const{
    if ( hit->isX() ) return 0.f;
    return distance( hit ) / hit->dxDy();
  }
  void sortbyz(){
    std::sort(m_hits.begin(),m_hits.end(),[](const PrHit* hit1, const PrHit* hit2)->bool{return hit1->z()<hit2->z();});
  }
  
  struct GreaterBySize{//Before elements with lower n hits later and lower Chi2
    inline bool operator() (const PrHybridSeedTrack& lhs, const PrHybridSeedTrack& rhs) const {
      if( lhs.hits().size() == rhs.hits().size() ){
        return lhs.chi2PerDoF() > rhs.chi2PerDoF();
      }
      return lhs.hits().size() < rhs.hits().size();
      //< by default
    }
  };
  struct LowerBySize{ //Before higher number of hits and better chi2
    inline bool operator() ( const PrHybridSeedTrack& lhs, const PrHybridSeedTrack& rhs) const{
      if(lhs.hits().size() == rhs.hits().size()){
        return lhs.chi2PerDoF() < rhs.chi2PerDoF();
      }
      return lhs.hits().size() > rhs.hits().size();
    }
  };
  
  unsigned int size() const{
    return m_hits.size();
  };
  
protected:
  
private:

  void init(){
    // m_xT1 = std::numeric_limits<float>::min();
    // m_xT3 = std::numeric_limits<float>::max();
    // m_xT2 = std::numeric_limits<float>::min();
    m_valid = true;
    m_hits.reserve( 32 );
    //m_ids.reserve(12);
    m_case = -1;
    m_nx = 0;
    m_ny = 0;
    //track params
    m_ax = 0.f;
    m_bx = 0.f;
    m_cx = 0.f;
    m_by = 0.f;
    m_ay = 0.f;
    //track chi2 settings
    m_chi2 = 0.f;
    m_chi2DoF = 0.f;
    m_nDoF = -1;
    m_dRatio = 0.f;
    m_X0 = std::numeric_limits<float>::min();
  }
  //Global Private variables
private:
  float m_chi2DoF;
  int m_type;
  unsigned int m_nx;
  unsigned int m_ny;
  // int m_nUVLine;
  unsigned int m_zone;
  float       m_zRef;
  PrHits       m_hits;
  bool         m_valid;
  int          m_nDoF;
  float       m_chi2;
  float       m_ax;
  float       m_bx;
  float       m_cx;
  float       m_ay;
  float       m_by;
  float       m_dRatio;
  float       m_X0;
  float       m_xT1;
  float       m_xT2;
  float       m_xT3;
  unsigned int m_case;
};

typedef std::vector<PrHybridSeedTrack> PrHybridSeedTracks;

#endif // PRSEEDTRACK2_H
