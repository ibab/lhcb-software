// ============================================================================
/// CVStag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.5  2001/08/09 16:48:03  ibelyaev
/// update in interfaces and redesign of solids
/// 
// ============================================================================
/// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"
///@{
/** DetDesc includes */ 
#include "DetDesc/SolidTubs.h"
#include "DetDesc/SolidBox.h"
#include "DetDesc/SolidTicks.h"
#include "DetDesc/SolidException.h"
///@}

// ============================================================================
/** @file SolidTubs.cpp 
 * 
 *  implementation of class SolidTubs 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date xx/xx/xx 
 */
// ============================================================================

// ============================================================================
/** constructor
 *  @param name           name of tube segment 
 *  @param ZHalfLength    half-length of the tube segment
 *  @param OuterRadius    outer radius of tube segment 
 *  @param InnerRadius    inner radius of tube segment 
 *  @param StartPhiAngle  start phi angle 
 *  @param DeltaPhiAngle  delta phi angle  
 *  @param CoverModel     covering model   
 *  @exception SolidException wrong parameter range 
 */
// ============================================================================
SolidTubs::SolidTubs( const std::string& name          ,
                      const double       ZHalfLength   , 
                      const double       OuterRadius   ,
                      const double       InnerRadius   , 
                      const double       StartPhiAngle , 
                      const double       DeltaPhiAngle , 
                      const int          CoverModel    )  
  : SolidBase             ( name          )
  , m_tubs_zHalfLength    ( ZHalfLength   )
  , m_tubs_outerRadius    ( OuterRadius   )
  , m_tubs_innerRadius    ( InnerRadius   )
  , m_tubs_startPhiAngle  ( StartPhiAngle )
  , m_tubs_deltaPhiAngle  ( DeltaPhiAngle )
  , m_tubs_coverModel     ( CoverModel    )
{
  if( 0 >= ZHalfLength )
    { throw SolidException("SolidTubs::ZHalfLength is not positive!"); } 
  if( 0 >= OuterRadius )
    { throw SolidException("SolidTubs::OuterRadius is not positive!"); } 
  if( 0 >  InnerRadius ) 
    { throw SolidException("SolidTubs::InnerRadius is negative    !"); } 
  if( InnerRadius >= OuterRadius ) 
    { throw SolidException("SolidTubs::InnerRadius >= OuterRadius !"); } 
  if( -180.0 * degree > StartPhiAngle ) 
    { throw SolidException("SolidTubs::StartPhiAngle is < -180 degree! "); } 
  if(  360.0 * degree < StartPhiAngle ) 
    { throw SolidException("SolidTubs::StartPhiAngle is >  360 degree! "); } 
  if(    0.0 * degree > DeltaPhiAngle ) 
    { throw SolidException("SolidTubs::DeltaPhiAngle is <    0 degree! "); } 
  if(  360.0 * degree < StartPhiAngle + DeltaPhiAngle ) 
    { throw SolidException("SolidTubs::StartPhiAngle+DeltaPhiAngle >2pi"); } 
  //
};

// ============================================================================
/** constructor 
 *  @param name name of tube segment 
 */
// ============================================================================
SolidTubs::SolidTubs( const std::string& name )
  : SolidBase             ( name          )
  , m_tubs_zHalfLength    ( 10000000      )
  , m_tubs_outerRadius    ( 10000000      )
  , m_tubs_innerRadius    ( 0             )
  , m_tubs_startPhiAngle  ( 0             )                         
  , m_tubs_deltaPhiAngle  ( 360*degree    )                         
  , m_tubs_coverModel     (        0      )
{};

// ============================================================================
/// destructor 
// ============================================================================
SolidTubs::~SolidTubs() { reset(); };

// ============================================================================
/** - check for the given 3D-point. 
 *    Point coordinated are in the local reference 
 *    frame of the solid.   
 *  - implementation of ISolid absstract interface  
 *  @see ISolid 
 *  @param point point (in local reference system of the solid)
 *  @return true if the point is inside the solid
 */
// ============================================================================
bool  SolidTubs::isInside( const HepPoint3D & point ) const
{
  /// check Z 
  if( abs(point.z()) > zHalfLength() ) { return false; }
  /// check R 
  const double rho = point.perp();
  if( rho > outerRadius() ) { return false; }
  if( rho < innerRadius() ) { return false; }
  // check phi 
  double phi = point.phi();
  if( phi < 0 ) { phi += 360.0*degree; } 
  if( phi < startPhiAngle()                 ) { return false; }
  if( phi > startPhiAngle()+deltaPhiAngle() ) { return false; }
  //
  return true; 
};

// ============================================================================
/** - serialization for reading
 *  - implementation of ISerialize abstract interface 
 *  - reimplementation of SolidBase::serialize 
 *  @see ISerialize 
 *  @see ISolid  
 *  @see SolidBase   
 *  @param      s               reference to stream buffer
 *  @exception  SolidException  wrong parameters range 
 *  @return reference to stream buffer
 */
