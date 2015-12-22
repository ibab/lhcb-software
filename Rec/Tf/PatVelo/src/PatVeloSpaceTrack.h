// $Id: PatVeloSpaceTrack.h,v 1.11 2010-02-18 14:12:07 dhcroft Exp $
#ifndef TF_PATVELOSPACETRACK_H
#define TF_PATVELOSPACETRACK_H 1

// Include files
#include <cmath>

// Gaudi
// Include files
#include "Kernel/DifTraj.h"
#ifdef _WIN32
// Avoid conflict of Windows macro with std::max
  #ifndef NOMINMAX
    #define NOMINMAX
  #endif
#endif
#include "GaudiKernel/boost_allocator.h"

#include "Event/Track.h"
#include "PatVeloHit.h"
#include "CircularRangeUtils.h"
#include "PatVeloTrackTool.h"

namespace Tf {
  /** @class PatVeloSpaceTrack PatVeloSpaceTrack.h
   *
   *Local working track during Velo space tracking
   *  @author Olivier Callot
   *  @date   2005-06-10
   */
  class PatVeloSpaceTrack {
  public:
    ///< constructor taking a PatVeloTrackTool (only one allowed now)
    PatVeloSpaceTrack( const PatVeloTrackTool * const trackTool ); 

    ~PatVeloSpaceTrack( ) {}; ///< Destructor

    /// defined the "side" of the track for halfbox fitting purposes
    void setSide( PatVeloHitSide::Side side ){ m_side = side; }

    /// get the "side" of the track for halfbox fitting purposes
    PatVeloHitSide::Side side() { return m_side; }

    /// add an R coordinate and update rz parameters 
    void addRCoord( PatVeloRHit* coord );

    /// fit the RZ part of the track
    void fitRZ();

    /// add a phi coord to the track
    void addPhi( PatVeloPhiHit* phiCoord ) { m_phiCoord.push_back( phiCoord ); }

    /// set vaild flag
    void setValid( bool flag )         { m_valid = flag; }

    /// set the R zone of the track (indicates phi range)
    void setZone( int zone )           { m_zone = zone;   }

    /// set if track is forward or backwards
    void setBackward( bool flag )      { m_backward = flag; }

    /// find interpolated R at z
    double rInterpolated( double z );

    /// if this coord is on the track set the effective phi and r of the coord
    void setPhiInterpolated( PatVeloRHit* myCoord );

    /// fit the track, computes point, direction and covariance matrix.
    void fitSpaceTrack ( double stepError, 
			 bool inwardFit = true, 
			 bool beamState = true, 
			 unsigned int fullErrorPoints = 0 );

    /// set the tracks ancestor (RZ track normally)
    void setAncestor( const LHCb::Track* tr )      { m_ancestor = tr; }

    /// set the used type flag for all cluster on the track
    void tagClustersAsUsed( HitBase::EStatus );

    /// set the number of expected R clusters (from RZ Track)
    void setNVeloExpected( double nVeloExp ) { m_nVeloExpected = nVeloExp ; }

    /// add a not to fit R coordinate (avoid big chi2 for tracks crossing halves)
    void addRCoordNoFit( PatVeloRHit* coord ) { m_rCoordNoFit.push_back( coord ); }

    /// add a not to fit Phi coordinate (avoid big chi2 for tracks crossing halves)
    void addPhiCoordNoFit( PatVeloPhiHit* coord ) { m_phiCoordNoFit.push_back( coord ); }

  public:
    // inline getters
    inline double meanZ()           const { return m_meanZ; }
    inline double rPred( double z ) const { return m_pos0 + z * m_slope; }
    inline double rErr2( double z ) const { 
      return m_posErr + (z-m_meanZ)*(z-m_meanZ) * m_slopeErr;  
    }
    inline std::vector<PatVeloRHit*>* rCoords()    { return &m_rCoord; };
    inline unsigned int nbCoords()          const  { return m_rCoord.size(); }
    inline unsigned int nFitCoords() const  { 
      return m_rCoord.size()+m_phiCoord.size(); }
    inline std::vector<PatVeloPhiHit*>* phiCoords(){ return &m_phiCoord; };
    inline std::vector<PatVeloRHit*>* rCoordsNoFit()    { return &m_rCoordNoFit; };
    inline std::vector<PatVeloPhiHit*>* phiCoordsNoFit(){ return &m_phiCoordNoFit; };
    inline bool valid()          const { return m_valid; }
    inline double firstZ()       const { return m_rCoord[0]->z(); }
    inline double rSlope()       const { return m_slope; }
    inline int zone()            const { return m_zone; }
    inline bool backward()       const { return m_backward; }
    inline const Gaudi::XYZPoint&  point( ) const { return m_point; }
    inline double slopeX()       const { return m_slopeX; }
    inline double slopeY()       const { return m_slopeY; }
    inline double chi2Dof( )     const { return m_chiSqDof; }
    inline const LHCb::Track* ancestor() const { return m_ancestor; }
    inline unsigned int minRSensor() const { return m_minRSensor; }
    inline unsigned int maxRSensor() const { return m_maxRSensor; }
    inline double nVeloExpected()    const { return m_nVeloExpected; }

