// $Id: SolidSphere.cpp,v 1.10 2003-03-28 18:15:35 ibelyaev Exp $ 
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.9  2002/05/21 17:02:58  ibelyaev
//  bug fix for SolidSphere
//
// Revision 1.8  2002/05/11 18:25:48  ibelyaev
//  see $DETDESCROOT/doc/release.notes 11 May 2002
//
// ===========================================================================
// STD & STL 
#include <algorithm>
// CLHEP 
#include "CLHEP/Units/PhysicalConstants.h" 
#include "CLHEP/Geometry/Point3D.h" 
#include "CLHEP/Geometry/Vector3D.h" 
/// GaudiKernel
#include "GaudiKernel/IInspector.h" 
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
  if( -180.0 * degree > StartPhiAngle )
    { throw SolidException("SolidSphere::StartPhiAngle < -180 degree !"); }
  if(  360.0 * degree < StartPhiAngle )
    { throw SolidException("SolidSphere::StartPhiAngle >  360 degree !"); }
  if(    0.0 * degree > DeltaPhiAngle )
    { throw SolidException("SolidSphere::DeltaPhiAngle <    0 degree !"); }
  if(  360.0 * degree < StartPhiAngle+DeltaPhiAngle )
    { throw SolidException("SolidSphere::StartPhiAngle+DeltaPhiAngle>2pi"); }
  if(    0.0 * degree > StartThetaAngle )
    { throw SolidException("SolidSphere::StartThetaAngle < 0 degree !"); }
  if(  180.0 * degree < StartThetaAngle )
    { throw SolidException("SolidSphere::StartThetaAngle >  180 degree !"); }
  if(    0.0 * degree > DeltaThetaAngle )
    { throw SolidException("SolidSphere::DeltaThetaAngle <    0 degree !"); }
  if(  180.0 * degree < StartThetaAngle+DeltaThetaAngle )
    { throw SolidException("SolidSphere::StartThetaAngle+DeltaThetaAngle>pi");}
  /// 
  m_noPhiGap   = true ;
  if(   0 * degree != startPhiAngle   () ) { m_noPhiGap   = false ; }
  if( 360 * degree != deltaPhiAngle   () ) { m_noPhiGap   = false ; }
  m_noThetaGap = true ;
  if(   0 * degree != startThetaAngle () ) { m_noThetaGap = false ; }
  if( 180 * degree != deltaThetaAngle () ) { m_noThetaGap = false ; }  
  /// set bounding parameters 
  setBP();
  
};  

// ===========================================================================
/// destructor 
// ===========================================================================
SolidSphere::~SolidSphere() { reset(); };

// ============================================================================
/** set parameters for bounding solids (box, sphere and cylinder)
 *  @return status code 
 */
