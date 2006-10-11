// $Id: SolidPolycone.cpp,v 1.13 2006-10-11 15:02:58 cattanem Exp $
// ============================================================================
#include "DetDesc/SolidPolycone.h"

// GaudiKernel
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/MsgStream.h"
// DetDesc
#include "DetDesc/SolidFactory.h"
#include "DetDesc/SolidCons.h"
#include "DetDesc/SolidTubs.h"
#include "DetDesc/SolidTicks.h" 
#include "DetDesc/SolidException.h"


//=============================================================================
/** @file SolidPolycone.cpp 
 *
 * Implementation file for class : SolidPolycone
 * 
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru
 * @date 24/10/2001 
 *
 */
//=============================================================================

//=============================================================================
/** Standard constructor
 *  @param Name          the name of the polycone
 *  @param Params        vector of "triplets" (z,(rMax,rMin))
 *  @param StartPhiAngle the azimuthal angle phi at which polycone "begins"
 *  @param DeltaPhiAngle the opening angle 
 */
//=============================================================================
SolidPolycone::SolidPolycone( const std::string&             Name          ,
                              const SolidPolycone::Triplets& Params        , 
                              const double                   StartPhiAngle ,
                              const double                   DeltaPhiAngle )
  : SolidBase       ( Name          ) 
  , m_triplets      ( Params        )
  , m_startPhiAngle ( StartPhiAngle ) 
  , m_deltaPhiAngle ( DeltaPhiAngle )
{
  /// check for parameters
  if( number() < 2 ) 
    { throw SolidException("SolidPolycone:: wrong number of sections!");}
  /// check for Phi
  if(    0.0 * Gaudi::Units::degree > startPhiAngle()                 ) 
    { throw SolidException("SolidPolycone ::StartPhiAngle < 0 degree!"    );}
  if(    0.0 * Gaudi::Units::degree > deltaPhiAngle()                 ) 
    { throw SolidException("SolidPolycone ::DeltaPhiAngle < 0 degree!"    );}
  if(  360.0 * Gaudi::Units::degree < startPhiAngle()+deltaPhiAngle() ) 
    { throw SolidException("SolidPolycone ::StartPhiAngle+DeltaPhiAngle>2pi");}
  /// sort the triplets
  std::sort( m_triplets.begin() , m_triplets.end() );
  ///
  for( unsigned int i = 0 ; i < number() ; ++i )
    {
      if( RMin( i ) < 0 ) 
        { throw SolidException("SolidPolycone ::rMin < 0 !");}
      if( !( RMin(i) < RMax( i ) ) )        
        { throw SolidException("SolidPolycone :: !(rMin < rMax) !");}      
    }
  // set bounding parameters 
  setBP();
};

// ============================================================================
/// destructor 
// ============================================================================
SolidPolycone::~SolidPolycone() {};

// ============================================================================
/** set parameters for bounding solids (box, sphere and cylinder)
 *  @return status code 
 */