    inline const Gaudi::TrackSymMatrix& covariance()  const { return m_cov; }

    void setChiSqDof( double chi )     { m_chiSqDof = chi; }

    //== Auxilliary for sorting
    /// sort tracks by decreasing length, then by Z 
    class decreasingByLength  {
    public:
      bool operator() (const PatVeloSpaceTrack* first,
		       const PatVeloSpaceTrack* second) const {
	if ( 0==first  ) return true;
	if ( 0==second ) return false;

	if ( first->nbCoords() == second->nbCoords() ) {
	  return first->firstZ() > second->firstZ();
	}
	return first->nbCoords() > second->nbCoords() ;
      }
    };

    // BOOST pool allocator contructor
#ifndef _WIN32

    /// operator new
    static void* operator new ( size_t size ){
      return 
	( sizeof(PatVeloSpaceTrack) == size ?
	  boost::singleton_pool<PatVeloSpaceTrack, sizeof(PatVeloSpaceTrack)>::malloc() :
	  ::operator new(size)
          );
    }

    /// placement operator new
    /// it is needed by libstdc++ 3.2.3 (e.g. in std::vector)
    /// it is not needed in libstdc++ >= 3.4
    static void* operator new ( size_t size, void* pObj ){
      return ::operator new (size,pObj);
    }

    /// operator delete
    static void operator delete ( void* p ){
      boost::singleton_pool<PatVeloSpaceTrack, 
	sizeof(PatVeloSpaceTrack)>::is_from(p) ?
	boost::singleton_pool<PatVeloSpaceTrack,sizeof(PatVeloSpaceTrack)>::free(p) :
	::operator delete(p);
    }

    /// placement operator delete
    /// not sure if really needed, but it does not harm
    static void operator delete ( void* p, void* pObj ){
      ::operator delete (p, pObj);
    }
#endif

  protected:

    /// find the two coords closest to a z position (may be both on one side)
    template<typename HIT>
    std::pair<const HIT*,const HIT*> surroundZ(const std::vector<HIT*>& cont,double z) const;

  private:
    /// make no argument constructor inaccessible 
    PatVeloSpaceTrack( ) :
      m_angleUtils(-Gaudi::Units::pi,Gaudi::Units::pi),m_trackTool(0){}; 

    double m_s0;     ///< sum of weight of R clusters
    double m_sr;     ///< sum of ( weight of R clusters * r )
    double m_sz;     ///< sum of ( weight of R clusters * z )
    double m_srz;    ///< sum of ( weight of R clusters * r * z )
    double m_sz2;    ///< sum of ( weight of R clusters * z**2 )
    bool   m_valid;  ///< is this track vaild
    int    m_zone;   ///< the phi zone of the R clusters on the track 
    bool   m_backward; ///< track points in -z direction; if it comes from PV

    double m_pos0;   ///< radius at z = 0 for RZ part
    double m_slope;  ///< slope in RZ for RZ measurements

    double m_meanZ;  ///< average of z measurements
    double m_posErr; ///< error on m_pos0
    double m_slopeErr; ///< error on m_slope
    double m_chiSqDof; ///< chi2/ndf for the fit

    std::vector<PatVeloRHit*>   m_rCoord; ///< The R coords of the track
    std::vector<PatVeloPhiHit*> m_phiCoord; ///< The phi coords of the track

    std::vector<PatVeloRHit*>   m_rCoordNoFit; ///< The R coords of the track (not for fitting)
    std::vector<PatVeloPhiHit*> m_phiCoordNoFit; ///< The phi coords of the track (not for fitting)

