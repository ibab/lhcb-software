// $Id: SolidSphere.cpp,v 1.18 2006-10-11 15:02:58 cattanem Exp $ 
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
// STD & STL 
#include <algorithm>
// Units
#include "GaudiKernel/SystemOfUnits.h" 
// LHCbDefinitions 
#include "Kernel/Point3DTypes.h" 
#include "Kernel/Vector3DTypes.h" 
/// DetDesc 
#include "DetDesc/DetDesc.h" 
#include "DetDesc/SolidSphere.h" 
#include "DetDesc/SolidBox.h" 
#include "DetDesc/SolidTicks.h" 
#include "DetDesc/SolidException.h" 

// ============================================================================
/** @file SolidSphere.cpp 
 * 
 *  implementation of class SolidSphere 
 * 
 *  @author Vanya Belyaev  Ivan.Belyaev@itep.ru
 *  @date xx/xx/xx  
 */
// ============================================================================

// ============================================================================ 
/** constructor 
 *  @param name             name of sphere segment 
 *  @param OuterRadius      outer radius of sphere segement        
 *  @param InsideRadius     inner  radius of sphere segement
 *  @param StartPhiAngle    start phi angle 
 *  @param DeltaPhiAngle    delta phi angle 
 *  @param StartThetaAngle  start theta angle 
 *  @param DeltaThetaAngle  delta theta angle 
 *  @param CoverModel       covering model 
 */
// ============================================================================ 
SolidSphere::SolidSphere
( const std::string & name             ,
  const double        OuterRadius      ,
  const double        InsideRadius     , 
  const double        StartPhiAngle    , 
  const double        DeltaPhiAngle    ,
  const double        StartThetaAngle  , 
  const double        DeltaThetaAngle  ,
  const int           CoverModel       )
  : SolidBase                ( name            ) 
  , m_sphere_outerR2         ( 0               )  
  , m_sphere_insideR2        ( 0               ) 
  , m_sphere_startPhiAngle   ( StartPhiAngle   ) 
  , m_sphere_deltaPhiAngle   ( DeltaPhiAngle   ) 
  , m_sphere_startThetaAngle ( StartThetaAngle ) 
  , m_sphere_deltaThetaAngle ( DeltaThetaAngle )
  , m_sphere_coverModel      (   CoverModel    )  
  , m_sphere_outerR          ( OuterRadius     )
  , m_sphere_insideR         ( InsideRadius    )
{
  if( 0 >= OuterRadius ) 
    { throw SolidException("SolidSphere::OuterRadius  is not positive!"); } 
  if( 0 >  InsideRadius ) 
    { throw SolidException("SolidSphere::InsideRadius is negative!    "); }
  if( InsideRadius >= OuterRadius ) 
    { throw SolidException("SolidSphere::InsideRadius>=OuterRadius    "); }
  ///
  m_sphere_outerR2  = OuterRadius  * OuterRadius  ;
  m_sphere_insideR2 = InsideRadius * InsideRadius ;
  ///
  if( -180.0 * Gaudi::Units::degree > StartPhiAngle )
    { throw SolidException("SolidSphere::StartPhiAngle < -180 degree !"); }
  if(  360.0 * Gaudi::Units::degree < StartPhiAngle )
    { throw SolidException("SolidSphere::StartPhiAngle >  360 degree !"); }
  if(    0.0 * Gaudi::Units::degree > DeltaPhiAngle )
    { throw SolidException("SolidSphere::DeltaPhiAngle <    0 degree !"); }
  if(  360.0 * Gaudi::Units::degree < StartPhiAngle+DeltaPhiAngle )
    { throw SolidException("SolidSphere::StartPhiAngle+DeltaPhiAngle>2pi"); }
  if(    0.0 * Gaudi::Units::degree > StartThetaAngle )
    { throw SolidException("SolidSphere::StartThetaAngle < 0 degree !"); }
  if(  180.0 * Gaudi::Units::degree < StartThetaAngle )
    { throw SolidException("SolidSphere::StartThetaAngle >  180 degree !"); }
  if(    0.0 * Gaudi::Units::degree > DeltaThetaAngle )
    { throw SolidException("SolidSphere::DeltaThetaAngle <    0 degree !"); }
  if(  180.0 * Gaudi::Units::degree < StartThetaAngle+DeltaThetaAngle )
    { throw SolidException("SolidSphere::StartThetaAngle+DeltaThetaAngle>pi");}
  /// 
  m_noPhiGap   = true ;
  if(   0*Gaudi::Units::degree != startPhiAngle  () ) { m_noPhiGap   = false ; }
  if( 360*Gaudi::Units::degree != deltaPhiAngle  () ) { m_noPhiGap   = false ; }
  m_noThetaGap = true ;
  if(   0*Gaudi::Units::degree != startThetaAngle() ) { m_noThetaGap = false ; }
  if( 180*Gaudi::Units::degree != deltaThetaAngle() ) { m_noThetaGap = false ; }  
  /// set bounding parameters 
  setBP();
  
};  