// ============================================================================
StreamBuffer& SolidTubs::serialize( StreamBuffer& s ) 
{
  /// reset
  reset();
  /// serialise teh baze class 
  SolidBase::serialize( s );
  /// serialize the members 
  s >> m_tubs_zHalfLength    
    >> m_tubs_outerRadius    
    >> m_tubs_innerRadius    
    >> m_tubs_startPhiAngle  
    >> m_tubs_deltaPhiAngle  
    >> m_tubs_coverModel    ;
  ///
  if( 0 >= zHalfLength() )
    { throw SolidException("SolidTubs::ZHalfLength is not positive!"); } 
  if( 0 >= outerRadius() )
    { throw SolidException("SolidTubs::OuterRadius is not positive!"); } 
  if( 0 >  innerRadius() ) 
    { throw SolidException("SolidTubs::InnerRadius is negative    !"); } 
  if( innerRadius() >= outerRadius() ) 
    { throw SolidException("SolidTubs::InnerRadius >= OuterRadius !"); } 
  if( -180.0 * degree > startPhiAngle() ) 
    { throw SolidException("SolidTubs::StartPhiAngle is < -180 degree! "); } 
  if(  360.0 * degree < startPhiAngle() ) 
    { throw SolidException("SolidTubs::StartPhiAngle is >  360 degree! "); } 
  if(    0.0 * degree > deltaPhiAngle() ) 
    { throw SolidException("SolidTubs::DeltaPhiAngle is <    0 degree! "); } 
  if(  360.0 * degree < startPhiAngle() + deltaPhiAngle() ) 
    { throw SolidException("SolidTubs::StartPhiAngle+DeltaPhiAngle>2pi"); } 
  ///
  return s;
};

// ============================================================================
/** - serialization for writing
 *  - implementation of ISerialize abstract interface 
 *  - reimplementation of SolidBase::serialize 
 *  @see ISerialize 
 *  @see ISolid  
 *  @see SolidBase   
 *  @param s reference to stream buffer
 *  @return reference to stream buffer
 */
// ============================================================================
StreamBuffer& SolidTubs::serialize( StreamBuffer& s ) const
{
  /// serialize the base class
  SolidBase::serialize( s ) ;
  return 
    s << m_tubs_zHalfLength    
      << m_tubs_outerRadius    
      << m_tubs_innerRadius    
      << m_tubs_startPhiAngle  
      << m_tubs_deltaPhiAngle  
      << m_tubs_coverModel    ;
};

// ============================================================================
/** retrieve the pointer to "the most simplified cover" 
 *    -# for Model = 0 
 *      - the cover for general tube segment is tube 
 *      - the cover for tube  is the cylinder 
 *      - the cover for cylinder is the box 
 *    -#  for Model = 1 
 *      - the cover for general tube segment is teh cylinder segment 
 *      - the cover for cylinder segment is cylinder 
 *      - the cover for cylinder is the box 
 *  implementation  of ISolid abstract interface 
 *  @see ISolid 
 *  @return pointer to the most simplified cover 
 */
