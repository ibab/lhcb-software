// $Id: SolidPolycone.cpp,v 1.19 2009-12-10 08:32:58 cattanem Exp $
// ============================================================================
#include "DetDesc/SolidPolycone.h"

// GaudiKernel
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/MsgStream.h"
// DetDesc
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
  ///
  checkTickContainerCapacity() ;
}

// ============================================================================
/// destructor 
// ============================================================================
SolidPolycone::~SolidPolycone() {}

// ============================================================================
/// set parameters for bounding solids (box, sphere and cylinder)
// ============================================================================
void SolidPolycone::setBP()
{
  setZMin   ( m_triplets.front().first                  ) ;
  setZMax   ( m_triplets.front().first                  ) ;
  setRhoMax ( m_triplets.front().second.second          ) ;
  setRMax   ( sqrt( rhoMax() * rhoMax() + zMax() * zMax() ) ) ;
  
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
  checkBP();
}
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
{}

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
  /// check z
  if( point.z() < triplets().front().first ||
      point.z() > triplets().back().first ) { return false; }

  /// check phi
  if( !insidePhi( point.phi() ) ) { return false ; }

  /// check radius
  Iterator it = std::find_if( begin() , end() , 
                              [&](const Triplet& i) 
                              { return i.first >= point.z(); } );

  /// outside!
  if( begin() == it || end() == it ) { return false; }

  const double rho  = std::sqrt( point.perp2() );

  const unsigned int i2   = it - begin();
  const unsigned int i1   = i2 - 1 ;
  const double zfrac = (point.z() - z(i1))/(z(i2) - z(i1)) ;
  const double rmax  = (1-zfrac)*RMax( i1 ) + zfrac*RMax( i2 ) ;
  const double rmin  = (1-zfrac)*RMin( i1 ) + zfrac*RMin( i2 ) ;
  if( rho > rmax || rho < rmin ) return false ;
  
  return true;
}

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
}