// ===========================================================================
/// destructor 
// ===========================================================================
SolidSphere::~SolidSphere() {};

// ============================================================================
/** set parameters for bounding solids (box, sphere and cylinder)
 *  @return status code 
 */
// ============================================================================
StatusCode SolidSphere::setBP() 
{
  // set bounding parameters of SolidBase class
  setRMax ( outerRadius() );
  
  typedef std::vector<double> Values ;
  
  const double theta1 = startThetaAngle ()                      ;
  const double theta2 = startThetaAngle () + deltaThetaAngle () ;
  const double phi1   = startPhiAngle   ()                      ;
  const double phi2   = startPhiAngle   () + deltaPhiAngle   () ;
  
  { // evaluate ZMin and ZMax
    Values values ;
    
    values.push_back ( outerRadius  () * cos ( theta1 ) ) ;
    values.push_back ( insideRadius () * cos ( theta1 ) ) ;
    values.push_back ( outerRadius  () * cos ( theta2 ) ) ;
    values.push_back ( insideRadius () * cos ( theta2 ) ) ;
    //
    setZMax ( *std::max_element ( values.begin () , values.end () ) ) ;
    setZMin ( *std::min_element ( values.begin () , values.end () ) ) ;    
  };
  
  
  double rhoMin = 0.0 ;
  { // evaluate rho max 
    Values values ;
    
    // regular case
    values.push_back( rMax         () * sin ( theta1 ) ) ;
    values.push_back( rMax         () * sin ( theta2 ) ) ;      
    
    // needed for evaluation of RhoMin
    values.push_back( insideRadius () * sin ( theta1 ) ) ;
    values.push_back( insideRadius () * sin ( theta2 ) ) ;      
    
    // special cases 
    if ( theta1 <= 90*Gaudi::Units::degree && 90*Gaudi::Units::degree <= theta2 ) 
      { values.push_back( rMax () ) ; }
    
    setRhoMax ( *std::max_element ( values.begin() , values.end() ) ) ;
    rhoMin =  ( *std::min_element ( values.begin() , values.end() ) ) ;
  };
  
  
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
// ===========================================================================


// ===========================================================================
/** protected constructor 
 *  @param name name of the sphere segment 
 */
// ===========================================================================
SolidSphere::SolidSphere( const std::string& name )
  : SolidBase                ( name             ) 
  , m_sphere_outerR2         ( 1000000.0        )  
  , m_sphere_insideR2        ( 0.0              ) 
  , m_sphere_startPhiAngle   ( 0.0              ) 
  , m_sphere_deltaPhiAngle   ( 360.0 * Gaudi::Units::degree ) 
  , m_sphere_startThetaAngle ( 0.0              ) 
  , m_sphere_deltaThetaAngle ( 180.0 *Gaudi::Units:: degree )
  , m_sphere_coverModel      (       0          ) 
  , m_noPhiGap               ( true )
  , m_noThetaGap             ( true )
{};

// ===========================================================================
/** - check for the given 3D-point. 
 *    Point coordinated are in the local reference 
 *    frame of the solid.   
 *  - implementation of ISolid absstract interface  
 *  @see ISolid 
 *  @param point point (in local reference system of the solid)
 *  @return true if the point is inside the solid
 */
// ===========================================================================
bool SolidSphere::isInside( const Gaudi::XYZPoint   & point ) const 
{
  return isInsideImpl(point);
}
// ============================================================================
bool SolidSphere::isInside( const Gaudi::Polar3DPoint& point ) const 
{
  return isInsideImpl(point);
}
// ============================================================================
bool SolidSphere::isInside( const Gaudi::RhoZPhiPoint   & point ) const 
{
  return isInsideImpl(point);
}
// ============================================================================
template <class aPoint>
bool  SolidSphere::isInsideImpl( const aPoint & point) const
{
  // check bounding box 
  if(  isOutBBox    ( point )  ) { return false ; }
  // check for radius 
  if( !insideR      ( point )  ) { return false ; }
  // check for theta
  if( !insideTheta  ( point )  ) { return false ; }
  // check for phi  
  if( !insidePhi    ( point )  ) { return false ; }
  //
  return true ;
};

// ===========================================================================
/** -# retrieve the pointer to "simplified" solid - "cover"
 *  -# implementation of ISolid abstract interface  
 *    The simplification scheme: 
 *  - for Model == 0 
 *     -# The cover for the general sphere segment is the clove 
 *         (remove gap in theta) 
 *     -# The cover for the clove is the sphere 
 *         (remove gap in phi) 
 *     -# The cover for the sphere is the ball 
 *         (set the inner radius to zero) 
 *     -# The cover for the ball is the box 
 *  - alternative model 
 *     -# The cover for general sphere segment is 
 *          the segment with inner radius equal to zero 
 *     -# The cover for the general sphere segment with 
 *        inner radius equal to zero is the 
 *        the sphere segemnt with no phi gap 
 *     -# The cover for the general sphere segment with 
 *         inner radius equal to zero and nophi gap 
 *         is the ball 
 *     -# the cover for the ball is the box   
 *  @see ISolid 
 *  @see SolidSphere::m_sphere_coverModel  
 *  @return pointer to "simplified" solid - "cover"
 */
// ===========================================================================
const ISolid* SolidSphere::cover () const 
{
  /// is cover already calculated? 
  if( 0 != m_cover ) { return m_cover; } 
  //  
  ISolid* cov = 0 ;
  if( 0 == m_sphere_coverModel ) 
    { 
      /** cover for sphere segment 
       *  is the segment with no gap in Theta 
       */
      if      ( 0.0   * Gaudi::Units::degree  != startThetaAngle() || 
                180.0 * Gaudi::Units::degree  != deltaThetaAngle()  )           
        { cov = 
            new SolidSphere("Cover for " + name () , 
                            outerRadius        () , 
                            insideRadius         () , 
                            startPhiAngle       () , 
                            deltaPhiAngle       () ); }
      /** cover for sphere segment with no gap in Theta is 
       *  the spehere segment with no gap in Phi */ 
      else if ( 0.0   * Gaudi::Units::degree  != startPhiAngle  () || 
                360.0 * Gaudi::Units::degree  != deltaPhiAngle  ()  )           
        { cov = 
            new SolidSphere("Cover for " + name () , 
                            outerRadius         () , 
                            insideRadius        () ); } 
      /** cover for sphere segment with no gap in Phi 
       *  is the sphere with null inside radius 
       */
      else if ( 0.0 != insideRadius() )
        { cov = 
            new SolidSphere("Cover for " + name () , 
                            outerRadius         () ); } 
      /** cover for sphere with null inside radius 
       *  is the box 
       */
      else                                           
        { cov = 
            new SolidBox   ("Cover for " + name () , 
                            outerRadius         () , 
                            outerRadius         () , 
                            outerRadius         () ); }
    }
  else
    {
      /** cover for sphere with not null inside radius is 
       *  the sphere with null inside radius 
       */
      if      ( 0.0 != insideRadius() )
        { cov = 
            new SolidSphere("Cover for " + name () , 
                            outerRadius         () ,  
                            0.0 * Gaudi::Units::mm , 
                            startPhiAngle       () , 
                            deltaPhiAngle       () , 
                            startThetaAngle     () , 
                            deltaThetaAngle     () , 
                            m_sphere_coverModel    ); } 
      /** cover for sphere with null inside radius
       *  is the sphere segment with no gap in phi 
       */
      else if ( 0.0   * Gaudi::Units::degree  != startPhiAngle  () || 
                360.0 * Gaudi::Units::degree  != deltaPhiAngle  ()  )           
        { cov = 
            new SolidSphere("Cover for " + name () , 
                            outerRadius         () , 
                            insideRadius        () , 
                            0.0 * Gaudi::Units::degree , 
                            360.0 * Gaudi::Units::degree , 
                            startThetaAngle     () , 
                            deltaThetaAngle     () ,  
                            m_sphere_coverModel    ); } 
      /** cover for sphere segment with no gap in phi is 
       *  the full sphere - ball 
       */
      else if ( 0.0   * Gaudi::Units::degree  != startThetaAngle() || 
                180.0 * Gaudi::Units::degree  != deltaThetaAngle()  )           
        { cov = 
            new SolidSphere("Cover for " + name () ,  
                            outerRadius         () , 
                            insideRadius        () , 
                            startPhiAngle       () , 
                            deltaPhiAngle       () ,
                            0.0 * Gaudi::Units::degree , 
                            180.0 * Gaudi::Units::degree , 
                            m_sphere_coverModel    ); }
      //    cover for full sphere is box 
      else
        { cov = 
            new SolidBox   ("Cover for " + name () , 
                            outerRadius         () , 
                            outerRadius         () , 
                            outerRadius         () ); }
    }
  //
  if( 0 == cov ) { return this; } 
  //
  m_cover = cov; 
  //
  return m_cover;
  //
};

// ============================================================================
/** -# calculate the intersection points("ticks") of the solid objects 
 *    with given line. 
 *  - Line is parametrized with parameter \a t :
 *     \f$ \vec{x}(t) = \vec{p} + t \times \vec{v} \f$ 
 *      - \f$ \vec{p} \f$ is a point on the line 
 *      - \f$ \vec{v} \f$ is a vector along the line  
 *  - \a tick is just a value of parameter \a t, at which the
 *    intersection of the solid and the line occurs
 *  - both  \a Point  (\f$\vec{p}\f$) and \a Vector  
 *    (\f$\vec{v}\f$) are defined in local reference system 
 *   of the solid 
 *  -# implementation of ISolid abstract interface  
 *  @see ISolid 
 *  @param Point initial point for the line
 *  @param Vector vector along the line
 *  @param ticks output container of "Ticks"
 *  @return the number of intersection points
 */
// ============================================================================
unsigned int SolidSphere::intersectionTicks( const Gaudi::XYZPoint& point,
                                             const Gaudi::XYZVector& vect,
                                             ISolid::Ticks&    ticks ) const 
{
  return intersectionTicksImpl(point, vect, ticks);
};
// ============================================================================
unsigned int SolidSphere::intersectionTicks( const Gaudi::Polar3DPoint& point,
                                             const Gaudi::Polar3DVector& vect,
                                             ISolid::Ticks&    ticks ) const 
{
  return intersectionTicksImpl(point, vect, ticks);
};
// ============================================================================
unsigned int SolidSphere::intersectionTicks( const Gaudi::RhoZPhiPoint& point,
                                             const Gaudi::RhoZPhiVector& vect, 
                                             ISolid::Ticks&    ticks ) const 
{
  return intersectionTicksImpl(point, vect, ticks);
};
// ============================================================================
template<class aPoint, class aVector>
unsigned int SolidSphere::intersectionTicksImpl( const aPoint&  point,
                                                 const aVector& vect,
                                                 ISolid::Ticks& ticks  ) const 
{
  ticks.clear();
  /// line with null direction vector in not able to intersect something
  if( vect.mag2() <= 0 ) { return 0; }   ///<  RETURN !!!
  ///  try to intersect with sphere outer radius
  if( 0 == 
      SolidTicks::LineIntersectsTheSphere( point                       , 
                                           vect                        , 
                                           outerRadius()               , 
                                           std::back_inserter( ticks ) ) ) 
    { return 0; }                         ///< RETURN !!!
  /// check for intersection with inner radius 
  if( insideRadius() > 0 ) 
    { SolidTicks::LineIntersectsTheSphere( point                       , 
                                           vect                        , 
                                           insideRadius ()             , 
                                           std::back_inserter( ticks ) ) ; }
  // check for phi angle 
  if( 0   * Gaudi::Units::degree != startPhiAngle() || 
      360 * Gaudi::Units::degree != deltaPhiAngle() ) 
    {
      SolidTicks::LineIntersectsThePhi( point                       , 
                                        vect                        , 
                                        startPhiAngle()             , 
                                        std::back_inserter( ticks ) ) ;  
      SolidTicks::LineIntersectsThePhi( point                       , 
                                        vect                        , 
                                        endPhiAngle()               , 
                                        std::back_inserter( ticks ) ) ;  
    }
  /// check for theta angle 
  if( 0   * Gaudi::Units::degree != startThetaAngle() || 
      180 * Gaudi::Units::degree != deltaThetaAngle() )
    {
      SolidTicks::LineIntersectsTheTheta( point                       , 
                                          vect                        , 
                                          startThetaAngle()           , 
                                          std::back_inserter( ticks ) ) ;  
      SolidTicks::LineIntersectsTheTheta( point                       , 
                                          vect                        , 
                                          endThetaAngle()             , 
                                          std::back_inserter( ticks ) ) ;  
    }
  /// sort and remove adjancent and some EXTRA ticks and return
  return SolidTicks::RemoveAdjancentTicks( ticks , 
                                           point , 
                                           vect  , 
                                           *this );    
};

// ============================================================================
/** - printout to STD/STL stream
 *  - implementation of ISolid abstract interface  
 *  - reimplementation of SolidBase::printOut( std::ostream& ) 
 *  @see ISolid 
 *  @see SolidBase 
 *  @param os STD/STL stream
 *  @return reference to the stream
 */
// ============================================================================
std::ostream&  SolidSphere::printOut      ( std::ostream&  os ) const
{
  /// serialize the base class
  SolidBase::printOut( os ) ;
  os << "outerRadius[mm]"          << DetDesc::print( outerRadius  () / Gaudi::Units::mm );
  if( 0 != insideR2() ) 
    { os << "innerRadius[mm]"      << DetDesc::print( insideRadius () / Gaudi::Units::mm ) ; }
  if( 0   * Gaudi::Units::degree != startThetaAngle ()  || 
      180 * Gaudi::Units::degree != deltaThetaAngle ()    )
    {  
      os << "startThetaAngle[deg]" 
         <<  DetDesc::print( startThetaAngle () / Gaudi::Units::degree ) ;
      os << "deltaThetaAngle[deg]" 
         <<  DetDesc::print( deltaThetaAngle () / Gaudi::Units::degree ) ;
    }
  if( 0   * Gaudi::Units::degree != startPhiAngle () || 
      360 * Gaudi::Units::degree != deltaPhiAngle () ) 
    {
      os << "startPhiAngle[deg]"   
         <<  DetDesc::print( startPhiAngle   () / Gaudi::Units::degree ) ;
      os << "deltaPhiAngle[deg]"   
         <<  DetDesc::print( deltaPhiAngle   () / Gaudi::Units::degree ) ;
    }
  ///
  return os << std::endl ;
};

// ============================================================================
/** - printout to Gaudi  stream
 *  - implementation of ISolid abstract interface  
 *  - reimplementation of SolidBase::printOut( MsgStream& ) 
 *  @see ISolid 
 *  @see SolidBase 
 *  @param os Gaudi stream
 *  @return reference to the stream
 */
// ============================================================================
MsgStream&     SolidSphere::printOut      ( MsgStream&     os ) const
{
  /// serialize the base class
  SolidBase::printOut( os ) ;
  os << "outerRadius[mm]"          << DetDesc::print( outerRadius  () / Gaudi::Units::mm );
  if( 0 != insideR2() ) 
    { os << "innerRadius[mm]"      << DetDesc::print( insideRadius () / Gaudi::Units::mm ) ; }
  if( 0   * Gaudi::Units::degree != startThetaAngle ()  || 
      180 * Gaudi::Units::degree != deltaThetaAngle ()    )
    {  
      os << "startThetaAngle[deg]" 
         <<  DetDesc::print( startThetaAngle () / Gaudi::Units::degree ) ;
      os << "deltaThetaAngle[deg]" 
         <<  DetDesc::print( deltaThetaAngle () / Gaudi::Units::degree ) ;
    }
  if( 0   * Gaudi::Units::degree != startPhiAngle () || 
      360 * Gaudi::Units::degree != deltaPhiAngle () ) 
    {
      os << "startPhiAngle[deg]"   
         <<  DetDesc::print( startPhiAngle   () / Gaudi::Units::degree ) ;
      os << "deltaPhiAngle[deg]"   
         <<  DetDesc::print( deltaPhiAngle   () / Gaudi::Units::degree ) ;
    }
  ///
  return os << endreq ;
};
// ============================================================================ 

// ============================================================================ 
// The END  
// ============================================================================ 