// ============================================================================
const ISolid* SolidTubs::cover () const 
{
  /// cover is calculated already 
  if( 0 != m_cover ) { return m_cover; } 
  //
  ISolid* cov = 0 ;  
  if( 0 == m_tubs_coverModel ) 
    { 
      if( 0.0   * degree  != startPhiAngle() || 
          360.0 * degree  != deltaPhiAngle()    ) 
        { cov = 
            new SolidTubs("Cover for " + name () , 
                          zHalfLength         () , 
                          outerRadius         () , 
                          innerRadius         () ); }
      else if ( 0.0 != innerRadius() )                              
        { cov = 
            new SolidTubs("Cover for " + name () , 
                          zHalfLength         () , 
                          outerRadius         () ); }
      else                                                          
        { cov = 
            new SolidBox ("Cover for " + name () , 
                          outerRadius         () , 
                          outerRadius         () , 
                          zHalfLength         () ); }
    }
  else 
    {
      if ( 0.0 != innerRadius() )                              
        { cov = 
            new SolidTubs("Cover for " + name () , 
                          zHalfLength         () , 
                          outerRadius         () , 
                          0.0 * mm               , 
                          startPhiAngle       () , 
                          deltaPhiAngle       () , 
                          m_tubs_coverModel      ); }
      else if( 0.0   * degree  != startPhiAngle() || 
               360.0 * degree  != deltaPhiAngle()    ) 
        { cov = 
            new SolidTubs("Cover for " + name () , 
                          zHalfLength         () , 
                          outerRadius         () , 
                          innerRadius         () ,
                          0.0 * degree           , 
                          360.0 * degree         , 
                          m_tubs_coverModel      ); }
      else                                                          
        { cov = 
            new SolidBox ("Cover for " + name () , 
                          outerRadius         () , 
                          outerRadius         () , 
                          zHalfLength         () ); }
    }
  if( 0 == cov ) { return this; } 
  m_cover = cov ; 
  return m_cover;
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
 *  @param point  initial point for the line
 *  @param vect   vector along the line
 *  @param ticks  output container of "Ticks"
 *  @return the number of intersection points
 */
// ============================================================================
unsigned int 
SolidTubs::intersectionTicks ( const HepPoint3D&  point  ,        
                               const HepVector3D& vect   ,       
                               ISolid::Ticks   &  ticks  ) const 
{
  /// clear the container 
  ticks.clear(); 
  /// line with null direction vector is not able to intersect any solid. 
  if( vect.mag2() == 0 )                  { return 0 ; } ///< RETURN! 
  // find intersection points ("ticks") with cylinder (outer radius) 
  SolidTicks::LineIntersectsTheCylinder( point                       , 
                                         vect                        ,
                                         outerRadius()               , 
                                         std::back_inserter( ticks ) ); 
  // find intersection points("ticks") with cylinder (inner radius) 
  if( innerRadius() > 0 ) 
    { SolidTicks::LineIntersectsTheCylinder( point                       , 
                                             vect                        , 
                                             innerRadius()               , 
                                             std::back_inserter( ticks ) ); }
  // find intersection points("ticks") with z-planes 
  SolidTicks::LineIntersectsTheZ( point , 
                                  vect  , -1.0 * zHalfLength() , 
                                  std::back_inserter( ticks ) ); 
  SolidTicks::LineIntersectsTheZ( point , 
                                  vect ,        zHalfLength() , 
                                  std::back_inserter( ticks ) ); 
  ///
  if( 0   * degree !=  startPhiAngle() || 
      360 * degree !=  deltaPhiAngle()   )
    {
      SolidTicks::LineIntersectsThePhi( point                       , 
                                        vect                        , 
                                        startPhiAngle()             , 
                                        std::back_inserter( ticks ) );
      SolidTicks::LineIntersectsThePhi( point                             , 
                                        vect                              , 
                                        startPhiAngle() + deltaPhiAngle() , 
                                        std::back_inserter( ticks )       );
    }
  /// sort and remove adjancent and some EXTRA ticks and return 
  return SolidTicks::RemoveAdjancentTicks( ticks , 
                                           point , 
                                           vect  , 
                                           *this );    
};

// ============================================================================
/** - printout to STD/STL stream    
 *  - implementation  of ISolid abstract interface 
 *  - reimplementation of SolidBase::printOut( std::ostream& )
 *  @see SolidBase 
 *  @see ISolid 
 *  @param os STD/STL stream
 *  @return reference to the stream 
 */
// ============================================================================
std::ostream&  SolidTubs::printOut      ( std::ostream&  os ) const
{
  /// serialize the base
  SolidBase::printOut( os ) ;
  return 
    os << "["
       << " sizeZ[mm]="        << zLength     () / millimeter 
       << " outerRadius[mm]="  << outerRadius () / millimeter ;
  if( 0 != innerRadius() ) 
    { os << " innerRadius[mm]=" << innerRadius() / millimeter ; }
  if( 0.0   * degree != startPhiAngle() || 
      360.0 * degree != deltaPhiAngle()   )
    {
      os << " startPhiAngle[deg]=" << startPhiAngle () / degree 
         << " deltaPhiAngle[deg]=" << deltaPhiAngle () / degree ;
    }
  return os << "]";
};

// ============================================================================
/** - printout to Gaudi MsgStream stream    
 *  - implementation  of ISolid abstract interface 
 *  - reimplementation of SolidBase::printOut( MsgStream& )
 *  @see SolidBase 
 *  @see ISolid 
 *  @param os Gaudi MsgStream  stream
 *  @return reference to the stream 
 */
// ============================================================================
MsgStream&     SolidTubs::printOut      ( MsgStream&     os ) const
{
  /// serialize the base
  SolidBase::printOut( os ) ;
  return 
    os << "["
       << " sizeZ[mm]="        << zLength     () / millimeter 
       << " outerRadius[mm]="  << outerRadius () / millimeter ;
  if( 0 != innerRadius() ) 
    { os << " innerRadius[mm]=" << innerRadius() / millimeter ; }
  if( 0.0   * degree != startPhiAngle() || 
      360.0 * degree != deltaPhiAngle()   )
    {
      os << " startPhiAngle[deg]=" << startPhiAngle () / degree 
         << " deltaPhiAngle[deg]=" << deltaPhiAngle () / degree ;
    }
  return os << "]";
};

// ============================================================================