// ============================================================================
/** - printout to STD/STL stream    
 *  - implementation  of ISolid abstract interface 
 *  - reimplementation of SolidBase::printOut( std::ostream& )
 *  @see SolidBase 
 *  @see ISolid 
 *  @param  os STD/STL stream
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
}

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
}

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
unsigned int SolidPolycone::intersectionTicks( const Gaudi::XYZPoint&  Point,
                                               const Gaudi::XYZVector& Vector,
                                               ISolid::Ticks&          ticks ) const 
{
  return intersectionTicksImpl(Point, Vector, ticks);
}
// ============================================================================
unsigned int 
SolidPolycone::intersectionTicks( const Gaudi::Polar3DPoint&  Point,
                                  const Gaudi::Polar3DVector& Vector,
                                  ISolid::Ticks&              ticks ) const 
{
  return intersectionTicksImpl(Point, Vector, ticks);
}
// ============================================================================
unsigned int 
SolidPolycone::intersectionTicks( const Gaudi::RhoZPhiPoint&  Point,
                                  const Gaudi::RhoZPhiVector& Vector, 
                                  ISolid::Ticks&              ticks ) const 
{
  return intersectionTicksImpl(Point, Vector, ticks);
}
// ============================================================================
template <class aPoint, class aVector>
unsigned int SolidPolycone::intersectionTicksImpl ( const aPoint & Point,
                                                    const aVector& Vector,
                                                    ISolid::Ticks& ticks  
                                                    ) const 
{ 
  // check tick capacity
  checkTickContainerCapacity() ;
  
  /// clear the container 
  ticks.clear() ;
  
/// line with null direction vector is not able to intersect any solid!
  if( Vector.mag2() <= 0 ) { return 0 ; }

  // bail out if we don't cross the envelope
  if(!crossBCylinder( Point , Vector )) { return 0 ; }

  // intersect with first z-planes
  ISolid::Ticks tmpticks ;
  tmpticks.clear() ; 
  if(SolidTicks::LineIntersectsTheZ( Point, Vector, z(0), std::back_inserter( tmpticks ))) {
    double tick = tmpticks.front() ;
    double x = Point.x() + tick * Vector.x() ;
    double y = Point.y() + tick  * Vector.y() ;
    double r = sqrt(x*x+y*y) ;
    if(RMin(0)<=r && r<=RMax(0) && (noPhiGap() || insidePhi( atan2(y,x) ) ) )
      ticks.push_back(tick) ;
  }

  // intersect with last z-plane
  tmpticks.clear() ; 
  if(SolidTicks::LineIntersectsTheZ( Point, Vector, z(number()-1), std::back_inserter( tmpticks ))) {
    double tick = tmpticks.front() ;
    double x = Point.x() + tick * Vector.x() ;
    double y = Point.y() + tick  * Vector.y() ;
    double r = sqrt(x*x+y*y) ;
    if(RMin(number()-1)<=r && r<=RMax(number()-1) && (noPhiGap() || insidePhi( atan2(y,x) ) ) )
      ticks.push_back(tick) ;
  }
  
  if( !noPhiGap() ) {
    // intersect with phi planes
    tmpticks.clear() ; 
    SolidTicks::LineIntersectsThePhi( Point,Vector,startPhiAngle(), std::back_inserter( tmpticks ) );  
    //if( deltaPhiAngle() != M_PI )
    SolidTicks::LineIntersectsThePhi( Point,Vector,startPhiAngle() + deltaPhiAngle(), std::back_inserter( tmpticks ));
    
    // check that we are anywhere inside this cylinder
    for( ISolid::Ticks::const_iterator it = tmpticks.begin(); it != tmpticks.end(); ++it) {
      double thisz = Point.z() + *it * Vector.z() ;
      if( z(0) <=thisz && thisz<= z(number()-1) ) {
	Triplets::size_type i = index(thisz) ;
	//std::cout << "in phi: " << thisz << " " << z(i) << " " << z(i+1) << std::endl ;
	assert(i < number() - 1 ) ;
	double zfrac = (thisz - z(i))/(z(i+1) - z(i)) ;
	double x = Point.x() + *it * Vector.x() ;
	double y = Point.y() + *it * Vector.y() ;
	double r = sqrt(x*x+y*y) ;
	if( r >= ((1-zfrac)*RMin(i) + zfrac*RMin(i+1)) && 
	    r <= ((1-zfrac)*RMax(i) + zfrac*RMax(i+1)) )
	  ticks.push_back(*it) ;
      }
    }
  }
  
  for( unsigned int i = 1 ; i < number() ; ++i ) 
    if( z(i) > z (i-1) ) {
      // intersect with the cones
      tmpticks.clear() ;
      // intersect with outer conical surface 
      SolidTicks::LineIntersectsTheCone( Point , Vector, 
					 RMax ( i - 1 ), RMax ( i ), 
					 z    ( i - 1 ), z    ( i ), 
					 std::back_inserter( tmpticks ) );   
      // intersect with inner conical surface 
      if( ( 0 < RMin( i - 1 ) ) || ( 0 < RMin ( i ) ) ) 
	SolidTicks::LineIntersectsTheCone( Point, Vector, 
					   RMin ( i - 1 ), RMin ( i ), 
					   z    ( i - 1 ), z    ( i ), 
					   std::back_inserter( tmpticks ) );   
      // check that we are in the right z and phi range
      for( ISolid::Ticks::const_iterator it = tmpticks.begin(); it != tmpticks.end(); ++it) {
	double thisz = Point.z() + *it * Vector.z() ;
	if( z(i-1) <= thisz && thisz <= z(i) &&
	    (noPhiGap() ||
	     insidePhi(atan2( Point.y() + *it * Vector.y(), Point.x() + *it * Vector.x())) ) )
	  ticks.push_back(*it) ;
      }
    } else {
      // double z-plane: intersect with this z-plane
      tmpticks.clear() ; 
      if(SolidTicks::LineIntersectsTheZ( Point, Vector, z(i), std::back_inserter( tmpticks ))) {
	double tick = tmpticks.front() ;
	double x = Point.x() + tick * Vector.x() ;
	double y = Point.y() + tick  * Vector.y() ;
	double r = sqrt(x*x+y*y) ;
	if( noPhiGap() || insidePhi( atan2(y,x) ) ) {
	  // take an excusive-OR
	  bool insideFirst  = RMin(i-1)<=r && r<=RMax(i-1) ;
	  bool insideSecond = RMin(i)<=r && r<=RMax(i) ;
	  if( (insideFirst && !insideSecond) || (insideSecond && !insideFirst) )
	    ticks.push_back(tick) ;
	}
      }
    }
  
  // sort the ticks and solve eventual problems
  std::sort(ticks.begin(),ticks.end()) ;
  return SolidTicks::RemoveAdjacentTicksFast(ticks , Point, Vector, *this );
}

//=============================================================================
/** static function to generate triplets for a cone */
SolidPolycone::Triplets SolidPolycone::makeTriplets(double ZHalfLength        , 
						    double OuterRadiusMinusZ  ,
						    double OuterRadiusPlusZ   ,
						    double InnerRadiusMinusZ  , 
						    double InnerRadiusPlusZ )
{
  Triplets triplets ;
  triplets.emplace_back( -ZHalfLength, std::make_pair(InnerRadiusMinusZ,OuterRadiusMinusZ) ) ;
  triplets.emplace_back(  ZHalfLength, std::make_pair(InnerRadiusPlusZ, OuterRadiusPlusZ)  ) ;
  return triplets ;
}

//=============================================================================
// The end 
//=============================================================================