// ============================================================================
StatusCode SolidPolycone::setBP()
{
  setZMin   ( m_triplets.front().first                  ) ;
  setZMax   ( m_triplets.front().first                  ) ;
  setRhoMax ( m_triplets.front().second.second          ) ;
  setRMax   ( sqrt( rMax() * rMax() + zMax() * zMax() ) ) ;
  
  double rhoMin =  1.e+23 ;
  
  for( Triplets::const_iterator triplet = m_triplets.begin() ;
       m_triplets.end() != triplet ; ++triplet )
    {
      const double z      = triplet->first                   ;
      setZMin   (  z < zMin () ? z : zMin () )  ;
      setZMax   (  z > zMax () ? z : zMax () )  ;
      
      if( rhoMin > triplet->second.first  ) 
        { rhoMin = triplet->second.first  ; }
      
      if( rhoMin > triplet->second.second ) 
        { rhoMin = triplet->second.second ; }
      
      if( rhoMax() < triplet->second.first   ) 
        { setRhoMax( triplet->second.first  ) ; }
      
      if( rhoMax() < triplet->second.second  ) 
        { setRhoMax( triplet->second.second ) ; }
      
      const double rmax   = sqrt( rhoMax() * rhoMax() + z * z )  ;
      setRMax   ( rmax   > rMax   () ? rmax   : rMax   () )  ;
      
    }
  
  typedef std::vector<double> Values ;
  
  const double phi1   = startPhiAngle   ()                      ;
  const double phi2   = startPhiAngle   () + deltaPhiAngle   () ;
  
  
  { // evaluate xmin & xmax
    Values values ;
    
    // regular cases 
    values.push_back( rhoMax () * cos ( phi1 ) );
    values.push_back( rhoMax () * cos ( phi2 ) );
    values.push_back( rhoMin    * cos ( phi1 ) );
    values.push_back( rhoMin    * cos ( phi2 ) );
    
    // special cases 
    if( phi1 <=    0*Gaudi::Units::degree &&    0*Gaudi::Units::degree <= phi2 ) 
      { 
        values.push_back (   rhoMax () ) ;
        values.push_back (   rhoMin    )  ;
      }
    if( phi1 <=  360*Gaudi::Units::degree &&  360*Gaudi::Units::degree <= phi2 ) 
      { 
        values.push_back (   rhoMax () ) ; 
        values.push_back (   rhoMin    ) ; 
      }
    
    // special cases 
    if( phi1 <=  180*Gaudi::Units::degree &&  180*Gaudi::Units::degree <= phi2 ) 
      { 
        values.push_back ( - rhoMax () ) ; 
        values.push_back ( - rhoMin    ) ; 
      }
    if( phi1 <= -180*Gaudi::Units::degree && -180*Gaudi::Units::degree <= phi2 ) 
      { 
        values.push_back ( - rhoMax () ) ; 
        values.push_back ( - rhoMin    ) ; 
      }
    
    setXMax ( *std::max_element ( values.begin () , values.end () ) ) ;
    setXMin ( *std::min_element ( values.begin () , values.end () ) ) ;
    
  }

  { // evaluate ymin & ymax
    Values values ;
    
    // regular cases 
    values.push_back( rhoMax () * sin ( phi1 ) );
    values.push_back( rhoMax () * sin ( phi2 ) );
    values.push_back( rhoMin    * sin ( phi1 ) );
    values.push_back( rhoMin    * sin ( phi2 ) );
    
    // special cases 
    if( phi1 <=   90*Gaudi::Units::degree &&   90*Gaudi::Units::degree <= phi2 ) 
      { 
        values.push_back (   rhoMax () ) ; 
        values.push_back (   rhoMin    ) ; 
      }
    
    // special cases 
    if( phi1 <=  -90*Gaudi::Units::degree &&  -90*Gaudi::Units::degree <= phi2 ) 
      { 
        values.push_back ( - rhoMax () ) ; 
        values.push_back ( - rhoMin    ) ; 
      }
    if( phi1 <=  270*Gaudi::Units::degree &&  270*Gaudi::Units::degree <= phi2 ) 
      { 
        values.push_back ( - rhoMax () ) ; 
        values.push_back ( - rhoMin    ) ; 
      }
    
    setYMax ( *std::max_element ( values.begin () , values.end () ) ) ;
    setYMin ( *std::min_element ( values.begin () , values.end () ) ) ;
    
  }

  ///
  return checkBP();
};
// ============================================================================


// ============================================================================
/** default protected  coinstructor 
 *  @param Name name of conical tube segment  
 */
// ============================================================================
SolidPolycone::SolidPolycone( const std::string& Name )
  ///
  : SolidBase       ( Name         )
  , m_triplets      ()
  , m_startPhiAngle ( 0            ) 
  , m_deltaPhiAngle ( 360 * Gaudi::Units::degree )
{};

// ============================================================================
/** check for the given point (local frame)
 *  @param point point (in local reference system)
 *  @return true ifpoint is inside the solid
 */
// ============================================================================
bool SolidPolycone::isInside( const Gaudi::XYZPoint   & point ) const 
{
  return isInsideImpl(point);
}
// ============================================================================
bool SolidPolycone::isInside( const Gaudi::Polar3DPoint& point ) const 
{
  return isInsideImpl(point);
}
// ============================================================================
bool SolidPolycone::isInside( const Gaudi::RhoZPhiPoint   & point ) const 
{
  return isInsideImpl(point);
}
// ============================================================================
template <class aPoint>
bool SolidPolycone::isInsideImpl (  const aPoint& point ) const
{
  /// look for concrete z-segment
  if( point.z() > triplets().back().first ) { return false; }
  Iterator it = 
    std::find_if( begin () , 
                  end   () ,
                  std::bind2nd( CmpZ() , point.z() ) ) ;
  /// outside!
  if( begin() == it || end() == it        ) { return false; }
  /// check for phi
  bool   phiok = false ;
  double phi = point.phi(); ///  [-180, 180]
  if( startPhiAngle ()                    <= phi && 
      startPhiAngle () + deltaPhiAngle () >= phi     ) { phiok = true ;}
  else 
    {
      phi += 360 * Gaudi::Units::degree ;
      if( startPhiAngle ()                    <= phi && 
          startPhiAngle () + deltaPhiAngle () >= phi ) { phiok = true ; }
    }
  if( !phiok ) { return false ; }
  /// check for radius
  const double       rho  = std::sqrt( point.perp2() );
  const unsigned int i2   = it - begin();
  const unsigned int i1   = i2 - 1 ;
  ///
  const double       dzi  = 1.0 / ( z( i2 ) - z ( i1 ) ) ;
  /// check for outer radius 
  if(  rho > RMax( i1 )  +  
       ( point.z() - z( i1 ) ) * ( RMax( i2 ) - RMax( i1 ) ) * dzi ) 
    { return false; }
  /// check for inner radius  
  if( 0 < RMin( i1 )  &&  0 < RMin( i2 ) )
    {
      if(  rho < RMin( i1 )  +  
           ( point.z() - z( i1 ) ) * ( RMin( i2 ) - RMin( i1 ) ) * dzi ) 
        { return false; }
    }
  ///
  return true;
};