    Gaudi::XYZPoint   m_point; ///< a position on the track returned by fit
    double       m_slopeX;     ///< slope dx/dz of the fitted track
    double       m_slopeY;     ///< slope dy/dz of the fitted track
    Gaudi::TrackSymMatrix m_cov; ///< covariance matrix of the fitted track

    unsigned int m_maxRSensor; ///< the biggest R sensor index on the track
    unsigned int m_minRSensor; ///< the smallest R sensor index on the track

    double m_nVeloExpected;   ///< Number of clusters expected, read from RZ 

    const LHCb::Track* m_ancestor; ///< RZ track ancestor; if one exists

    /// tool to do angle conversions to -pi -> pi
    const CircularRangeUtils<double> m_angleUtils; 

    /// TrackTool for calculating box offsets
    const PatVeloTrackTool * const m_trackTool;
    
    /// side of detector: which halfbox frame to do fit in
    PatVeloHitSide::Side m_side;
  };


  /// Function to get the two closest hits to a z position on the track
  template<typename HIT>
  std::pair<const HIT*,const HIT*> PatVeloSpaceTrack::surroundZ(const std::vector<HIT*>& cont,double z) const {
    // assume sorted, but do not know if accending or decending
    typename std::vector<HIT*>::const_iterator it1,it2,itCurr;
    itCurr = cont.begin();
    it2 = itCurr;
    double dz2 = (*it2)->z() - z;
    ++itCurr;
    it1 = itCurr;
    double dz1 = (*it1)->z() - z;
    for ( ++itCurr ; itCurr != cont.end() ; ++itCurr ){
      double dzCurrent = (*itCurr)->z() - z;      
      if ( fabs(dzCurrent) > fabs(dz2) ) break;  //both points already closer
      dz2 = dz1;      
      dz1 = dzCurrent;
      it2 = it1;
      it1 = itCurr;
      if( std::signbit(dz1) != std::signbit(dz2) ) break; // points bracket z
    }
    // in case z is the same (cheated PR only) skip to next sensor
    while( (*it1)->z() == (*it2)->z() && it1 != (cont.end()-1) ) {
      ++it1;
    }
    while( (*it1)->z() == (*it2)->z() && it2 != (cont.begin()+1) ) {
      --it2;
    }
    
    return std::pair<const HIT*,const HIT*>(*it1,*it2);
  }

  /// OK make a little struture to hold the accumulating values
  /// use linear regression in each frame seperatly 
  namespace PatVeloSpaceTrackLocal{
    struct FrameParam {    
      FrameParam () : w(0.), x(0.), z(0.), xz(0.), z2(0.) {};    
      double w;      ///< sum of weights of points
      double x;      ///< sum of (weight*value)
      double z;      ///< sum of (weight*z)
      double xz;     ///< sum of (weight*value*z)   
      double z2;     ///< sum of (weight*z^2)
      double x0;     ///< intercept [ x at z=0 ]
      double dxdz;   ///< slope
      double denom;  ///< denominator for several calculations
      /// function to increment values
      inline void increment(double iw, double ix, double iz){
        w += iw;
        x += iw * ix;
        z += iw * iz;
        xz += iw * ix * iz;
        z2 += iw * iz * iz;
      }
      /// function to set xz intercept and slope of xz
      void fitLinear(){
        denom = z2 * w - z * z ;
        if (  fabs(denom) < 1e-10 ) denom = 1;
        x0  = ( x * z2 - xz * z  ) / denom ;
        dxdz= ( xz * w  - x * z  ) / denom ;
      }
      /// get errors on value^2
      inline double errorX2(double zUse){
        return ( z2 - 2 * z * zUse +  w * zUse * zUse ) / denom;
      }
      /// get errors on value*z
      inline double errorXZ(double zUse){
        return - ( z  -   w*zUse ) / denom ;
      }
      /// get the errors on the square of dx/dz
      inline double errorsDxDz2(){
        return w / denom ;
      }
      /// get the predicted value of x at z
      inline double pred(double z){
        fitLinear();
        return x0 + dxdz * z;
      }
      /// get the errors on on the predicted value of x squared
      inline double predErr2(double z){
        return errorX2(z) + std::pow(z*errorsDxDz2(),2);
      }
    };
  }
}
#endif // TF_PATVELOSPACETRACK_H