// ============================================================================
StatusCode SolidSphere::setBP() 
{
  // set bounding parameters of SolidBase class
  setRMax ( outerRadius() );
  
  { /// evaluate ZMin and ZMax
    typedef std::vector<double> TMP ;
    TMP tmp;
    tmp.push_back ( outerRadius  () * cos( startThetaAngle () ) ) ;
    tmp.push_back ( insideRadius () * cos( startThetaAngle () ) ) ;
    tmp.push_back ( outerRadius  () * cos( startThetaAngle () + 
                                           deltaThetaAngle () ) ) ;
    tmp.push_back ( insideRadius () * cos( startThetaAngle () + 
                                           deltaThetaAngle () ) ) ;
    //
    setZMax ( *std::max_element( tmp.begin () , tmp.end () ) ) ;
    setZMin ( *std::min_element( tmp.begin () , tmp.end () ) ) ;    
  };
  
  // evaluate rho max 
  if(      startThetaAngle()                        <=   90.0 * degree 
           && startThetaAngle() + deltaThetaAngle() >=   90.0 * degree  )
    { setRhoMax ( rMax () ) ; }
  else 
    {
      const double r1     = sin( startThetaAngle()                     );
      const double r2     = sin( startThetaAngle() + deltaThetaAngle() );
      const double rhomax = ( r1 > r2 ? r1 : r2 ) * rMax() ;
      setRhoMax ( rhomax  ) ;
    }
  
  // evaluate xmax 
  if(      startPhiAngle()                          <=    0.0 * degree 
           && startPhiAngle() + deltaPhiAngle()     >=    0.0 * degree )
    { setXMax (  rhoMax () ) ; }
  else if( startPhiAngle()                          <=  360.0 * degree 
           && startPhiAngle() + deltaPhiAngle()     >=  360.0 * degree )
    { setXMax (  rhoMax () ) ; }
  else
    {
      const double x1   = cos( startPhiAngle()                   );
      const double x2   = cos( startPhiAngle() + deltaPhiAngle() );
      const double xmax = ( x1 > x2 ? x1 : x2 ) * rhoMax ()  ;
      setXMax ( xmax       ) ;
    }
  // evaluate xmin 
  if(      startPhiAngle()                          <=  180.0 * degree 
           && startPhiAngle() + deltaPhiAngle()     >=  180.0 * degree )
    { setXMin ( -rhoMax()  ) ; }
  else if( startPhiAngle()                          <= -180.0 * degree 
           && startPhiAngle() + deltaPhiAngle()     >= -180.0 * degree )
    { setXMin ( -rhoMax()  ) ; }
  else 
    {
      const double x1   = cos( startPhiAngle()                   );
      const double x2   = cos( startPhiAngle() + deltaPhiAngle() );
      const double xmin = ( x1 < x2 ? x1 : x2 ) * rhoMax ()  ;
      setXMin ( xmin       ) ;
    }
  // evaluate ymax 
  if(      startPhiAngle()                          <=   90.0 * degree 
           && startPhiAngle() + deltaPhiAngle()     >=   90.0 * degree )
    { setYMax (  rhoMax () ) ; }
  else
    {
      const double y1   = sin( startPhiAngle()                   );
      const double y2   = sin( startPhiAngle() + deltaPhiAngle() );
      const double ymax = ( y1 > y2 ? y1 : y2 ) * rhoMax ()  ;
      setYMax ( ymax       ) ;
    }
  // evaluate ymin 
  if(      startPhiAngle()                          <=  -90.0 * degree 
           && startPhiAngle() + deltaPhiAngle()     >=  -90.0 * degree )
    { setYMin ( -rhoMax () ) ; }
  else if( startPhiAngle()                          <=  270.0 * degree 
           && startPhiAngle() + deltaPhiAngle()     >=  270.0 * degree )
    { setYMin ( -rhoMax () ) ; }
  else
    {
      const double y1   = sin( startPhiAngle()                   );
      const double y2   = sin( startPhiAngle() + deltaPhiAngle() );
      const double ymin = ( y1 < y2 ? y1 : y2 ) * rhoMax ()  ;
      setYMin ( ymin       ) ;
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
  , m_sphere_deltaPhiAngle   ( 360.0 * degree   ) 
  , m_sphere_startThetaAngle ( 0.0              ) 
  , m_sphere_deltaThetaAngle ( 180.0 * degree   )
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
bool  SolidSphere::isInside( const HepPoint3D & point) const
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
/** - serialization for reading
 *  - implementation of ISerialize abstract interface 
 *  - reimplementation of SolidBase::serialize 
 *  @see ISerialize 
 *  @see ISolid  
 *  @see SolidBase   
 *  @param s reference to stream buffer
 *  @return reference to stream buffer
 */
// ===========================================================================
StreamBuffer& SolidSphere::serialize( StreamBuffer& s ) 
{
  /// reset the sphere segment
  reset();
  /// serialialize the base class
  SolidBase::serialize( s );  
  s >>  m_sphere_outerR2    
    >>  m_sphere_insideR2     
    >>  m_sphere_startPhiAngle  
    >>  m_sphere_deltaPhiAngle   
    >>  m_sphere_startThetaAngle 
    >>  m_sphere_deltaThetaAngle 
    >>  m_sphere_coverModel  ;    
  ///
  if( 0 >= outerR2() ) 
    { throw SolidException("SolidSphere::OuterRadius2  is not positive!"); } 
  if( 0 >  insideR2() ) 
    { throw SolidException("SolidSphere::InsideRadius2 is negative!    "); }
  if( insideRadius() >= outerRadius() ) 
    { throw SolidException("SolidSphere::InsideRadius>=OuterRadius    "); }
  if( -180.0 * degree > startPhiAngle() )
    { throw SolidException("SolidSphere::StartPhiAngle < -180 degree !"); }
  if(  360.0 * degree < startPhiAngle() )
    { throw SolidException("SolidSphere::StartPhiAngle >  360 degree !"); }
  if(    0.0 * degree > deltaPhiAngle() )
    { throw SolidException("SolidSphere::DeltaPhiAngle <    0 degree !"); }
  if(  360.0 * degree < startPhiAngle()+deltaPhiAngle() )
    { throw SolidException("SolidSphere::StartPhiAngle+DeltaPhiAngle>2pi"); }
  if(    0.0 * degree > startThetaAngle() )
    { throw SolidException("SolidSphere::StartThetaAngle < 0 degree !"); }
  if(  180.0 * degree < startThetaAngle() )
    { throw SolidException("SolidSphere::StartThetaAngle >  180 degree !"); }
  if(    0.0 * degree > deltaThetaAngle() )
    { throw SolidException("SolidSphere::DeltaThetaAngle <    0 degree !"); }
  if(  180.0 * degree < startThetaAngle()+deltaThetaAngle() )
    { throw SolidException("SolidSphere::StartThetaAngle+DeltaThetaAngle>pi");}
  ///
  m_noPhiGap   = true ;
  if(   0 * degree != startPhiAngle   () ) { m_noPhiGap   = false ; }
  if( 360 * degree != deltaPhiAngle   () ) { m_noPhiGap   = false ; }
  m_noThetaGap = true ;
  if(   0 * degree != startThetaAngle () ) { m_noThetaGap = false ; }
  if( 180 * degree != deltaThetaAngle () ) { m_noThetaGap = false ; }  
  /// set bounding parameters 
  setBP();
  ///
  return s;
};

// ===========================================================================
/** - serialization for writing
 *  - implementation of ISerialize abstract interface 
 *  - reimplementation of SolidBase::serialize 
 *  @see ISerialize 
 *  @see ISolid  
 *  @see SolidBase   
 *  @param s reference to stream buffer
 *  @return reference to stream buffer
 */
// ===========================================================================
StreamBuffer& SolidSphere::serialize( StreamBuffer& s ) const
{
  /// serialialize the base class
  SolidBase::serialize( s );   
  return s <<  m_sphere_outerR2    
           <<  m_sphere_insideR2    
           <<  m_sphere_startPhiAngle  
           <<  m_sphere_deltaPhiAngle   
           <<  m_sphere_startThetaAngle 
           <<  m_sphere_deltaThetaAngle 
           <<  m_sphere_coverModel    ;
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
      if      ( 0.0   * degree  != startThetaAngle() || 
                180.0 * degree  != deltaThetaAngle()  )           
        { cov = 
            new SolidSphere("Cover for " + name () , 
                            outerRadius        () , 
                            insideRadius         () , 
                            startPhiAngle       () , 
                            deltaPhiAngle       () ); }
      /** cover for sphere segment with no gap in Theta is 
       *  the spehere segment with no gap in Phi */ 
      else if ( 0.0   * degree  != startPhiAngle  () || 
                360.0 * degree  != deltaPhiAngle  ()  )           
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
                            0.0 * mm               , 
                            startPhiAngle       () , 
                            deltaPhiAngle       () , 
                            startThetaAngle     () , 
                            deltaThetaAngle     () , 
                            m_sphere_coverModel    ); } 
      /** cover for sphere with null inside radius
       *  is the sphere segment with no gap in phi 
       */
      else if ( 0.0   * degree  != startPhiAngle  () || 
                360.0 * degree  != deltaPhiAngle  ()  )           
        { cov = 
            new SolidSphere("Cover for " + name () , 
                            outerRadius         () , 
                            insideRadius        () , 
                            0.0 * degree           , 
                            360.0 * degree         , 
                            startThetaAngle     () , 
                            deltaThetaAngle     () ,  
                            m_sphere_coverModel    ); } 
      /** cover for sphere segment with no gap in phi is 
       *  the full sphere - ball 
       */
      else if ( 0.0   * degree  != startThetaAngle() || 
                180.0 * degree  != deltaThetaAngle()  )           
        { cov = 
            new SolidSphere("Cover for " + name () ,  
                            outerRadius         () , 
                            insideRadius        () , 
                            startPhiAngle       () , 
                            deltaPhiAngle       () ,
                            0.0 * degree           , 
                            180.0 * degree         , 
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
unsigned int 
SolidSphere::intersectionTicks 
( const HepPoint3D&  point  ,       
  const HepVector3D& vect   ,      
  ISolid::Ticks&     ticks  ) const 
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
  if( 0   * degree != startPhiAngle() || 
      360 * degree != deltaPhiAngle() ) 
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
  if( 0   * degree != startThetaAngle() || 
      180 * degree != deltaThetaAngle() )
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
  os << "outerRadius[mm]"          << DetDesc::print( outerRadius  () / mm );
  if( 0 != insideR2() ) 
    { os << "innerRadius[mm]"      << DetDesc::print( insideRadius () / mm ) ; }
  if( 0   * degree != startThetaAngle ()  || 
      180 * degree != deltaThetaAngle ()    )
    {  
      os << "startThetaAngle[deg]" 
         <<  DetDesc::print( startThetaAngle () / degree ) ;
      os << "deltaThetaAngle[deg]" 
         <<  DetDesc::print( deltaThetaAngle () / degree ) ;
    }
  if( 0   * degree != startPhiAngle () || 
      360 * degree != deltaPhiAngle () ) 
    {
      os << "startPhiAngle[deg]"   
         <<  DetDesc::print( startPhiAngle   () / degree ) ;
      os << "deltaPhiAngle[deg]"   
         <<  DetDesc::print( deltaPhiAngle   () / degree ) ;
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
  os << "outerRadius[mm]"          << DetDesc::print( outerRadius  () / mm );
  if( 0 != insideR2() ) 
    { os << "innerRadius[mm]"      << DetDesc::print( insideRadius () / mm ) ; }
  if( 0   * degree != startThetaAngle ()  || 
      180 * degree != deltaThetaAngle ()    )
    {  
      os << "startThetaAngle[deg]" 
         <<  DetDesc::print( startThetaAngle () / degree ) ;
      os << "deltaThetaAngle[deg]" 
         <<  DetDesc::print( deltaThetaAngle () / degree ) ;
    }
  if( 0   * degree != startPhiAngle () || 
      360 * degree != deltaPhiAngle () ) 
    {
      os << "startPhiAngle[deg]"   
         <<  DetDesc::print( startPhiAngle   () / degree ) ;
      os << "deltaPhiAngle[deg]"   
         <<  DetDesc::print( deltaPhiAngle   () / degree ) ;
    }
  ///
  return os << endreq ;
};
// ============================================================================ 

// ============================================================================ 
// The END  
// ============================================================================ 