// ============================================================================/
/** -# retrieve the pointer to "simplified" solid - "cover"
 *  -# implementation of ISolid abstract interface 
 *  @see ISolid 
 *  @return pointer to "simplified" solid - "cover"
 */
// ============================================================================
const ISolid* SolidPolycone::cover () const 
{
  /// cover is calculated already 
  if( 0 != m_cover ) { return m_cover; } 
  //  
  ISolid* cov = 0 ;
  if( 0.0 != startPhiAngle() || 360 * Gaudi::Units::degree != deltaPhiAngle() )
    { cov = new SolidPolycone( "Cover for " + name () , triplets() ); }  
  else
    {
      double rmxmx = RMin( 0 ) ;
      double rmnmn = RMax( 0 ) ;
      double zmxmx = 0         ;
      ///
      for( unsigned int i = 0 ; i < number() ; ++i )
        {
          if( RMax  ( i )   > rmxmx ) { rmxmx = RMax   ( i )   ; }
          if( RMin  ( i )   < rmnmn ) { rmnmn = RMin   ( i )   ; }
          if( fabs( z(i) )  > zmxmx ) { zmxmx = fabs( z ( i ) ) ; }
        }
      cov = new SolidTubs( "Cover for " + name () , zmxmx , rmxmx , rmnmn ) ;
    }
  ///
  if( 0 == cov ) { return this ; }
  ///
  m_cover = cov ;
  ///
  return m_cover;
};

// ============================================================================
/** - printout to STD/STL stream    
 *  - implementation  of ISolid abstract interface 
 *  - reimplementation of SolidBase::printOut( std::ostream& )
 *  @see SolidBase 
 *  @see ISolid 
 *  @param stream STD/STL stream
 *  @return reference to the stream 
 */
// ============================================================================
std::ostream&  SolidPolycone::printOut ( std::ostream& os ) const
{
  /// serialize the base class 
  SolidBase::printOut( os );
  /// serialize the members 
  os << "["
     << " number of planes: " << number() ;
  for( unsigned int i = 0 ; i < number() ; ++i ) 
    {
      os << "(Z[mm]="    << z    ( i ) / Gaudi::Units::millimeter 
         << ",Rmax[mm]=" << RMax ( i ) / Gaudi::Units::millimeter 
         << ",Rmin[mm]=" << RMax ( i ) / Gaudi::Units::millimeter 
         << ")";
    }
  ///
  if( 0   * Gaudi::Units::degree != startPhiAngle() || 
      360 * Gaudi::Units::degree != deltaPhiAngle()  ) 
    {
      os << " startPhiAngle[deg]" << startPhiAngle() / Gaudi::Units::degree ;
      os << " deltaPhiAngle[deg]" << startPhiAngle() / Gaudi::Units::degree ;
    }
  ///
  return os ;
  ///
};

// ============================================================================
/** printout to Gaudi  stream
 *  @param os Gaudi stream
 *  @return reference to the stream
 */
// ============================================================================
MsgStream&  SolidPolycone::printOut ( MsgStream& os ) const
{
  /// serialize the base class 
  SolidBase::printOut( os );
  /// serialize the members 
  os << "["
     << " number of planes: " << number() ;
  for( unsigned int i = 0 ; i < number() ; ++i ) 
    {
      os << "(Z[mm]="    << z    ( i ) / Gaudi::Units::millimeter 
         << ",Rmax[mm]=" << RMax ( i ) / Gaudi::Units::millimeter 
         << ",Rmin[mm]=" << RMax ( i ) / Gaudi::Units::millimeter 
         << ")";
    }
  ///
  if( 0   * Gaudi::Units::degree != startPhiAngle() || 
      360 * Gaudi::Units::degree != deltaPhiAngle()  ) 
    {
      os << " startPhiAngle[deg]" << startPhiAngle() / Gaudi::Units::degree ;
      os << " deltaPhiAngle[deg]" << startPhiAngle() / Gaudi::Units::degree ;
    }
  ///
  return os ;
  ///
};

// ============================================================================
/** - calculate the intersection points("ticks") of the solid objects 
 *    with given line. 
 *  -# Line is parametrized with parameter \a t :
 *     \f$ \vec{x}(t) = \vec{p} + t \times \vec{v} \f$ 
 *      - \f$ \vec{p} \f$ is a point on the line 
 *      - \f$ \vec{v} \f$ is a vector along the line  
 *  -# \a tick is just a value of parameter \a t, at which the
 *    intersection of the solid and the line occurs
 *  -# both  \a Point  (\f$\vec{p}\f$) and \a Vector  
 *    (\f$\vec{v}\f$) are defined in local reference system 
 *   of the solid 
 *  - implementation of ISolid abstract interface  
 *  @see ISolid 
 *  @param Point initial point for the line
 *  @param Vector vector along the line
 *  @param ticks output container of "Ticks"
 *  @return the number of intersection points
 */
// ============================================================================
unsigned int SolidPolycone::intersectionTicks( const Gaudi::XYZPoint& point,
                                               const Gaudi::XYZVector& vect,
                                               ISolid::Ticks&    ticks ) const 
{
  return intersectionTicksImpl(point, vect, ticks);
};
// ============================================================================
unsigned int 
SolidPolycone::intersectionTicks( const Gaudi::Polar3DPoint& point,
                                  const Gaudi::Polar3DVector& vect,
                                  ISolid::Ticks&    ticks ) const 
{
  return intersectionTicksImpl(point, vect, ticks);
};
// ============================================================================
unsigned int 
SolidPolycone::intersectionTicks( const Gaudi::RhoZPhiPoint& point,
                                  const Gaudi::RhoZPhiVector& vect, 
                                  ISolid::Ticks&    ticks ) const 
{
  return intersectionTicksImpl(point, vect, ticks);
};
// ============================================================================
template <class aPoint, class aVector>
unsigned int SolidPolycone::intersectionTicksImpl ( const aPoint & Point,
                                                    const aVector& Vector,
                                                    ISolid::Ticks& ticks  
                                                    ) const 
{
  /// line with null direction vector is not able to intersect any solid!
  if( Vector.mag2() <= 0 ) { return 0 ; }
  /// intersect with phi 
  if( ( 0            != startPhiAngle() ) || 
      ( 360 * Gaudi::Units::degree != deltaPhiAngle() ) )
    {
      SolidTicks::LineIntersectsThePhi( Point                             , 
                                        Vector                            , 
                                        startPhiAngle()                   , 
                                        std::back_inserter( ticks )       ); 
      SolidTicks::LineIntersectsThePhi( Point                             ,
                                        Vector                            , 
                                        startPhiAngle() + deltaPhiAngle() , 
                                        std::back_inserter( ticks )       ); 
    }
  /// intersect with first z-plane 
  SolidTicks::LineIntersectsTheZ( Point        , 
                                  Vector       , 
                                  z( 0 )       , 
                                  std::back_inserter( ticks ) ); 
  /// loop over all other surfaces 
  for( unsigned int i = 1 ; i < number() ; ++i )
    {
      /// intersect with outer conical surface 
      SolidTicks::LineIntersectsTheCone( Point                       , 
                                         Vector                      , 
                                         RMax ( i - 1 )              , 
                                         RMax ( i     )              , 
                                         z    ( i - 1 )              , 
                                         z    ( i     )              , 
                                         std::back_inserter( ticks ) );   
      /// intersect with inner conical surface 
      if( ( 0 < RMin( i - 1 ) ) || ( 0 < RMin ( i ) ) ) 
        {
          SolidTicks::LineIntersectsTheCone( Point                       , 
                                             Vector                      , 
                                             RMin ( i - 1 )              , 
                                             RMin ( i     )              , 
                                             z    ( i - 1 )              , 
                                             z    ( i     )              , 
                                             std::back_inserter( ticks ) );   
          
        }      
      /// intersect with z-plane 
      SolidTicks::LineIntersectsTheZ( Point        , 
                                      Vector       , 
                                      z( i )       , 
                                      std::back_inserter( ticks ) ); 
    }
  ///
  /// sort and remove adjancent and some EXTRA ticks and return 
  return SolidTicks::RemoveAdjancentTicks( ticks , Point , Vector , *this );  
  ///  
};

//=============================================================================
// The end 
//=============================================================================
